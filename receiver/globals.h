// ============================================================================
// File: globals.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for globals.h.
// ============================================================================

#ifndef GLOBALS_H
#define GLOBALS_H

#define USE_INTERN_SAMPLING_FREQ -1.0

#include <iio.h>

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QWidget>
#include <QString>

struct w_info {
    QWidget * widget;
    char *  name;
    unsigned char * lut;
    unsigned char lut_len;
};

struct plotData{
    int plotType=0;
    QList<QVector<double>> xItemList,yItemList;
    QList<QVector<double>> fftxItemList,fftyItemList;
};

class globals
{

public:
    // Constructor: Initializes globals subsystem components, default parameters, and state.
    globals();
    static struct iio_context *ctx;
    static bool status;
    static struct iio_device *dev;
    static QList<w_info*> *attrs;
    static QList<plotData> *PlotsData;
    static bool hopping;
    static double peakValue;
    static double peakValueDb;
    static double temp7291;
    static double temp9009;

    // Board serial number ("hw_serial" context attribute, with an ssh
    // EEPROM read fallback).  Shared by the first-start form and the
    // Profile tab serial rules: SN001 boards have no ORx 400 (the forms
    // disable the option), SN003 uses the ORx 400 section without the
    // orxMergeFilter (orx_400_03.txt), every other board is normal.
    static QString boardSerialNumber();
    // Executes serial is sn001 routine.
    static bool serialIsSn001(const QString &sn);
    // Executes serial is sn003 routine.
    static bool serialIsSn003(const QString &sn);

    // Executes osc destroy context routine.
    static void osc_destroy_context();
    // Executes connect widgets routine.
    static int connect_widgets(QWidget *builder);
    static int __connect_widget(struct iio_device *dev, const char *attr,
                         const char *value, size_t len, void *d);
    // Sets widget_value value and updates internal subsystem configuration.
    static char * set_widget_value(QWidget *widget, struct w_info *item, long long val);
    // Executes connect widget routine.
    static void connect_widget(QWidget *widget, struct w_info *item, long long val);

};

#endif // GLOBALS_H
