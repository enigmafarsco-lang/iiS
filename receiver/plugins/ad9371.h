#ifndef AD9371_H
#define AD9371_H

#include <QWidget>
#include "dacDataManager.h"
#include "ui_ad9371.h"
#include "receiver/globals.h"

#include <glib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <QSignalMapper>
#include <string.h>
#include <unistd.h>

#include "../datatypes.h"
#include "../libini2.h"
#include "../osc_plugin.h"
#include "../config.h"
#include "receiver/iio_widget.h"
#include <QTimer>

namespace Ui {
class AD9371;
}

class AD9371 : public QWidget
{
    Q_OBJECT

public:
    explicit AD9371(QApplication *mainApp,QWidget *parent = 0);
    ~AD9371();

    QWidget *init();

    double get_gui_tx_sampling_freq();

private slots:
    void reload_button_clicked();
    void glb_settings_update_labels(void);
    void save_widget_value(iio_widget *iio_w);
    void LogValue(QString value);
    void on_btnReloadSettings_clicked();

private:
    Ui::AD9371 *ui;
    QSignalMapper *signalMapper;
    void profile_update_labels();
    void rx_freq_info_update();
    void rssi_update_labels();
    void make_widget_update_signal_based(iio_widget *widgets, unsigned int num_widgets);

    QTimer refreshTimer;
    QThread *refreshThread;
    int interval=500;
    bool refreshMode=false;

    void profile_update();
    void rx_phase_rotation_update();
    void rx_phase_rotation_set(QDoubleSpinBox *spinbutton, gpointer user_data);
    void int_dec_update_cb();
    void int_dec_freq_update();
    void tx_sample_rate_changed(QDoubleSpinBox *spinbutton);
signals:
    void change_widget(QWidget*,struct iio_widget *);

public slots:
    void refreshTimerStarted();

};

#endif // AD9371_H
