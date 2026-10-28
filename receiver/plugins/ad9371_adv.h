/**
 * @file ad9371_adv.h
 * @brief Subsystem module for plugins in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class ad9371_adv
 * @brief High-level controller class for ad9371_adv.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ad9371_adv;
}

/**
 * @class ad9371_adv
 * @brief High-level controller class for ad9371_adv.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ad9371_adv : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new ad9371_adv instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit ad9371_adv(QWidget *parent = 0);
    /**
     * @brief Destructor for ad9371_adv, releasing allocated hardware resources, timers, and buffers.
     */
    ~ad9371_adv();

    QWidget *init();



private slots:
    /**
     * @brief Slot handler responding to on btn save settings clicked event.
     */
    void on_btn_save_settings_clicked();

private:
    Ui::ad9371_adv *ui;
    /**
     * @brief Executes the initialize widgets procedure for ad9371_adv.
     */
    void InitializeWidgets();
    /**
     * @brief Executes the add widget to list procedure for ad9371_adv.
     * @param widget Input parameter specifying widget (QWidget).
     * @param name Input parameter specifying name (char).
     * @param lut Input parameter specifying lut (unsigned char * const).
     * @param lut_len Input parameter specifying lut_len (unsigned char).
     */
    void AddWidgetToList(QWidget *widget, char *name,  unsigned char * const lut,  unsigned char lut_len);
};

#endif // AD9371_ADV_H
