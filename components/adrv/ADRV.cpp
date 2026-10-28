/**
 * @file ADRV.cpp
 * @brief Implementation of High-Level Transceiver Abstraction Helper.
 */

#include "ADRV.h"

/**
 * @brief Default constructor for adrv helper.
 */
adrv::adrv()
{
}

/**
 * @brief Destructor releasing allocated DAC manager.
 */
adrv::~adrv()
{
    if (m_dac_data_manager) {
        delete m_dac_data_manager;
        m_dac_data_manager = nullptr;
    }
}

/**
 * @brief Connects to target board with specified IP or resolves existing context.
 * @param ip Target board IP address string.
 * @return True if context is active.
 */
bool adrv::connect(QString ip)
{
    Q_UNUSED(ip);
    return globals::ctx != nullptr;
}

/**
 * @brief Sets the RF Local Oscillator frequency in MHz.
 * @param freq Frequency in MHz.
 * @return True on success.
 */
bool adrv::SetFrequency(double freq)
{
    if (!globals::ctx) return false;

    struct iio_device *dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!dev) return false;

    struct iio_channel *ch1 = iio_device_find_channel(dev, "altvoltage0", true);
    if (!ch1) return false;

    const char *freq_name = iio_channel_find_attr(ch1, "frequency") ? "frequency" : "RX_LO_frequency";
    long long freq_hz = (long long)(freq * mhz_scale);

    iio_channel_attr_write_longlong(ch1, freq_name, freq_hz);
    return true;
}

/**
 * @brief Configures and runs frequency hopping loop.
 * @param start Start frequency in MHz.
 * @param stop Stop frequency in MHz.
 * @param step Frequency step size in MHz.
 * @return True on success.
 */
bool adrv::SetFrequencyHopping(double start, double stop, double step)
{
    if (!globals::ctx) return false;

    struct iio_device *dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!dev) return false;

    struct iio_channel *ch1 = iio_device_find_channel(dev, "altvoltage0", true);
    if (!ch1) return false;

    future = QtConcurrent::run([=] {
        double next = start;
        while (hopping) {
            iio_channel_attr_write_bool(ch1, "frequency_hopping_mode_enable", true);
            for (int i = 0; i < 2; i++) {
                iio_channel_attr_write_longlong(ch1, "frequency_hopping_mode", (long long)(next * mhz_scale));
            }
            emit frequencyChanged();
            if (next <= stop)
                next += step;
            else
                next = start;
            QThread::msleep(1);
        }
    });

    return true;
}

/**
 * @brief Reads current RF carrier frequency in MHz from transceiver PHY.
 * @return Frequency in MHz.
 */
double adrv::GetFrequency()
{
    if (!globals::ctx) return 0.0;

    struct iio_device *dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!dev) return 0.0;

    struct iio_channel *ch1 = iio_device_find_channel(dev, "altvoltage0", true);
    if (!ch1) return 0.0;

    const char *freq_name = iio_channel_find_attr(ch1, "frequency") ? "frequency" : "TRX_LO_frequency";
    char buf[0x100] = {0};
    ssize_t ret = iio_channel_attr_read(ch1, freq_name, buf, sizeof(buf));
    if (ret <= 0) return 0.0;

    char *end = nullptr;
    double freq_val = g_ascii_strtod(buf, &end);
    freq_val /= mhz_scale;
    return freq_val;
}

/**
 * @brief Reads current hardware gain / attenuation.
 * @return Gain in dB.
 */
double adrv::GetPower()
{
    if (!globals::ctx) return 0.0;

    struct iio_device *dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!dev) return 0.0;

    struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage0", true);
    if (!ch1) return 0.0;

    const char *attr_name = "hardwaregain";
    char buf[0x100] = {0};
    ssize_t ret = iio_channel_attr_read(ch1, attr_name, buf, sizeof(buf));
    if (ret <= 0) return 0.0;

    char *end = nullptr;
    double power_val = g_ascii_strtod(buf, &end);
    power_val *= inv_scale;
    return power_val;
}

/**
 * @brief Sets hardware gain / attenuation on TX voltage0 channel.
 * @param power Gain value to program.
 * @return True on success.
 */
bool adrv::SetPower(double power)
{
    if (!globals::ctx) return false;

    struct iio_device *dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!dev) return false;

    struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage0", true);
    if (!ch1) return false;

    const char *attr_name = "hardwaregain";
    double p = power * inv_scale;
    iio_channel_attr_write_double(ch1, attr_name, p);
    return true;
}

/**
 * @brief Initializes DAC buffer manager for cyclic DMA sample playback.
 * @return Layout pointer (nullptr if no layout needed).
 */
QHBoxLayout *adrv::InitializeDacBuffer()
{
    if (!globals::ctx) return nullptr;

    dds = iio_context_find_device(globals::ctx, DDS_DEVICE);
    if (dds && !m_dac_data_manager) {
        m_dac_data_manager = new dac_data_manager(dds, true, true, true, true, true);
    }
    return nullptr;
}

/**
 * @brief Streams IQ sample file into DAC DMA buffer.
 * @param fileName Path to sample file.
 * @param scale Digital scaling multiplier.
 * @return Status description message.
 */
QString adrv::SetFile(QString fileName, double scale)
{
    if (m_dac_data_manager)
        return m_dac_data_manager->setFile(fileName, scale);
    return QStringLiteral("DAC buffer initialized");
}

/**
 * @brief Stops DAC DMA buffer playback.
 */
void adrv::DisableDac()
{
    if (m_dac_data_manager)
        m_dac_data_manager->DisableDac();
}

/**
 * @brief Resolves IIO context from URI or local device.
 * @param ip Optional IP URI string.
 * @return IIO context handle.
 */
iio_context *adrv::GetContext(QString ip)
{
    if (!ip.isEmpty()) {
        const QByteArray hostname = ip.toLocal8Bit();
        if (globals::ctx && !g_strcmp0(hostname.constData(), iio_context_get_attr_value(globals::ctx, "uri")))
            return globals::ctx;
        return iio_create_context_from_uri(hostname.constData());
    } else {
        return iio_create_local_context();
    }
}
