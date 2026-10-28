// ============================================================================
// File: ad9371.h
// Subsystem: eLynxSDR plugins
// Description: Subsystem implementation and interface guidance for ad9371.h.
// ============================================================================

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
    // Constructor: Initializes AD9371 subsystem components, default parameters, and state.
    explicit AD9371(QApplication *mainApp,QWidget *parent = 0);
    // Destructor: Cleans up AD9371 resources, timers, and allocated memory.
    ~AD9371();

    QWidget *init();

    // Retrieves current gui_tx_sampling_freq state.
    double get_gui_tx_sampling_freq();

private slots:
    // Event/Signal handler for reload button clicked.
    void reload_button_clicked();
    // Executes glb settings update labels routine.
    void glb_settings_update_labels(void);
    // Executes save widget value routine.
    void save_widget_value(iio_widget *iio_w);
    // Executes log value routine.
    void LogValue(QString value);
    // Event/Signal handler for on btn reload settings clicked.
    void on_btnReloadSettings_clicked();

private:
    Ui::AD9371 *ui;
    QSignalMapper *signalMapper;
    // Executes profile update labels routine.
    void profile_update_labels();
    // Executes rx freq info update routine.
    void rx_freq_info_update();
    // Executes rssi update labels routine.
    void rssi_update_labels();
    // Executes make widget update signal based routine.
    void make_widget_update_signal_based(iio_widget *widgets, unsigned int num_widgets);

    QTimer refreshTimer;
    QThread *refreshThread;
    int interval=500;
    bool refreshMode=false;

    // Executes profile update routine.
    void profile_update();
    // Executes rx phase rotation update routine.
    void rx_phase_rotation_update();
    // Executes rx phase rotation set routine.
    void rx_phase_rotation_set(QDoubleSpinBox *spinbutton, gpointer user_data);
    // Executes int dec update cb routine.
    void int_dec_update_cb();
    // Executes int dec freq update routine.
    void int_dec_freq_update();
    // Event/Signal handler for tx sample rate changed.
    void tx_sample_rate_changed(QDoubleSpinBox *spinbutton);
signals:
    // Executes change widget routine.
    void change_widget(QWidget*,struct iio_widget *);

public slots:
    // Updates and synchronizes refresh timer started data stream.
    void refreshTimerStarted();

};

#endif // AD9371_H
