#include "ADRV.h"

#pragma region Constructor {

/**
 * @brief adrv::adrv
 */
adrv::adrv()
{

}

#pragma endregion }

#pragma region Public Functions {

/**
 * @brief connect to board with specified Ip
 * @param ip : Board Ip
 * @return true if no problem detect
 */
bool adrv::connect(QString ip)
{
//    ctx = GetContext("ip:"+ip);

//    emit thisIsTest(ctx);

    return ctx != nullptr;
}


bool adrv::SetFrequency(double freq){

    struct iio_device *dev;
    struct iio_channel *ch1;
    const char *freq_name;

    dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    ch1 = iio_device_find_channel(dev, "altvoltage0", true);

    if (iio_channel_find_attr(ch1, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "RX_LO_frequency";

    freq*=mhz_scale;

    iio_channel_attr_write_longlong(ch1,freq_name, (long long) freq);

    return true;
}

/**
 * @brief adrv::SetFrequencyHopping
 * @param freq
 * @return
 */
bool adrv::SetFrequencyHopping(double start, double stop, double step)
{
    struct iio_device *dev;
    struct iio_channel *ch1;

    dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    ch1 = iio_device_find_channel(dev, "altvoltage0", true);

    future=QtConcurrent::run([=]{

        double next=start;

        while(hopping){

            iio_channel_attr_write_bool(ch1,"frequency_hopping_mode_enable",true);

            for(int i=0;i<2;i++)
            {
                iio_channel_attr_write_longlong(ch1,"frequency_hopping_mode", next*mhz_scale);
            }

            emit frequencyChanged();

            if(next<=stop)
                next+=step;
            else
                next=start;

            QThread::msleep(1);

//            qInfo()<<GetFrequency()<<" -> "<<next;
        }

    });

    return true;
}

/**
 * @brief Get current frequency
 * @return frequency Changed status
 */
double adrv::GetFrequency()
{
    struct iio_device *dev;
    struct iio_channel *ch1,*ch2;
    const char *freq_name;

    dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    ch1 = iio_device_find_channel(dev, "altvoltage0", true);
//    ch2 = iio_device_find_channel(dev, "voltage0", true);

    if (iio_channel_find_attr(ch1, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "TRX_LO_frequency";

    ssize_t ret;
    char buf[0x100];

    if (ch1) ret = iio_channel_attr_read(ch1,freq_name, buf, sizeof(buf));

    char *end;

    baseFreq = g_ascii_strtod(buf, &end);
    baseFreq/=1000000.0;

    return baseFreq;
}


double adrv::GetPower()
{
    struct iio_device *dev;
    struct iio_channel *ch1;
    const char *attr_name;

    dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    ch1 = iio_device_find_channel(dev, "voltage0", true);

    attr_name="hardwaregain";

    char buf[0x100];

    if (ch1)
        iio_channel_attr_read(ch1,attr_name, buf, sizeof(buf));

    char *end;

    power = g_ascii_strtod(buf, &end);

    power*=inv_scale;

    return power;
}

bool adrv::SetPower(double power)
{
    struct iio_device *dev;
    struct iio_channel *ch1;
    const char *attr_name="hardwaregain";

    dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    ch1 = iio_device_find_channel(dev, "voltage0", true);

    power*=inv_scale;

    iio_channel_attr_write_double(ch1,attr_name, power);

    return true;
}

QHBoxLayout *adrv::InitializeDacBuffer()
{
    dds = iio_context_find_device(globals::ctx, DDS_DEVICE);
    dac_data_manager=new dacDataManager();

    if (dds)
    {
//        dac_tx_manager = dac_data_manager->dac_data_manager_new(dds, NULL, globals::ctx);
        dac_data_manager->dac_data_manager_set_buffer_size_alignment(dac_tx_manager, 16);
    }

    return dac_data_manager->dac_data_manager_get_gui_container(dac_tx_manager);
}


QString adrv::SetFile(QString fileName, double scale)
{
    return dac_data_manager->setFile(fileName,scale);
}


void adrv::DisableDac()
{
    dac_data_manager->DisableDac();
}

#pragma endregion }

#pragma region Private Functions {


iio_context *adrv::GetContext(QString ip)
{
    // Manual Scan Mode
    if (ip != "") {
        char *hostname=ip.toLocal8Bit().data();

        if (globals::ctx && !g_strcmp0(hostname, iio_context_get_attr_value(globals::ctx, "uri")))
            return globals::ctx;

        return iio_create_context_from_uri(hostname);

    } else {
        return iio_create_local_context();
    }
}

#pragma endregion }
