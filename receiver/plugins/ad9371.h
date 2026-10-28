/**
 * @file ad9371.h
 * @brief Subsystem module for plugins in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class AD9371
 * @brief High-level controller class for AD9371.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class AD9371;
}

/**
 * @class AD9371
 * @brief High-level controller class for AD9371.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class AD9371 : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new AD9371 instance and initializes subsystem components and UI state.
     * @param mainApp Input parameter specifying mainApp (QApplication).
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit AD9371(QApplication *mainApp,QWidget *parent = 0);
    /**
     * @brief Destructor for AD9371, releasing allocated hardware resources, timers, and buffers.
     */
    ~AD9371();

    QWidget *init();

    /**
     * @brief Retrieves the current gui_tx_sampling_freq value.
     * @return Output value of type double.
     */
    double get_gui_tx_sampling_freq();

private slots:
    /**
     * @brief Slot handler responding to reload button clicked event.
     */
    void reload_button_clicked();
    /**
     * @brief Executes the glb settings update labels procedure for AD9371.
     */
    void glb_settings_update_labels(void);
    /**
     * @brief Executes the save widget value procedure for AD9371.
     * @param iio_w Input parameter specifying iio_w (iio_widget).
     */
    void save_widget_value(iio_widget *iio_w);
    /**
     * @brief Executes the log value procedure for AD9371.
     * @param value Input parameter specifying value (QString).
     */
    void LogValue(QString value);
    /**
     * @brief Slot handler responding to on btn reload settings clicked event.
     */
    void on_btnReloadSettings_clicked();

private:
    Ui::AD9371 *ui;
    QSignalMapper *signalMapper;
    /**
     * @brief Executes the profile update labels procedure for AD9371.
     */
    void profile_update_labels();
    /**
     * @brief Executes the rx freq info update procedure for AD9371.
     */
    void rx_freq_info_update();
    /**
     * @brief Executes the rssi update labels procedure for AD9371.
     */
    void rssi_update_labels();
    /**
     * @brief Executes the make widget update signal based procedure for AD9371.
     * @param widgets Input parameter specifying widgets (iio_widget).
     * @param num_widgets Input parameter specifying num_widgets (unsigned int).
     */
    void make_widget_update_signal_based(iio_widget *widgets, unsigned int num_widgets);

    QTimer refreshTimer;
    QThread *refreshThread;
    int interval=500;
    bool refreshMode=false;

    /**
     * @brief Executes the profile update procedure for AD9371.
     */
    void profile_update();
    /**
     * @brief Executes the rx phase rotation update procedure for AD9371.
     */
    void rx_phase_rotation_update();
    /**
     * @brief Executes the rx phase rotation set procedure for AD9371.
     * @param spinbutton Input parameter specifying spinbutton (QDoubleSpinBox).
     * @param user_data Input parameter specifying user_data (gpointer).
     */
    void rx_phase_rotation_set(QDoubleSpinBox *spinbutton, gpointer user_data);
    /**
     * @brief Executes the int dec update cb procedure for AD9371.
     */
    void int_dec_update_cb();
    /**
     * @brief Executes the int dec freq update procedure for AD9371.
     */
    void int_dec_freq_update();
    /**
     * @brief Slot handler responding to tx sample rate changed event.
     * @param spinbutton Input parameter specifying spinbutton (QDoubleSpinBox).
     */
    void tx_sample_rate_changed(QDoubleSpinBox *spinbutton);
signals:
    /**
     * @brief Executes the change widget procedure for AD9371.
     * @param QWidget* Input parameter specifying QWidget* (QWidget*).
     * @param  Input parameter specifying  (struct iio_widget).
     */
    void change_widget(QWidget*,struct iio_widget *);

public slots:
    /**
     * @brief Updates and synchronizes the refresh timer started data stream and visual indicators.
     */
    void refreshTimerStarted();

};

#endif // AD9371_H
