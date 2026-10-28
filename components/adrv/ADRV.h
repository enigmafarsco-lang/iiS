/**
 * @file ADRV.h
 * @brief Subsystem module for adrv in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef ADRV_H
#define ADRV_H

#include <QObject>
#include <iio.h>
#include <glib.h>
#include <glib-object.h>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include "qtconcurrentrun.h"
#include <QThread>
#include "receiver/plugins/dacDataManager.h"
#include <receiver/globals.h>

#define PHY_DEVICE "adrv9009-phy"
#define DDS_DEVICE "axi-adrv9009-tx-hpc"

/**
 * @class adrv
 * @brief High-level controller class for adrv.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class adrv : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a new adrv instance and initializes subsystem components and UI state.
     */
    explicit adrv();
    /**
     * @brief Executes the connect procedure for adrv.
     * @param ip Input parameter specifying ip (QString).
     * @return Output value of type bool.
     */
    bool connect(QString ip);

    /**
     * @brief Executes the get frequency procedure for adrv.
     * @return Output value of type double.
     */
    double GetFrequency();
    /**
     * @brief Executes the set frequency procedure for adrv.
     * @param freq Input parameter specifying freq (double).
     * @return Output value of type bool.
     */
    bool SetFrequency(double freq);
    /**
     * @brief Executes the set frequency hopping procedure for adrv.
     * @param start Input parameter specifying start (double).
     * @param stop Input parameter specifying stop (double).
     * @param step Input parameter specifying step (double).
     * @return Output value of type bool.
     */
    bool SetFrequencyHopping(double start,double stop,double step);

    /**
     * @brief Executes the get power procedure for adrv.
     * @return Output value of type double.
     */
    double GetPower();
    /**
     * @brief Executes the set power procedure for adrv.
     * @param freq Input parameter specifying freq (double).
     * @return Output value of type bool.
     */
    bool SetPower(double freq);

    QHBoxLayout *InitializeDacBuffer();

    /**
     * @brief Executes the set file procedure for adrv.
     * @param fileName Input parameter specifying fileName (QString).
     * @param scale Input parameter specifying scale (double).
     * @return Output value of type QString.
     */
    QString SetFile(QString fileName,double scale);
    /**
     * @brief Executes the disable dac procedure for adrv.
     */
    void DisableDac();

    bool hopping=false;
private:
    const gdouble mhz_scale = 1000000.0;
    const gdouble inv_scale = -1.0;
    const gdouble scale100 = 100.0;

    struct iio_context *ctx=nullptr;    

    dacDataManager *dac_data_manager;

    bool freqChanged=false;
    double baseFreq=0;
    double oldFreq=-1;

    double RFBandwidth;
    double RFBandwidthOBS;

    double power=0;

    QFuture<void> future;

    iio_context *GetContext(QString ip);

    struct iio_device *dds;
    struct dac_data_manager *dac_tx_manager;

signals:
 /**
  * @brief Executes the progress procedure for adrv.
  * @param int Input parameter specifying int (int).
  */
 void progress(int);
 /**
  * @brief Executes the frequency changed procedure for adrv.
  */
 void frequencyChanged();
// void thisIsTest( struct iio_context *x);

};

#endif // ADRV_H
