#ifndef ADRv9009_H
#define ADRv9009_H

#include <QWidget>
#include "dacDataManager.h"
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

class adrv9009 : public QWidget
{
    Q_OBJECT

public:
    explicit adrv9009(QApplication *mainApp,QWidget *parent = 0);
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
    QString setFile(QString fileName,double scale );
    QString changingDac(QString);


//    struct iio_device *dds;
//    struct dac_data_manager *dac_tx_manager;

private slots:
    void reload_button_clicked();
    void glb_settings_update_labels(void);
    void save_widget_value(iio_widget *iio_w);
    void LogValue(QString value);
    void on_btnReloadSettings_clicked();

//    void on_profile_config_clicked();



public slots:
    void startHopping(bool, double min=0, double max=0, double step=0, int delay=0);
    void test(QString);
void on_profile_config_clicked(QString);
private:
    void printHello();
    Ui::adrv9009 *ui;
    QSignalMapper *signalMapper;
    QFuture<void> refreshFuture;
    QFuture<void> hoppingFuture;

    //saeid raziani
    const QString fileAddress{"/home/seraj3/Downloads/iio-oscilloscope/filters/adrv9009/Tx_BW400_IR491p52_Rx_BW100_OR122p88_ORx_BW400_OR491p52_DC245p76.txt"};


    bool hopping=false;

    Settings setting;
    QProgressDialog *progress=nullptr;

    void rx_freq_info_update();
    void rssi_update_labels();
    void make_widget_update_signal_based(iio_widget *widgets, unsigned int num_widgets);

    QThread *refreshThread;
    int interval=1000;
    bool refreshMode=false;

    QList<plugin_subcomponent> subcomponents;

    bool plugin_single_device_mode = TRUE;
    guint phy_devs_count = 0;

    void profile_update();
    void rx_phase_rotation_update();
    void rx_phase_rotation_set(QDoubleSpinBox *spinbutton, gpointer user_data);
    void int_dec_update_cb();
    void int_dec_freq_update();

    void update_widgets();
    void update_label_from(QLabel *label, iio_device *dev, const char *channel, const char *attribute, bool output, const char *unit, int scale);
    GArray *get_iio_devices_starting_with(iio_context *ctx, const char *sequence);
    void ConnectSignals();

    QString last_profile="";
    int load_tal_profile(QString file_name);
    void update_label_with_scale_from(QLabel *label, iio_device *dev, const char *channel, const char *attribute,const char *attribute_scale, bool output, const char *unit, int scale);

signals:
    void change_widget(QWidget*,struct iio_widget *);
    void frqHasFocusedSignal(QString);
    void fileLoadIsCompleteSignal();
    void frqValueChangedSignal(QString);

};

#endif // ADRV9009_H
