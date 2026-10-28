/**
 * @file ADRV.h
 * @brief High-Level Transceiver Abstraction Helper for ADRV9009 IIO Devices.
 *
 * Provides helper wrappers for RF LO tuning, hardware gain adjustment,
 * frequency hopping initialization, and DAC DMA buffer setup.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef ADRV_H
#define ADRV_H

#include <QObject>
#include <iio.h>
#include <glib.h>
#include <glib-object.h>
#include <QDebug>
#include <QFuture>
#include <QThread>
#include <QHBoxLayout>

#include "receiver/plugins/dacDataManager.h"
#include "receiver/globals.h"

#define PHY_DEVICE "adrv9009-phy"
#define DDS_DEVICE "axi-adrv9009-tx-hpc"

/**
 * @class adrv
 * @brief Hardware wrapper class for ADRV9009 RF transceiver operations.
 */
class adrv : public QObject
{
    Q_OBJECT

public:
    explicit adrv();
    ~adrv();

    /**
     * @brief Connects to target SDR hardware IP or creates local context.
     * @param ip Hardware IP address string.
     * @return True if connection established.
     */
    bool connect(QString ip);

    /**
     * @brief Reads current RF carrier frequency from hardware.
     * @return Frequency in MHz.
     */
    double GetFrequency();

    /**
     * @brief Programs RF carrier frequency.
     * @param freq Frequency in MHz.
     * @return True on success.
     */
    bool SetFrequency(double freq);

    /**
     * @brief Configures fast frequency hopping parameters.
     * @param start Start frequency in MHz.
     * @param stop Stop frequency in MHz.
     * @param step Step size in MHz.
     * @return True on success.
     */
    bool SetFrequencyHopping(double start, double stop, double step);

    /**
     * @brief Reads current TX power attenuation.
     * @return Power attenuation value.
     */
    double GetPower();

    /**
     * @brief Sets TX hardware power attenuation.
     * @param power Power attenuation value.
     * @return True on success.
     */
    bool SetPower(double power);

    /**
     * @brief Initializes DAC DMA buffer manager for cyclic sample transmission.
     * @return Optional GUI container layout.
     */
    QHBoxLayout *InitializeDacBuffer();

    /**
     * @brief Uploads IQ sample file into DAC DMA buffer.
     * @param fileName Path to sample file.
     * @param scale Digital scaling factor.
     * @return Status description message.
     */
    QString SetFile(QString fileName, double scale);

    /**
     * @brief Stops DAC DMA transmission.
     */
    void DisableDac();

    bool hopping=false;

private:
    const gdouble mhz_scale = 1000000.0;
    const gdouble inv_scale = -1.0;
    const gdouble scale100 = 100.0;

    struct iio_context *ctx=nullptr;    
    dac_data_manager *m_dac_data_manager{nullptr};

    bool freqChanged=false;
    double baseFreq=0;
    double oldFreq=-1;

    double RFBandwidth;
    double RFBandwidthOBS;
    double power=0;

    QFuture<void> future;
    iio_context *GetContext(QString ip);

    struct iio_device *dds{nullptr};

signals:
    void progress(int);
    void frequencyChanged();
};

#endif // ADRV_H
