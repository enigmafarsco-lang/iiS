#ifndef GLOBALS_H
#define GLOBALS_H

#define USE_INTERN_SAMPLING_FREQ -1.0

#include <iio.h>

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QWidget>

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

    static void osc_destroy_context();
    static int connect_widgets(QWidget *builder);
    static int __connect_widget(struct iio_device *dev, const char *attr,
                         const char *value, size_t len, void *d);
    static char * set_widget_value(QWidget *widget, struct w_info *item, long long val);
    static void connect_widget(QWidget *widget, struct w_info *item, long long val);

};

#endif // GLOBALS_H
