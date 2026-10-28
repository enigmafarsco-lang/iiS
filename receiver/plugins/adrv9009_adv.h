// ============================================================================
// File: adrv9009_adv.h
// Subsystem: eLynxSDR plugins
// Description: Subsystem implementation and interface guidance for adrv9009_adv.h.
// ============================================================================

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
class adrv9009_adv;
}

class adrv9009_adv : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes adrv9009_adv subsystem components, default parameters, and state.
    explicit adrv9009_adv(QWidget *parent = 0);
    // Destructor: Cleans up adrv9009_adv resources, timers, and allocated memory.
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
    // Event/Signal handler for on btn save settings clicked.
    void on_btn_save_settings_clicked();
    // Executes bist tone cb routine.
    void bist_tone_cb();

    // Event/Signal handler for on fhm chk trigger mode state changed.
    void on_fhm_chk_trigger_mode_stateChanged(int arg1);

signals:
    // Executes save setting signal routine.
    void saveSettingSignal();
    // Transmits send transmit command/packet over communication interface.
    void sendTransmit(int);

private:
    Ui::adrv9009_adv *ui;
    // Executes initialize widgets routine.
    void InitializeWidgets();
    //saeid raziani
    QProgressBar * newProg = new QProgressBar();
    // Executes add widget to list routine.
    void AddWidgetToList(QWidget *widget, char *name,  unsigned char * const lut,  unsigned char lut_len);
};

#endif // adrv9009_ADV_H
