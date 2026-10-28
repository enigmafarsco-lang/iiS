/**
 * @file adrv9009.h
 * @brief Subsystem module for plugins in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef ADRv9009_H
#define ADRv9009_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QSettings>
#include "dacDataManager.h"
#include <receiver/datafiles.h>
#include "receiver/globals.h"
#include <receiver/osc.h>
#include <QFileDialog>
#include <glib.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <QSignalMapper>
#include <receiver/settings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "../datatypes.h"
#include "../libini2.h"
#include "../osc_plugin.h"
#include "../config.h"
#include "receiver/iio_widget.h"
#include <QTimer>
#include <receiver/iio_utils.h>

namespace Ui {
/**
 * @class adrv9009
 * @brief High-level controller class for adrv9009.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class adrv9009;
}

enum plugin_section
{
    SECTION_GLOBAL,
    SECTION_TX,
    SECTION_RX,
    SECTION_OBS,
    SECTION_FPGA,
    SECTION_NUM,
};


/* This structure contains all information related to one adrv9009-phy device.
 * This plugin will dynamically create new sets of widgets for each additional
 * device that will find.
 */
struct plugin_subcomponent
{
    /* References to IIO structures */
    struct iio_device *iio_dev;
    struct iio_channel *ch0, *ch1, *ch2, *ch3, *alt_ch0, *alt_ch1, *out_ch0, *out_ch1;

    /* List of containers of widgets grouped for each section */
    QWidget *section_containers[SECTION_NUM];

    /* Widgets for sensor*/
    QWidget *label_temp;

    /* Widgets for Receive Settings */
    QWidget *rx_gain_control_rx1;
    QWidget *rx_gain_control_modes_rx1;
    QWidget *rx_gain_control_rx2;
    QWidget *rx1_rssi;
    QWidget *rx2_rssi;
    QWidget *label_rf_bandwidth_rx;
    QWidget *label_sampling_freq_rx;

    /* Widgets for Transmit Settings */
    QWidget *label_rf_bandwidth_tx;
    QWidget *label_sampling_freq_tx;

    /* Widgets for Observation Receive Settings */
    QWidget *obs_port_select;
    //QWidget *obs_rssi;
    QWidget *label_rf_bandwidth_obs;
    QWidget *label_sampling_freq_obs;

    /* Widgets for FPGA Settings */
    QWidget *rx_phase_rotation[2];

    /* IIO Widgets */
    struct iio_widget widgets[200];
    struct iio_widget *glb_widgets, *tx_widgets, *rx_widgets, *obsrx_widgets;
    unsigned int num_glb, num_tx, num_rx, num_obsrx;

    /* Useful indexes of IIO widgets from the list of iio widgets of this subcomponent */
    unsigned int rx1_gain, rx2_gain, obs_gain;
    unsigned int trx_lo, aux_lo;
    unsigned int rx_sample_freq, tx_sample_freq;

    /* Save/Restore attributes */
    char **sr_attribs;
    size_t sr_attribs_count;
};

/**
 * @class adrv9009
 * @brief High-level controller class for adrv9009.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class adrv9009 : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new adrv9009 instance and initializes subsystem components and UI state.
     * @param mainApp Input parameter specifying mainApp (QApplication).
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit adrv9009(QApplication *mainApp,QWidget *parent = 0);
    /**
     * @brief Destructor for adrv9009, releasing allocated hardware resources, timers, and buffers.
     */
    ~adrv9009();

    QWidget *init();
    // saeid raziani ==> here we define some ui pointer in order to using and showing them in receiver main.
    //--- Global ------------------------------------------------------------------
    QComboBox* ensmCmb;
    QDoubleSpinBox *frqSpn;
    QComboBox * ensm_mode_available;
    //--- Hopping ------------------------------------------------------------------
    QDoubleSpinBox * nextFrqSpn;
    QDoubleSpinBox * minSpn;
    QDoubleSpinBox * maxSpn;
    QDoubleSpinBox * stepSpn;
    QDoubleSpinBox * delaySpn;
    QPushButton    * startBtn;
    QCheckBox      * modeChk;
    //--- TX1 ------------------------------------------------------------------
    QCheckBox      * pinn_TX1_Chk;
    QCheckBox      * track_TX1_Chk;
    QCheckBox      * power_TX1_DownChk;
    QCheckBox      * lo_TX1_Chk;
    QDoubleSpinBox * att_TX1_Spn;
    QLabel         * rfBandlbl;
    QLabel         * sampleRatelbl;
    //--- TX1 ------------------------------------------------------------------
    QCheckBox      * pinn_TX2_Chk;
    QCheckBox      * track_TX2_Chk;
    QCheckBox      * powerTX2DownChk;
    QCheckBox      * lo_TX2_Chk;
    QDoubleSpinBox * att_TX2_Spn;
    //--- OBSRX ------------------------------------------------------------------
    QCheckBox      * power_OBSRX_Spn;
    QCheckBox      * track_OBSRX_Chk;
    QDoubleSpinBox * hardwareGain;
    QLabel         * rfBandOBSlbl;
    QLabel         * sampleRateOBSlbl;

    QCheckBox * rx1Powerdown;
    QCheckBox * rx2Powerdown;
    QCheckBox * obs2Powerdown;
//    QCheckBox * obs1_powerdown;

    QDoubleSpinBox * tx_lo_freq;
    //--- ADRV9009 calibration widgets (shown on ReceiverMain Calibration tab)
    QCheckBox  * cal_rx_qec_chk;
    QCheckBox  * cal_tx_qec_chk;
    QCheckBox  * cal_tx_lol_chk;
    QCheckBox  * cal_tx_lol_ext_chk;
    QCheckBox  * cal_rx_phase_chk;
    QCheckBox  * cal_fhm_chk;
    QPushButton * calibrateBtn;

    //saeid raziani
     dacDataManager *dac_data_manager;

    //----- Load file
    QPushButton * profile_config;

    void tx_sample_rate_changed(const int);
//    void tx_sample_rate_changed(QDoubleSpinBox *spinbutton);

    //
    double get_gui_tx_sampling_freq();

    struct iio_widget iio_ensm_mode_available;

    QWidget *fpga_rx_frequency_available;
    QWidget *fpga_tx_frequency_available;
    struct iio_widget fpga_widgets[2];
    /**
     * @brief Sets the file parameter and updates internal state.
     * @param fileName Input parameter specifying fileName (QString).
     * @param scale Input parameter specifying scale (double).
     * @param pair01 Input parameter specifying pair01 (bool).
     * @param pair23 Input parameter specifying pair23 (bool).
     * @return Output value of type QString.
     */
    QString setFile(QString fileName,double scale, bool pair01 = true, bool pair23 = true);
    /**
     * @brief Executes the changing dac procedure for adrv9009.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type QString.
     */
    QString changingDac(QString);
    // TX1 LO-leakage + quadrature tracking: forced enabled + checked
    // (and locked) - must hold in ALL settings, re-applied after every
    // profile load / widget sync.
    // Re-write TX1/TX2 hardwaregain so the board carries Pb = Pa + Pc
    // with the current P calibration (constants/tx_calibration.h).
    void resaveTxGainWidgets();
    bool saveSettingsToIni(const QString &fileName);
    bool loadSettingsFromIni(const QString &fileName);
    bool loadIioOscSettings(const QString &fileName);
    void firmwareResetOverLan();
    // Phase 6: CW-tab DDS tone parameters, set from the exciter before
    // changingDac("set-cw") switches TX1/TX2 into the DDS tone mode.
    static void setCwDdsParams(double freqMhz, double scaleDbfs, double phaseDeg);
    QString changeDacToBuffer(); // CW tone -> DAC Buffer Output (DRFM tab)
    bool isDacBufferRunning() const;


//    struct iio_device *dds;
//    struct dac_data_manager *dac_tx_manager;

private slots:
    void reload_button_clicked();
    void glb_settings_update_labels(void);
    void save_widget_value(iio_widget *iio_w);
    /**
     * @brief Executes the log value procedure for adrv9009.
     * @param value Input parameter specifying value (QString).
     */
    void LogValue(QString value);
    /**
     * @brief Slot handler responding to on btn reload settings clicked event.
     */
    void on_btnReloadSettings_clicked();

//    void on_profile_config_clicked();



public slots:
    void startHopping(bool, double min=0, double max=0, double step=0, int delay=0);
    /**
     * @brief Executes the test procedure for adrv9009.
     * @param QString Input parameter specifying QString (QString).
     */
    void test(QString);
/**
 * @brief Slot handler responding to on profile config clicked event.
 * @param QString Input parameter specifying QString (QString).
 */
void on_profile_config_clicked(QString);
private:
    /**
     * @brief Executes the widget value procedure for adrv9009.
     * @param w Input parameter specifying w (QWidget).
     * @return Output value of type double.
     */
    double widgetValue(QWidget *w) const;
    /**
     * @brief Executes the apply widget value procedure for adrv9009.
     * @param w Input parameter specifying w (QWidget).
     * @param v Input parameter specifying v (double).
     */
    void applyWidgetValue(QWidget *w, double v);
    /**
     * @brief Sets the tings widget list parameter and updates internal state.
     * @return Output value of type QVector<QPair<QString, QWidget *> >.
     */
    QVector<QPair<QString, QWidget *> > settingsWidgetList();
    /**
     * @brief Executes the print hello procedure for adrv9009.
     */
    void printHello();
    Ui::adrv9009 *ui;
    QSignalMapper *signalMapper;
    QFuture<void> refreshFuture;
    QFuture<void> hoppingFuture;

    //saeid raziani
    const QString fileAddress{resourceFilePath(QStringLiteral("filters/adrv9009/Tx_BW400_IR491p52_Rx_BW100_OR122p88_ORx_BW400_OR491p52_DC245p76.txt"))};


    bool hopping=false;

    Settings setting;
    QProgressDialog *progress=nullptr;

    /**
     * @brief Executes the rx freq info update procedure for adrv9009.
     */
    void rx_freq_info_update();
    /**
     * @brief Executes the rssi update labels procedure for adrv9009.
     */
    void rssi_update_labels();
    /**
     * @brief Executes the make widget update signal based procedure for adrv9009.
     * @param widgets Input parameter specifying widgets (iio_widget).
     * @param num_widgets Input parameter specifying num_widgets (unsigned int).
     */
    void make_widget_update_signal_based(iio_widget *widgets, unsigned int num_widgets);

    QThread *refreshThread = nullptr;
    QTimer *refreshTimer = nullptr;
    int interval=1000;
    bool refreshMode=false;

    QList<plugin_subcomponent> subcomponents;

    bool plugin_single_device_mode = TRUE;
    guint phy_devs_count = 0;

    /**
     * @brief Executes the profile update procedure for adrv9009.
     */
    void profile_update();
    /**
     * @brief Executes the rx phase rotation update procedure for adrv9009.
     */
    void rx_phase_rotation_update();
    /**
     * @brief Executes the rx phase rotation set procedure for adrv9009.
     * @param spinbutton Input parameter specifying spinbutton (QDoubleSpinBox).
     * @param user_data Input parameter specifying user_data (gpointer).
     */
    void rx_phase_rotation_set(QDoubleSpinBox *spinbutton, gpointer user_data);
    /**
     * @brief Executes the int dec update cb procedure for adrv9009.
     */
    void int_dec_update_cb();
    /**
     * @brief Executes the int dec freq update procedure for adrv9009.
     */
    void int_dec_freq_update();

    /**
     * @brief Updates and synchronizes the update_widgets data stream and visual indicators.
     */
    void update_widgets();
    /**
     * @brief Updates and synchronizes the update_label_from data stream and visual indicators.
     * @param label Input parameter specifying label (QLabel).
     * @param dev Input parameter specifying dev (iio_device).
     * @param channel Input parameter specifying channel (const char).
     * @param attribute Input parameter specifying attribute (const char).
     * @param output Input parameter specifying output (bool).
     * @param unit Input parameter specifying unit (const char).
     * @param scale Input parameter specifying scale (int).
     */
    void update_label_from(QLabel *label, iio_device *dev, const char *channel, const char *attribute, bool output, const char *unit, int scale);
    GArray *get_iio_devices_starting_with(iio_context *ctx, const char *sequence);
    /**
     * @brief Executes the connect signals procedure for adrv9009.
     */
    void ConnectSignals();

    QString last_profile="";
    /**
     * @brief Executes the load tal profile procedure for adrv9009.
     * @param file_name Input parameter specifying file_name (QString).
     * @return Output value of type int.
     */
    int load_tal_profile(QString file_name);
    /**
     * @brief Updates and synchronizes the update_label_with_scale_from data stream and visual indicators.
     * @param label Input parameter specifying label (QLabel).
     * @param dev Input parameter specifying dev (iio_device).
     * @param channel Input parameter specifying channel (const char).
     * @param attribute Input parameter specifying attribute (const char).
     * @param attribute_scale Input parameter specifying attribute_scale (const char).
     * @param output Input parameter specifying output (bool).
     * @param unit Input parameter specifying unit (const char).
     * @param scale Input parameter specifying scale (int).
     */
    void update_label_with_scale_from(QLabel *label, iio_device *dev, const char *channel, const char *attribute,const char *attribute_scale, bool output, const char *unit, int scale);

signals:
    /**
     * @brief Executes the change widget procedure for adrv9009.
     * @param QWidget* Input parameter specifying QWidget* (QWidget*).
     * @param  Input parameter specifying  (struct iio_widget).
     */
    void change_widget(QWidget*,struct iio_widget *);
    /**
     * @brief Executes the frq has focused signal procedure for adrv9009.
     * @param QString Input parameter specifying QString (QString).
     */
    void frqHasFocusedSignal(QString);
    /**
     * @brief Executes the file load is complete signal procedure for adrv9009.
     */
    void fileLoadIsCompleteSignal();
    /**
     * @brief Executes the frq value changed signal procedure for adrv9009.
     * @param QString Input parameter specifying QString (QString).
     */
    void frqValueChangedSignal(QString);

};

#endif // ADRV9009_H
