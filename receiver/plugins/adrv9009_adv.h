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
    explicit adrv9009_adv(QWidget *parent = 0);
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
    void on_btn_save_settings_clicked();
    void bist_tone_cb();

    void on_fhm_chk_trigger_mode_stateChanged(int arg1);

signals:
    void saveSettingSignal();
    void sendTransmit(int);

private:
    Ui::adrv9009_adv *ui;
    void InitializeWidgets();
    //saeid raziani
    QProgressBar * newProg = new QProgressBar();
    void AddWidgetToList(QWidget *widget, char *name,  unsigned char * const lut,  unsigned char lut_len);
};

#endif // adrv9009_ADV_H
