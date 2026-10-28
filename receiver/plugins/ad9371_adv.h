// ============================================================================
// File: ad9371_adv.h
// Subsystem: eLynxSDR plugins
// Description: Subsystem implementation and interface guidance for ad9371_adv.h.
// ============================================================================

#ifndef AD9371_ADV_H
#define AD9371_ADV_H

#include "receiver/custom/tabwidget.h"
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


namespace Ui {
class ad9371_adv;
}

class ad9371_adv : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes ad9371_adv subsystem components, default parameters, and state.
    explicit ad9371_adv(QWidget *parent = 0);
    // Destructor: Cleans up ad9371_adv resources, timers, and allocated memory.
    ~ad9371_adv();

    QWidget *init();



private slots:
    // Event/Signal handler for on btn save settings clicked.
    void on_btn_save_settings_clicked();

private:
    Ui::ad9371_adv *ui;
    // Executes initialize widgets routine.
    void InitializeWidgets();
    // Executes add widget to list routine.
    void AddWidgetToList(QWidget *widget, char *name,  unsigned char * const lut,  unsigned char lut_len);
};

#endif // AD9371_ADV_H
