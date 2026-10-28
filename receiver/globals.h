/**
 * @file globals.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

/**
 * @class globals
 * @brief High-level controller class for globals.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class globals
{

public:
    /**
     * @brief Constructs a new globals instance and initializes subsystem components and UI state.
     */
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
    static bool serialIsSn001(const QString &sn);
    static bool serialIsSn003(const QString &sn);

    static void osc_destroy_context();
    /**
     * @brief Executes the connect widgets procedure for globals.
     * @param builder Input parameter specifying builder (QWidget).
     * @return Output value of type static int.
     */
    static int connect_widgets(QWidget *builder);
    static int __connect_widget(struct iio_device *dev, const char *attr,
                         const char *value, size_t len, void *d);
    /**
     * @brief Sets the widget_value parameter and updates internal state.
     * @param widget Input parameter specifying widget (QWidget).
     * @param item Input parameter specifying item (struct w_info).
     * @param val Input parameter specifying val (long long).
     * @return Output value of type static char *.
     */
    static char * set_widget_value(QWidget *widget, struct w_info *item, long long val);
    /**
     * @brief Executes the connect widget procedure for globals.
     * @param widget Input parameter specifying widget (QWidget).
     * @param item Input parameter specifying item (struct w_info).
     * @param val Input parameter specifying val (long long).
     * @return Output value of type static void.
     */
    static void connect_widget(QWidget *widget, struct w_info *item, long long val);

};

#endif // GLOBALS_H
