// ============================================================================
// File: ADRV.h
// Subsystem: eLynxSDR adrv
// Description: Subsystem implementation and interface guidance for ADRV.h.
// ============================================================================

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

class adrv : public QObject
{
    Q_OBJECT

public:

    // Constructor: Initializes adrv subsystem components, default parameters, and state.
    explicit adrv();
    // Executes connect routine.
    bool connect(QString ip);

    // Executes get frequency routine.
    double GetFrequency();
    // Executes set frequency routine.
    bool SetFrequency(double freq);
    // Executes set frequency hopping routine.
    bool SetFrequencyHopping(double start,double stop,double step);

    // Executes get power routine.
    double GetPower();
    // Executes set power routine.
    bool SetPower(double freq);

    QHBoxLayout *InitializeDacBuffer();

    // Executes set file routine.
    QString SetFile(QString fileName,double scale);
    // Executes disable dac routine.
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
 // Executes progress routine.
 void progress(int);
 // Executes frequency changed routine.
 void frequencyChanged();
// void thisIsTest( struct iio_context *x);

};

#endif // ADRV_H
