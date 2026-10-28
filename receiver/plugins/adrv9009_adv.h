/**
 * @file adrv9009_adv.h
 * @brief Subsystem module for plugins in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef adrv9009_ADV_H
#define adrv9009_ADV_H

#include <receiver/custom/tabwidget.h>
#include <stdio.h>

#include <QWidget>
#include <glib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iio.h>
#include "../libini2.h"
#include "../osc.h"
#include "../osc_plugin.h"
#include "../config.h"
#include "../iio_widget.h"
#include "../datatypes.h"
#include "receiver/globals.h"
#include <QProgressBar>


namespace Ui {
/**
 * @class adrv9009_adv
 * @brief High-level controller class for adrv9009_adv.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class adrv9009_adv;
}

/**
 * @class adrv9009_adv
 * @brief High-level controller class for adrv9009_adv.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class adrv9009_adv : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new adrv9009_adv instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit adrv9009_adv(QWidget *parent = 0);
    /**
     * @brief Destructor for adrv9009_adv, releasing allocated hardware resources, timers, and buffers.
     */
    ~adrv9009_adv();

    QWidget *init();


    //saeid raziani
    QCheckBox      * chk_trigger_mode;
    QCheckBox      * chk_exit_mode;
    QCheckBox      * chk_msc_sync;
    QCheckBox      * chk_enable;

    QDoubleSpinBox * txt_init_frequency;
    QSpinBox       * txt_gpio_pin;
    QSpinBox       * txt_min_freq;
    QSpinBox       * txt_max_freq;

    QPushButton * save_settings;




private slots:
    /**
     * @brief Slot handler responding to on btn save settings clicked event.
     */
    void on_btn_save_settings_clicked();
    /**
     * @brief Executes the bist tone cb procedure for adrv9009_adv.
     */
    void bist_tone_cb();

    /**
     * @brief Slot handler responding to on fhm chk trigger mode state changed event.
     * @param arg1 Input parameter specifying arg1 (int).
     */
    void on_fhm_chk_trigger_mode_stateChanged(int arg1);

signals:
    /**
     * @brief Executes the save setting signal procedure for adrv9009_adv.
     */
    void saveSettingSignal();
    /**
     * @brief Transmits send transmit command/packet over the communication bus.
     * @param int Input parameter specifying int (int).
     */
    void sendTransmit(int);

private:
    Ui::adrv9009_adv *ui;
    /**
     * @brief Executes the initialize widgets procedure for adrv9009_adv.
     */
    void InitializeWidgets();
    //saeid raziani
    QProgressBar * newProg = new QProgressBar();
    void AddWidgetToList(QWidget *widget, char *name,  unsigned char * const lut,  unsigned char lut_len);
};

#endif // adrv9009_ADV_H
