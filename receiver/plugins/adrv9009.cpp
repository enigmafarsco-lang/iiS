#include "adrv9009.h"
#include "qtconcurrentrun.h"
#include <ui_adrv9009.h>
#include <QFileInfo>
#include <QApplication>

#pragma region Properties {

#define HANNING_ENBW 1.50

#define THIS_DRIVER "adrv9009"
#define DDM_DEVICE "ad7291"
#define PHY_DEVICE "adrv9009-phy"
#define DDS_DEVICE "axi-adrv9009-tx-hpc"
#define CAP_DEVICE "axi-adrv9009-rx-hpc"
#define CAP_DEVICE_2 "axi-adrv9009-rx-obs-hpc"

#define ARRAY_SIZE(x) (!sizeof(x) ?: sizeof(x) / sizeof((x)[0]))

#define MHZ_TO_HZ(x) ((x) * 1000000ul)

#define REFCLK_RATE 40000000

extern bool dma_valid_selection(const char *device, unsigned mask, unsigned channel_count);

static struct dac_data_manager *dac_tx_manager;

static bool is_2rx_2tx;
static bool has_udc_driver;
static bool can_update_widgets;
static bool has_dpd;

static QWidget *rx_phase_rotation[2];

static const gdouble mhz_scale = 1000000.0;
static const gdouble inv_scale = -1.0;
static const gdouble scale100 = 100.0;

static const char *freq_name;

static struct iio_widget widgets[200];

static unsigned int rx1_gain, rx2_gain, obs_gain, tx1_clgc_desired_gain, tx2_clgc_desired_gain;
static unsigned int num_glb, num_tx, num_rx, num_obsrx, num_fpga=0;
static unsigned int rx_lo, tx_lo, sn_lo;
static unsigned int rx_sample_freq, tx_sample_freq;
static unsigned int tx1_dpd, tx2_dpd, tx1_clgc, tx2_clgc, tx1_vswr, tx2_vswr;;
static double updn_freq_span;
static double updn_freq_mix_sign;
static char last_profile[PATH_MAX];

static struct iio_device *dds,*ddm, *cap, *udc_rx, *udc_tx, *cap_obs;

static QAction *section_toggle[SECTION_NUM];
static QWidget *section_setting[SECTION_NUM];

static gint this_page;
static gboolean plugin_detached;

static const char *adrv9009_sr_attribs[] = {

    ".calibrate_fhm_en",
    ".calibrate_rx_phase_correction_en",
    ".calibrate_rx_qec_en",
    ".calibrate_tx_lol_en",
    ".calibrate_tx_lol_ext_en",
    ".calibrate_tx_qec_en",
    ".ensm_mode",
    ".in_voltage0_gain_control_mode",
    ".in_voltage0_gain_control_pin_mode_en",
    ".in_voltage0_hardwaregain",
    ".in_voltage0_hd2_tracking_en",
    ".in_voltage0_powerdown",
    ".in_voltage0_quadrature_tracking_en",
    ".in_voltage1_gain_control_pin_mode_en",
    ".in_voltage1_hardwaregain",
    ".in_voltage1_hd2_tracking_en",
    ".in_voltage1_powerdown",
    ".in_voltage1_quadrature_tracking_en",
    ".in_voltage2_hardwaregain",
    ".in_voltage2_powerdown",
    ".in_voltage2_quadrature_tracking_en",
    ".in_voltage2_rf_port_select",
    ".in_voltage2_rf_port_select_available",
    ".in_voltage3_hardwaregain",
    ".in_voltage3_powerdown",
    ".in_voltage3_quadrature_tracking_en",
    ".in_voltage3_rf_port_select",
    ".out_altvoltage0_TRX_LO_frequency",
    ".out_altvoltage0_TRX_LO_frequency_hopping_mode_enable",
    ".out_altvoltage0_TRX_LO_frequency_hopping_mode",
    ".out_altvoltage1_AUX_OBS_RX_LO_frequency",
    ".out_voltage0_atten_control_pin_mode_en",
    ".out_voltage0_hardwaregain",
    ".out_voltage0_lo_leakage_tracking_en",
    ".out_voltage0_pa_protection_en",
    ".out_voltage0_powerdown",
    ".out_voltage0_quadrature_tracking_en",
    ".out_voltage1_atten_control_pin_mode_en",
    ".out_voltage1_hardwaregain",
    ".out_voltage1_lo_leakage_tracking_en",
    ".out_voltage1_pa_protection_en",
    ".out_voltage1_powerdown",
    ".out_voltage1_quadrature_tracking_en",
    ".out_voltage1_rf_bandwidth",

};

static const char * adrv9009_driver_attribs[] = {
    "load_tal_profile_file",
    "ensm_mode",
    "global_settings_show",
    "tx_show",
    "rx_show",
    "fpga_show",
    "fpga_rx_frequency_available",
    "fpga_tx_frequency_available",
    "dac_buf_filename",
};

dacDataManager *dac_data_manager_instance;

#pragma endregion }

#pragma region Constructor {

adrv9009::adrv9009(QApplication *app,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adrv9009)
{
    ui->setupUi(this);

    //saeid raziani ==> find all elements that is needed to show in receiver main

    //--- Global ------------------------------------------------------------------
    ensmCmb           = findChild<QComboBox      *>("ensm_mode_available");
    frqSpn            = findChild<QDoubleSpinBox *>("tx_lo_freq");
    ensm_mode_available = ui->ensm_mode_available;
    //--- Hopping ------------------------------------------------------------------
    delaySpn          = findChild<QDoubleSpinBox *>("txt_delay_hopping");
    stepSpn           = findChild<QDoubleSpinBox *>("txt_step_hopping");
    minSpn            = findChild<QDoubleSpinBox *>("txt_min_hopping");
    maxSpn            = findChild<QDoubleSpinBox *>("txt_max_hopping");
    startBtn          = findChild<QPushButton    *>("btnStartHopping");
    nextFrqSpn        = findChild<QDoubleSpinBox *>("tx_lo_freq_hop");
    modeChk           = findChild<QCheckBox      *>("fhm_enable");
    //--- TX1 ------------------------------------------------------------------
    pinn_TX1_Chk      = findChild<QCheckBox      *>("tx1_atten_control_pin_mode_en");
    track_TX1_Chk     = findChild<QCheckBox      *>("tx1_quadrature_tracking_en");
    lo_TX1_Chk        = findChild<QCheckBox      *>("tx1_lo_leakage_tracking_en");
    sampleRatelbl     = findChild<QLabel         *>("label_sampling_freq_tx");
    rfBandlbl         = findChild<QLabel         *>("label_rf_bandwidth_tx");
    att_TX1_Spn       = findChild<QDoubleSpinBox *>("hardware_gain_tx1");
    power_TX1_DownChk = findChild<QCheckBox      *>("tx1_powerdown_en");
    //--- TX2 ------------------------------------------------------------------
    pinn_TX2_Chk      = findChild<QCheckBox      *>("tx2_atten_control_pin_mode_en");
    lo_TX2_Chk        = findChild<QCheckBox      *>("tx2_lo_leakage_tracking_en");
    track_TX2_Chk     = findChild<QCheckBox      *>("tx2_quadrature_tracking_en");
    att_TX2_Spn       = findChild<QDoubleSpinBox *>("hardware_gain_tx2");
    powerTX2DownChk   = findChild<QCheckBox      *>("tx2_powerdown_en");
    //--- OBSRX ------------------------------------------------------------------
    track_OBSRX_Chk   = findChild<QCheckBox      *>("obs1_quadrature_tracking_en");
    sampleRateOBSlbl  = findChild<QLabel         *>("label_sampling_freq_obs");
    rfBandOBSlbl      = findChild<QLabel         *>("label_rf_bandwidth_obs");
    hardwareGain      = findChild<QDoubleSpinBox *>("hardware_gain_obs1");
    power_OBSRX_Spn   = findChild<QCheckBox      *>("obs1_powerdown_en");

    rx1Powerdown   = ui->rx1_powerdown_en;
    rx2Powerdown   = ui->rx2_powerdown_en;
    obs2Powerdown  = ui->obs2_powerdown_en;
//    obs1_powerdown = ui->obs1_powerdown_en;

    tx_lo_freq = ui->tx_lo_freq;
    //saeid raziani
//    ui->tx_lo_freq->setRange(250,6000);
//    ui->obs1_powerdown_en->setStyleSheet("background-color:red");


    //    profile_config = ui->profile_config;

    connect(ui->tx_lo_freq, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){
        emit frqValueChangedSignal(QString::number(val));

    });

    //    connect(ui->tx_lo_freq, &QDoubleSpinBox::, [&](const double val){
    //        emit frqValueChangedSignal(QString::number(val));
    //    });

//    dac_data_manager.dac_da
    connect(ui->btnHello, &QPushButton::clicked,this,[&]{
//        dac_data_manager.setFile("",0,dac_data_manager);
    });
//    dac_data_manager.getData(false);

}

adrv9009::~adrv9009()
{
    if (refreshTimer)
        refreshTimer->stop();
    delete ui;
}

#pragma endregion }

#pragma region Update Widgets and Labels {

/**
 * @brief update_widgets
 */
void adrv9009::update_widgets(void) {

    IIO_Widget iio_w;
//    dacDataManager dacManager;

    guint i = 0;

    for (; i < phy_devs_count; i++) {
        iio_w.iio_update_widgets_of_device(subcomponents[i].widgets, subcomponents[i].num_glb +
                                           subcomponents[i].num_tx + subcomponents[i].num_rx + subcomponents[i].num_obsrx, subcomponents[i].iio_dev);
    }

    for (i = 0; i < num_fpga; i++)
        iio_w.iio_widget_update(&fpga_widgets[i]);

    if (!plugin_single_device_mode) {
        iio_w.iio_widget_update(&iio_ensm_mode_available);
    }

    if (dds)
        iio_w.iio_update_widgets_of_device(subcomponents[0].widgets, subcomponents[0].num_glb + subcomponents[0].num_tx +
                subcomponents[0].num_rx + subcomponents[0].num_obsrx, dds);

//    dacManager.dac_data_manager_update_iio_widgets(dac_tx_manager);
}

/**
 * @brief adrv9009::rx_freq_info_update
 */
void adrv9009::rx_freq_info_update(void) {

    double lo_freq;
    // These helpers do not use instance state, but calling them through an
    // uninitialized pointer is undefined and can crash on some builds.
    osc oscInstance;

    if (cap) {
        oscInstance.rx_update_device_sampling_freq(CAP_DEVICE,
                                                    USE_INTERN_SAMPLING_FREQ);
        lo_freq = mhz_scale*((QDoubleSpinBox*)(subcomponents[0].glb_widgets[subcomponents[0].trx_lo].widget))->value();

        lo_freq=0;
        oscInstance.rx_update_channel_lo_freq(CAP_DEVICE, "all", lo_freq);
    }

    if (cap_obs) {
        oscInstance.rx_update_device_sampling_freq(CAP_DEVICE_2,
                                                    USE_INTERN_SAMPLING_FREQ);

        guint i = 0;
        for (; i < phy_devs_count; i++) {
            // currentText().toLocal8Bit().data() is a pointer into a temporary
            // QByteArray destroyed at the semicolon. strstr() then read freed
            // heap (ASan heap-use-after-free). Keep the QString alive instead.
            const QString source = static_cast<QComboBox*>(subcomponents[i].obs_port_select)->currentText();

            if (source.contains(QLatin1String("TX"))) {
                lo_freq = mhz_scale
                        * ((QDoubleSpinBox*)(subcomponents[i].glb_widgets[subcomponents[i].trx_lo].widget))->value();
            } else {
                lo_freq = mhz_scale
                        * ((QDoubleSpinBox*)(subcomponents[i].obsrx_widgets[subcomponents[i].aux_lo].widget))->value();
            }
        }

        oscInstance.rx_update_channel_lo_freq(CAP_DEVICE_2, "all", lo_freq);
    }
}

/**
 * @brief rssi_update_label
 * @param label
 * @param chn
 * @param is_tx
 */
static void rssi_update_label(QWidget *label, struct iio_channel *chn) {
    char buf[1024];
    int ret;

    if (!label || !chn || !label->isVisible())
        return;

    /* Leave one byte so a libiio that null-terminates past len cannot smash the stack. */
    ret = iio_channel_attr_read(chn,"rssi", buf, sizeof(buf) - 1);
    if (ret >= 0)
        buf[ret < (int)sizeof(buf) ? ret : (int)sizeof(buf) - 1] = '\0';
    if (ret > 0)
        ((QLabel*)(label))->setText(buf);
    else
        ((QLabel*)(label))->setText("<error>");
}

/**
 * @brief adrv9009::rssi_update_labels
 */
void adrv9009::rssi_update_labels(void) {

    guint i = 0;
    for (; i < phy_devs_count; i++) {
        rssi_update_label(subcomponents[i].rx1_rssi, subcomponents[i].ch0);
        rssi_update_label(subcomponents[i].rx2_rssi, subcomponents[i].ch1);
        /*rssi_update_label(subcomponents[i].obs_rssi, subcomponents[i].ch2);*/
    }
}

/**
 * @brief adrv9009::update_label_from
 * @param label
 * @param dev
 * @param channel
 * @param attribute
 * @param output
 * @param unit
 * @param scale
 */
void adrv9009::update_label_from(QLabel *label, struct iio_device *dev, const char *channel,
                                 const char *attribute, bool output, const char *unit, int scale)
{

    if(!label)
        return;

    /* Never dereference a null IIO device. */
    if (!dev) {
        label->setText("N/A");
        fprintf(stderr, "ADRV9009: IIO device is NULL for channel %s\n",
                channel ? channel : "<null>");
        return;
    }

    char buf[80];
    long long val = 0;
    struct iio_channel *ch;
    int ret = -1;

    ch = iio_device_find_channel(dev, channel, output);
    if (ch) {
        ret = iio_channel_attr_read_longlong(ch, attribute, &val);

        if (scale == 1)
            snprintf(buf, sizeof(buf), "%lld %s", val, unit);
        else if (scale > 0 && scale <= 10)
            snprintf(buf, sizeof(buf), "%.1f %s", (float)val / scale, unit);
        else if (scale > 10)
            snprintf(buf, sizeof(buf), "%.2f %s", (float)val / scale, unit);
        else if (scale > 100)
            snprintf(buf, sizeof(buf), "%.3f %s", (float)val / scale, unit);
    }

    if (ret >= 0)
        label->setText(buf);
    else
        label->setText("<error>");
}

void adrv9009::update_label_with_scale_from(QLabel *label,
                                            struct iio_device *dev,
                                            const char *channel,
                                            const char *attribute,
                                            const char *attribute_scale,
                                            bool output,
                                            const char *unit,
                                            int scale)
{

    if(!label)
        return;

    /* ad7291 is optional on some board images. Never dereference a null IIO device. */
    if (!dev) {
        label->setText("N/A");
        fprintf(stderr, "ADRV9009: optional IIO device is NULL for channel %s\n",
                channel ? channel : "<null>");
        return;
    }

    char buf[80];
    long long val = 0;
    long long scale_val = 0;
    struct iio_channel *ch;
    int ret = -1;

    ch = iio_device_find_channel(dev, channel, output);
    if (ch) {
        ret = iio_channel_attr_read_longlong(ch, attribute, &val);
        ret=iio_channel_attr_read_longlong(ch, attribute_scale, &scale_val);

        val=val+scale_val;

        if (scale == 1)
            snprintf(buf, sizeof(buf), "%lld %s", val, unit);
        else if (scale > 0 && scale <= 10)
            snprintf(buf, sizeof(buf), "%.1f %s", (float)val / scale, unit);
        else if (scale > 10)
            snprintf(buf, sizeof(buf), "%.2f %s", (float)val / scale, unit);
        else if (scale > 100)
            snprintf(buf, sizeof(buf), "%.3f %s", (float)val / scale, unit);


    }

    if (ret >= 0)
        label->setText(buf);
    else
        label->setText("<error>");
}



/**
 * @brief adrv9009::glb_settings_update_labels
 */
void adrv9009::glb_settings_update_labels()
{

    char buf[1024];
    ssize_t ret;
    struct iio_channel *ch;
    guint i = 0;

    IIO_Widget iio_w;

    if (!ui)
        return;

    /* Get ensm_mode from all devices. Notify user if any of devices has a different mode than the others. */
    for (; i < phy_devs_count && ui->ensm_mode; i++) {
        if (!subcomponents[i].iio_dev)
            continue;
        ret = iio_device_attr_read(subcomponents[i].iio_dev, "ensm_mode", buf, sizeof(buf) - 1);
        if (ret >= 0)
            buf[ret < (ssize_t)sizeof(buf) ? ret : (ssize_t)sizeof(buf) - 1] = '\0';
        if (ret > 0) {
            if (i > 0) {
                if (QString::fromUtf8(buf) != ui->ensm_mode->text()) {
                    ui->ensm_mode->setText("<not synced>");
                    break;
                }
            } else {
                ui->ensm_mode->setText(buf);
            }
        } else {
            ui->ensm_mode->setText("<error>");
            break;
        }
    }

    // Temp Ad7291
    update_label_with_scale_from(ui->lblTempAd7291,
                                 ddm,"temp0", "raw","scale", false, " °C", 10);
    bool temp7291Ok = false;
    if (ui->lblTempAd7291) {
        const double temp7291Value = ui->lblTempAd7291->text().split(" ").value(0).toDouble(&temp7291Ok);
        if (temp7291Ok)
            globals::temp7291 = temp7291Value;
    }

    for (i = 0; i < phy_devs_count; i++) {
        if (!subcomponents[i].iio_dev)
            continue;

        ch = iio_device_find_channel(subcomponents[i].iio_dev, "voltage0", false);
        ret = 0;
        if (ch) {
            ret = iio_channel_attr_read(ch, "gain_control_mode", buf, sizeof(buf) - 1);
            if (ret >= 0)
                buf[ret < (ssize_t)sizeof(buf) ? ret : (ssize_t)sizeof(buf) - 1] = '\0';
        }

        if (subcomponents[i].rx_gain_control_rx1) {
            if (ret > 0)
                ((QLabel*)subcomponents[i].rx_gain_control_rx1)->setText(buf);
            else
                ((QLabel*)subcomponents[i].rx_gain_control_rx1)->setText("<error>");
        }

        ch = iio_device_find_channel(subcomponents[i].iio_dev, "voltage1", false);
        ret = 0;
        if (ch) {
            ret = iio_channel_attr_read(ch, "gain_control_mode", buf, sizeof(buf) - 1);
            if (ret >= 0)
                buf[ret < (ssize_t)sizeof(buf) ? ret : (ssize_t)sizeof(buf) - 1] = '\0';
        }

        if (subcomponents[i].rx_gain_control_rx2) {
            if (ret > 0)
                ((QLabel*)subcomponents[i].rx_gain_control_rx2)->setText(buf);
            else
                ((QLabel*)subcomponents[i].rx_gain_control_rx2)->setText("<error>");
        }

        // Temp Adrv9009. temp0 is optional on some images; do not touch a missing label.
        if (subcomponents[i].label_temp) {
            update_label_from((QLabel*)subcomponents[i].label_temp,
                              subcomponents[i].iio_dev,"temp0", "input", false, " °C", 1000);
            bool temp9009Ok = false;
            const double temp9009Value = ((QLabel*)subcomponents[i].label_temp)->text().split(" ").value(0).toDouble(&temp9009Ok);
            if (temp9009Ok)
                globals::temp9009 = temp9009Value;
        }

        update_label_from((QLabel*)subcomponents[i].label_rf_bandwidth_rx,
                          subcomponents[i].iio_dev,"voltage0", "rf_bandwidth", false, "MHz", 1000000);
        update_label_from((QLabel*)subcomponents[i].label_rf_bandwidth_obs,
                          subcomponents[i].iio_dev, "voltage2", "rf_bandwidth", false, "MHz", 1000000);
        update_label_from((QLabel*)subcomponents[i].label_rf_bandwidth_tx,
                          subcomponents[i].iio_dev, "voltage0", "rf_bandwidth", true, "MHz", 1000000);

        update_label_from((QLabel*)subcomponents[i].label_sampling_freq_rx,
                          subcomponents[i].iio_dev, "voltage0", "sampling_frequency", false, "MSPS", 1000000);
        update_label_from((QLabel*)subcomponents[i].label_sampling_freq_obs,
                          subcomponents[i].iio_dev, "voltage2", "sampling_frequency", false, "MSPS", 1000000);
        update_label_from((QLabel*)subcomponents[i].label_sampling_freq_tx,
                          subcomponents[i].iio_dev, "voltage0", "sampling_frequency", true, "MSPS", 1000000);

        if (subcomponents[i].rx1_gain) {
            iio_w.iio_widget_update(&subcomponents[i].rx_widgets[subcomponents[i].rx1_gain]);
        }

        if (subcomponents[i].rx2_gain) {
            iio_w.iio_widget_update(&subcomponents[i].rx_widgets[subcomponents[i].rx2_gain]);
        }

        if (subcomponents[i].obs_gain) {
            iio_w.iio_widget_update(&subcomponents[i].obsrx_widgets[subcomponents[i].obs_gain]);
        }
    }


}

#pragma endregion }

#pragma region Save Widgets Values {

/**
 * @brief adrv9009::save_widget_value
 * @param iio_w
 */
void adrv9009::save_widget_value(struct iio_widget *iio_w) {
    iio_w->save(iio_w);
}

#pragma endregion }

#pragma region Widget Signals {

/**
 * @brief adrv9009::ConnectSignals
 */
void adrv9009::ConnectSignals(){

    //saeid raziani
    //    QObject::connect(ui->tx_lo_freq, SIGNAL(valueChanged(QString)),this,SIGNAL(frqHasFocusedSignal(QString)));

    QObject::connect(ui->rx1_phase_rotation,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](){
        rx_phase_rotation_set(ui->rx1_phase_rotation,(gpointer *)0);
    });

    QObject::connect(ui->rx2_phase_rotation,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](){
        rx_phase_rotation_set(ui->rx2_phase_rotation,(gpointer *)2);
    });

    //saeid raziani
    //    QObject::connect(ui->tx_lo_freq,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](){
    //        tx_sample_rate_changed(ui->tx_lo_freq);
    //    });

    //

    QObject::connect(ui->fpga_rx_frequency_available,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
        int_dec_update_cb();
    });

    //                QObject::connect(ui->cmb_gain_control_mode_obs,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
    //                    glb_settings_update_labels();
    //                });

    QObject::connect(ui->gain_control_mode_available_rx1,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
        glb_settings_update_labels();
    });

    QObject::connect(ui->fpga_tx_frequency_available,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
        int_dec_update_cb();
    });

    QObject::connect(ui->ensm_mode_available,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
        glb_settings_update_labels();
    });

    for (guint i = 0; i < phy_devs_count; i++) {
        make_widget_update_signal_based(subcomponents[i].glb_widgets, subcomponents[i].num_glb);
        make_widget_update_signal_based(subcomponents[i].rx_widgets, subcomponents[i].num_rx);
        make_widget_update_signal_based(subcomponents[i].obsrx_widgets, subcomponents[i].num_obsrx);
        make_widget_update_signal_based(subcomponents[i].tx_widgets, subcomponents[i].num_tx);

        //                if (subcomponents[i].rx_sample_freq)
        //                {
        //                    iio_spin_button_set_on_complete_function(&subcomponents[i].rx_widgets[subcomponents[i].rx_sample_freq],
        //                            ui->sample_frequency_changed_cb, NULL);
        //                }
        //                if (subcomponents[i].tx_sample_freq)
        //                {
        //                    iio_spin_button_set_on_complete_function(&subcomponents[i].tx_widgets[subcomponents[i].tx_sample_freq],
        //                            sample_frequency_changed_cb, NULL);
        //                }
        //                if (subcomponents[i].trx_lo)
        //                {
        //                    iio_spin_button_set_on_complete_function(&subcomponents[i].glb_widgets[subcomponents[i].trx_lo],
        //                            sample_frequency_changed_cb, NULL);
        //                }
        //                if (subcomponents[i].aux_lo)
        //                {
        //                    iio_spin_button_set_on_complete_function(&subcomponents[i].obsrx_widgets[subcomponents[i].aux_lo],
        //                            sample_frequency_changed_cb, NULL);
        //                }
    }

}

/**
 * @brief adrv9009::make_widget_update_signal_based
 * @param widgets
 * @param num_widgets
 */
void adrv9009::make_widget_update_signal_based(struct iio_widget *widgets,
                                               unsigned int num_widgets)
{
    char signal_name[25];
    unsigned int i;

    QString className="";

    for (i = 0; i < num_widgets; i++) {

        if(widgets[i].widget)
        {
            className=widgets[i].widget->metaObject()->className();

            if (className=="QCheckBox")
                QObject::connect((QCheckBox*)widgets[i].widget,
                                 &QCheckBox::stateChanged,
                                 [=](int v){
                    save_widget_value(&widgets[i]);
                });
            else if (className=="QSpinBox") {
                QObject::connect((QSpinBox*)widgets[i].widget,
                                 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                                 [=](int value){
                    widgets[i].value=std::to_string(value).c_str ();
                    save_widget_value(&widgets[i]);
                });

            }
            else if (className=="QDoubleSpinBox") {
                QObject::connect((QDoubleSpinBox*)widgets[i].widget,
                                 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                 [=](double value){

                    widgets[i].value=QString::number(value).toLocal8Bit().data();

                    save_widget_value(&widgets[i]);
                });
            }

            else if (className=="QAbstractButton")
                QObject::connect((QAbstractButton*)widgets[i].widget,
                                 &QAbstractButton::clicked,
                                 [=](){
                    save_widget_value(&widgets[i]);
                });
            else if (className=="QComboBox")
            {
                QObject::connect((QComboBox*)widgets[i].widget,
                                 &QComboBox::currentTextChanged,
                                 [=](){
                    save_widget_value(&widgets[i]);
                });
            }
            else if (className=="QPushButton")
            {
                QObject::connect((QPushButton*)widgets[i].widget,
                                 &QPushButton::clicked,
                                 [=](){
                    save_widget_value(&widgets[i]);
                });

                sprintf(signal_name, "%s", "clicked");

            }
            else
                printf("unhandled widget type, attribute: %s (%u)\n",
                       widgets[i].attr_name, i);
        }

    }

}

#pragma endregion }

#pragma region Initialize {

/*
 * Gets all devices that have their name starting with the given sequence.
 * Returns an array of 'struct iio_device *' elements.
 */
GArray * adrv9009::get_iio_devices_starting_with(struct iio_context *ctx, const char *sequence)
{
    GArray *devices = g_array_new(FALSE, FALSE, sizeof(struct iio_devices *));
    size_t i = 0;

    iio_utils iioUtils;
    for (; i < iio_context_get_devices_count(ctx); i++) {
        struct iio_device *dev = iio_context_get_device(ctx, i);
        const char *dev_name = iio_device_get_name(dev);
        char *label = iioUtils.iio_get_device_label(dev);

        if ((label && !strncmp(sequence, label, strlen(sequence))) ||
                (dev_name && !strncmp(sequence, dev_name, strlen(sequence)))) {
            g_array_append_val(devices, dev);
        }

        g_free(label);
    }

    g_array_sort(devices, iioUtils.iio_dev_cmp_by_name);

    return devices;
}

/**
 * @brief Initialize all widgets for adrv9009
 * @return
 */
QWidget *adrv9009::init()
{


    //saeid raziani
//    connect(ui->rx2_powerdown_en,&QCheckBox::stateChanged,[&](int state){
//        if (state == 0)
//        {
//            ui->rx2_powerdown_en->setStyleSheet("background-color:red");
//            ui->rx2_powerdown_en->setChecked(true);
//        }
//        else {
//            ui->rx2_powerdown_en->setStyleSheet("background-color:green");
//        }
//    });

//    connect(ui->rx1_powerdown_en,&QCheckBox::stateChanged,[&](int state){
//        if (state == 0)
//        {
//            ui->rx1_powerdown_en->setStyleSheet("background-color:red");
//            ui->rx1_powerdown_en->setChecked(true);
//        }
//        else {
//            ui->rx1_powerdown_en->setStyleSheet("background-color:green");
//        }
//    });


//    connect(ui->tx2_powerdown_en,&QCheckBox::stateChanged,[&](int state){
//        if (state == 0)
//        {
//            ui->tx2_powerdown_en->setStyleSheet("background-color:red");
//            ui->tx2_powerdown_en->setChecked(true);
//        }
//        else {
//            ui->tx2_powerdown_en->setStyleSheet("background-color:green");
//        }
//    });

//    connect(ui->tx1_powerdown_en,&QCheckBox::stateChanged,[&](int state){
//        if (state == 0)
//        {
//            ui->tx1_powerdown_en->setStyleSheet("background-color:red");
//            ui->tx1_powerdown_en->setChecked(true);
//        }
//        else {
//            ui->tx1_powerdown_en->setStyleSheet("background-color:green");
//        }
//    });


//    connect(ui->obs2_powerdown_en,&QCheckBox::stateChanged,[&](int state){
//        if (state == 0)
//        {
//            ui->obs2_powerdown_en->setStyleSheet("background-color:red");
//            ui->obs2_powerdown_en->setChecked(true);
//        }
//        else {
//            ui->obs2_powerdown_en->setStyleSheet("background-color:green");
//        }
//    });

//    connect(ui->obs1_powerdown_en,&QCheckBox::stateChanged,this,[&](int state){
//        if (state == 0)
//        {
//            ui->obs1_powerdown_en->setStyleSheet("color:red");
////            ui->obs1_powerdown_en->setChecked(false);
//        }
//        else {
//            ui->obs1_powerdown_en->setStyleSheet("color:green");
//        }
//    });

    const char *freq_name;
    struct iio_channel *ch;

    can_update_widgets = false;
    num_fpga = 0;

    if (!globals::ctx)
        return NULL;

    dds = iio_context_find_device(globals::ctx, DDS_DEVICE);
    ddm = iio_context_find_device(globals::ctx, DDM_DEVICE);
    if (!ddm)
        fprintf(stderr, "ADRV9009: optional IIO device '%s' was not found; temperature display will be N/A\n", DDM_DEVICE);

    cap = iio_context_find_device(globals::ctx, CAP_DEVICE);
    cap_obs = iio_context_find_device(globals::ctx, CAP_DEVICE_2);

    /* Are there more adrv9009-phy devices? */
    GArray *phy_adrv9009_devs = get_iio_devices_starting_with(globals::ctx, PHY_DEVICE);
    phy_devs_count = phy_adrv9009_devs->len;
    plugin_single_device_mode = phy_devs_count == 1;

    //saeid raziani
//    dacDataManager dac_data_manager;
    dac_data_manager=new dacDataManager();

    guint i ;

    for (i = 0; i < phy_devs_count; i++) {
        subcomponents.append(plugin_subcomponent());

        struct iio_device *dev = g_array_index(phy_adrv9009_devs, struct iio_device*, i);

        subcomponents[i].iio_dev = dev;
        subcomponents[i].ch0 = iio_device_find_channel(dev, "voltage0", false); /* RX1 */
        subcomponents[i].ch1 = iio_device_find_channel(dev, "voltage1", false); /* RX2 */
        subcomponents[i].ch2 = iio_device_find_channel(dev, "voltage2", false); /* OBS-RX1 */
        subcomponents[i].ch3 = iio_device_find_channel(dev, "voltage3", false); /* OBS-RX1 */
        subcomponents[i].alt_ch0 = iio_device_find_channel(dev, "altvoltage0", true);
        subcomponents[i].alt_ch1 = iio_device_find_channel(dev, "altvoltage1", true);
        subcomponents[i].out_ch0 = iio_device_find_channel(dev, "voltage0", true); /* TX1 */
        subcomponents[i].out_ch1 = iio_device_find_channel(dev, "voltage1", true); /* TX2 */

        subcomponents[i].num_glb = 0;
        subcomponents[i].num_tx = 0;
        subcomponents[i].num_rx = 0;
        subcomponents[i].num_obsrx = 0;
        subcomponents[i].rx1_gain = 0;
        subcomponents[i].rx2_gain = 0;
        subcomponents[i].obs_gain = 0;
        subcomponents[i].trx_lo = 0;
        subcomponents[i].aux_lo = 0;
        subcomponents[i].rx_sample_freq = 0;
        subcomponents[i].tx_sample_freq = 0;

        subcomponents[i].sr_attribs_count = ARRAY_SIZE(adrv9009_sr_attribs);
        subcomponents[i].sr_attribs = g_new(char *, subcomponents[i].sr_attribs_count);

        size_t n = 0;
        for (; n < subcomponents[i].sr_attribs_count; n++)
        {
            //            if(n==29)
            //                qInfo()<< g_strconcat(
            //                              iio_device_get_name(subcomponents[i].iio_dev), adrv9009_sr_attribs[n], NULL);;
            subcomponents[i].sr_attribs[n] = g_strconcat(
                        iio_device_get_name(subcomponents[i].iio_dev), adrv9009_sr_attribs[n], NULL);
        }
    }

//    ui->obs1_powerdown_en->setChecked(false);
    if (dds) {
        dac_tx_manager = dac_data_manager->dac_data_manager_new(dds, NULL, globals::ctx);
        dac_data_manager->dac_data_manager_set_buffer_size_alignment(dac_tx_manager, 16);
    }
//    dac_tx_manager->dac1.txs[0].dds_mode_widget->setStyleSheet("background-color:red");

    /* Keep references to widgets for each subcomponent */
    for (i = 0; i < phy_devs_count; i++) {

        /* Sensors */
        subcomponents[i].label_temp = ui->lblTempAdrv9009;

        /* Receive Chain */
        subcomponents[i].rx_gain_control_rx1 = ui->gain_control_mode_rx1;
        subcomponents[i].rx_gain_control_rx2 = ui->gain_control_mode_rx2;
        subcomponents[i].rx_gain_control_modes_rx1 = ui->gain_control_mode_available_rx1;
        subcomponents[i].rx1_rssi = ui->rssi_rx1;
        subcomponents[i].rx2_rssi = ui->rssi_rx2;
        subcomponents[i].label_rf_bandwidth_rx = ui->label_rf_bandwidth_rx;
        subcomponents[i].label_sampling_freq_rx = ui->label_sampling_freq_rx;

        /* Transmit Chain */
        subcomponents[i].label_rf_bandwidth_tx = ui->label_rf_bandwidth_tx;
        subcomponents[i].label_sampling_freq_tx = ui->label_sampling_freq_tx;

        /* Observation Receive Chain */
        subcomponents[i].obs_port_select = ui->rf_port_select_obs;
        subcomponents[i].label_rf_bandwidth_obs = ui->label_rf_bandwidth_obs;
        subcomponents[i].label_sampling_freq_obs = ui->label_sampling_freq_obs;

        /* FPGA */
        subcomponents[i].rx_phase_rotation[0] = ui->rx1_phase_rotation;
        subcomponents[i].rx_phase_rotation[1] = ui->rx2_phase_rotation;
    }

//    dac_data_manager::dac_data_manager p = new dac_data_manager;
//    dac_data_manager *manager =(dac_data_manager *) calloc(1, sizeof(struct dac_data_manager));

    dac_tx_manager = dac_data_manager_instance->dac_data_manager_new(dds, NULL, globals::ctx);
    dac_data_manager_instance->dac_data_manager_set_buffer_size_alignment(dac_tx_manager, 16);


    //saeid raziani
//    dac_tx_manager->dac1.txs[0].dds_mode_widget->setStyleSheet("background-color:red");
    dac_tx_manager->dac1.txs[0].ch_i.t2.freq->valueChanged(0);
    dac_tx_manager->dac1.txs[0].ch_i.t1.freq->valueChanged(0);

    dac_tx_manager->dac1.txs[0].ch_q.t1.freq->valueChanged(0);
    dac_tx_manager->dac1.txs[0].ch_q.t2.freq->valueChanged(0);

    dac_tx_manager->dac1.txs[1].ch_i.t2.freq->valueChanged(0);
    dac_tx_manager->dac1.txs[1].ch_i.t1.freq->valueChanged(0);

    dac_tx_manager->dac1.txs[1].ch_q.t1.freq->valueChanged(0);
    dac_tx_manager->dac1.txs[1].ch_q.t2.freq->valueChanged(0);



    /* Transmit Chain */

    if (dac_tx_manager)
        ui->dds_container_group->setLayout(dac_data_manager->dac_data_manager_get_gui_container(dac_tx_manager));

    IIO_Widget iio_w;

    if (cap && num_fpga < 2) {
        ch = iio_device_find_channel(cap, "voltage0_i", false);
        if (ch && iio_channel_find_attr(ch, "sampling_frequency_available")) {
            iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++],
                    cap, ch, "sampling_frequency",
                    "sampling_frequency_available",
                    ui->fpga_rx_frequency_available, NULL);
        } else {
            //			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(
            //								  "receive_frame_dma_buf")));
        }
    } else {
        //		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(
        //								  "receive_frame_dma_buf")));
    }

    if (dds && num_fpga < 2) {
        ch = iio_device_find_channel(dds, "voltage0", true);
        if (ch && iio_channel_find_attr(ch, "sampling_frequency_available")) {
            iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++],
                    dds, ch, "sampling_frequency",
                    "sampling_frequency_available",
                    ui->fpga_tx_frequency_available, NULL);
        } else {
            //            gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(
            //                                                              "transmit_frame_dma_buf")));
        }
    } else {
        //		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(
        //								  "transmit_frame_dma_buf")));
    }

    /* Treat 'ensm_mode_available' separately because it will be shared between devices (when more are avaialable) */
    if (!plugin_single_device_mode) {
        iio_w.iio_combo_box_init(&iio_ensm_mode_available, subcomponents[0].iio_dev, NULL,
                "ensm_mode", "ensm_mode_available", ui->ensm_mode_available, NULL);
    }

    for (i = 0; i < phy_devs_count; i++) {
        subcomponents[i].glb_widgets = subcomponents[i].widgets;

        if (plugin_single_device_mode) {
            iio_w.iio_combo_box_init(&subcomponents[0].glb_widgets[subcomponents[0].num_glb++],
                    subcomponents[0].iio_dev, NULL, "ensm_mode", "ensm_mode_available",
                    ui->ensm_mode_available, NULL);
        }

        /* Global settings */

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_rx_qec_en",
                ui->calibrate_rx_qec_en, 0);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_tx_qec_en",
                ui->calibrate_tx_qec_en, 0);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_tx_lol_en",
                ui->calibrate_tx_lol_en, 0);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_tx_lol_ext_en",
                ui->calibrate_tx_lol_ext_en, 0);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_rx_phase_correction_en",
                ui->calibrate_rx_phase_correction_en, 0);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate_fhm_en",
                ui->calibrate_fhm_en, 0);

        iio_w.iio_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, NULL, "calibrate",
                ui->calibrate);

        subcomponents[i].trx_lo = subcomponents[i].num_glb;

        if (iio_channel_find_attr(subcomponents[i].alt_ch0, "frequency"))
            freq_name = "frequency";
        else
            freq_name = "TRX_LO_frequency";

        iio_w.iio_spin_button_s64_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, subcomponents[i].alt_ch0,
                freq_name, ui->tx_lo_freq, &mhz_scale);
        iio_w.iio_spin_button_add_progress(&subcomponents[i].glb_widgets[subcomponents[i].num_glb - 1]);

        iio_w.iio_toggle_button_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, subcomponents[i].alt_ch0,
                "frequency_hopping_mode_enable", ui->fhm_enable, 0);

        iio_w.iio_spin_button_s64_init(&subcomponents[i].glb_widgets[subcomponents[i].num_glb++],
                subcomponents[i].iio_dev, subcomponents[i].alt_ch0,
                "frequency_hopping_mode", ui->tx_lo_freq_hop, &mhz_scale);

        iio_w.iio_spin_button_add_progress(&subcomponents[i].glb_widgets[subcomponents[i].num_glb - 1]);

        subcomponents[i].rx_widgets = &subcomponents[i].glb_widgets[subcomponents[i].num_glb];

        /* Receive Chain */
        if (subcomponents[i].ch0 && subcomponents[i].ch1) {
            iio_w.iio_combo_box_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "gain_control_mode",
                    "gain_control_mode_available",
                    subcomponents[i].rx_gain_control_modes_rx1, NULL);

            subcomponents[i].rx1_gain = subcomponents[i].num_rx;
            iio_w.iio_spin_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "hardwaregain",
                    ui->hardware_gain_rx1, NULL);

            subcomponents[i].rx2_gain = subcomponents[i].num_rx;
            iio_w.iio_spin_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch1, "hardwaregain",
                    ui->hardware_gain_rx2, NULL);

            subcomponents[i].rx_sample_freq = subcomponents[i].num_rx;
            subcomponents[i].num_rx++;
            //                iio_w.iio_spin_button_int_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
            //                                subcomponents[i].iio_dev, subcomponents[i].ch0, "sampling_frequency",
            //                                ui->sampling_freq_rx, &mhz_scale);
            iio_w.iio_spin_button_add_progress(&subcomponents[i].rx_widgets[subcomponents[i].num_rx - 1]);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "quadrature_tracking_en",
                    ui->rx1_quadrature_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch1, "quadrature_tracking_en",
                    ui->rx2_quadrature_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "hd2_tracking_en",
                    ui->rx1_hd2_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch1, "hd2_tracking_en",
                    ui->rx2_hd2_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "gain_control_pin_mode_en",
                    ui->rx1_gain_control_pin_mode_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch1, "gain_control_pin_mode_en",
                    ui->rx2_gain_control_pin_mode_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch0, "powerdown",
                    ui->rx1_powerdown_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].rx_widgets[subcomponents[i].num_rx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch1, "powerdown",
                    ui->rx2_powerdown_en,0);

        } else {
            //                gtk_widget_hide(gtk_widget_get_parent(section_setting[SECTION_RX]));
        }

        /* Observation Receiver Chain */

        subcomponents[i].obsrx_widgets = &subcomponents[i].rx_widgets[subcomponents[i].num_rx];

        if (subcomponents[i].ch2) {
            iio_w.iio_combo_box_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch2, "rf_port_select",
                    "rf_port_select_available",
                    subcomponents[i].obs_port_select, NULL);

            subcomponents[i].obs_gain = subcomponents[i].num_obsrx;
            iio_w.iio_spin_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch2, "hardwaregain",
                    ui->hardware_gain_obs1, NULL);

            iio_w.iio_toggle_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch2, "quadrature_tracking_en",
                    ui->obs1_quadrature_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                    subcomponents[i].iio_dev, subcomponents[i].ch2, "powerdown",
                    ui->obs1_powerdown_en,0);

            if (subcomponents[i].ch3) {
                iio_w.iio_spin_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                        subcomponents[i].iio_dev, subcomponents[i].ch3, "hardwaregain",
                        ui->hardware_gain_obs2, NULL);

                iio_w.iio_toggle_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                        subcomponents[i].iio_dev, subcomponents[i].ch3, "quadrature_tracking_en",
                        ui->obs2_quadrature_tracking_en,0);

                iio_w.iio_toggle_button_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                        subcomponents[i].iio_dev, subcomponents[i].ch3, "powerdown",
                        ui->obs2_powerdown_en,0);
            }

            subcomponents[i].aux_lo = subcomponents[i].num_obsrx;

            if (iio_channel_find_attr(subcomponents[i].alt_ch1, "frequency"))
                freq_name = "frequency";
            else
                freq_name = "AUX_OBS_RX_LO_frequency";

            iio_w.iio_spin_button_s64_init(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx++],
                    subcomponents[i].iio_dev, subcomponents[i].alt_ch1, freq_name,
                    ui->sn_lo_freq, &mhz_scale);
            iio_w.iio_spin_button_add_progress(&subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx - 1]);
        } else {
            //                gtk_widget_hide(gtk_widget_get_parent(section_setting[SECTION_OBS]));
        }

        /* Transmit Chain */

        subcomponents[i].tx_widgets = &subcomponents[i].obsrx_widgets[subcomponents[i].num_obsrx];

        if (subcomponents[i].out_ch0 && subcomponents[i].out_ch1) {
            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "pa_protection_en",
                    ui->pa_protection,0);

            iio_w.iio_spin_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "hardwaregain",
                    ui->hardware_gain_tx1, &inv_scale);

            iio_w.iio_spin_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch1, "hardwaregain",
                    ui->hardware_gain_tx2, &inv_scale);

            subcomponents[i].tx_sample_freq = subcomponents[i].num_tx;

            //                iio_w.iio_spin_button_int_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
            //                                subcomponents[i].iio_dev, subcomponents[i].out_ch0, "sampling_frequency",
            //                                ui->sampling_freq_tx, &mhz_scale);
            subcomponents[i].num_tx++;
            iio_w.iio_spin_button_add_progress(&subcomponents[i].tx_widgets[subcomponents[i].num_tx - 1]);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "quadrature_tracking_en",
                    ui->tx1_quadrature_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch1, "quadrature_tracking_en",
                    ui->tx2_quadrature_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "lo_leakage_tracking_en",
                    ui->tx1_lo_leakage_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch1, "lo_leakage_tracking_en",
                    ui->tx2_lo_leakage_tracking_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "atten_control_pin_mode_en",
                    ui->tx1_atten_control_pin_mode_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch1, "atten_control_pin_mode_en",
                    ui->tx2_atten_control_pin_mode_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch0, "powerdown",
                    ui->tx1_powerdown_en,0);

            iio_w.iio_toggle_button_init(&subcomponents[i].tx_widgets[subcomponents[i].num_tx++],
                    subcomponents[i].iio_dev, subcomponents[i].out_ch1, "powerdown",
                    ui->tx2_powerdown_en,0);

        } else {
            //                gtk_widget_hide(gtk_widget_get_parent(section_setting[SECTION_TX]));
            //                gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object( "calibrate_tx_qec_en")));
            //                gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object( "calibrate_tx_lol_en")));
            //                gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object( "calibrate_tx_lol_ext_en")));
        }
    }

    //    if (ini_fn)
    //            load_profile(NULL, ini_fn);

    /* Update all widgets with current values */
    printf("Updating widgets...\n");
    update_widgets();
    rx_freq_info_update();
    printf("Updating FIR filter...\n");
    profile_update();
    glb_settings_update_labels();
    rssi_update_labels();

    if (dds)
    {
        dac_data_manager->dac_data_manager_freq_widgets_range_update(dac_tx_manager, get_gui_tx_sampling_freq() / 2.0);
        dac_data_manager->dac_data_manager_update_iio_widgets(dac_tx_manager);
    }

    ConnectSignals();

    // Must stay on the GUI thread. The old QtConcurrent loop called QWidget
    // and libiio from a pool thread while init() was still running, which
    // corrupts Qt and trips "stack smashing detected" on some PCs only.
    if (!refreshTimer) {
        refreshTimer = new QTimer(this);
        refreshTimer->setInterval(20000);
        connect(refreshTimer, &QTimer::timeout, this, [this]{
            if (!globals::status || hopping)
                return;
            if (ui->tx_lo_freq && ui->tx_lo_freq->hasFocus())
                return;
            update_widgets();
            glb_settings_update_labels();
            rssi_update_labels();
            int_dec_update_cb();
        });
    }
    refreshTimer->start();

    //saeid raziani
    QObject::connect(ui->profile_config,&QPushButton::clicked,[=](){

        on_profile_config_clicked(fileAddress);
    });

    //    setting.ReadSettingFile();
    //    QString lastFilter=setting.getLastFilter();
    //    if(lastFilter!="")
    //    {
    //        if(progress==nullptr)
    //            progress=new QProgressDialog();
    //        progress->setLabelText("Loading last Filter");
    //        progress->setRange(0,100);
    //        QList<QPushButton *> L=progress->findChildren<QPushButton *>();
    //        L.at(0)->hide();

#pragma region Old Code {

    //        QtConcurrent::run([=](){
    //            load_tal_profile(lastFilter);
    //        });

    //        progress->exec();

    //    if (!env_freq_mix_sign) {
    //        updn_freq_mix_sign = 1;
    //    } else {
    //        if (!strncmp(env_freq_mix_sign, "-", 1))
    //            updn_freq_mix_sign = -1;
    //        else
    //            updn_freq_mix_sign = 1;
    //    }

    //    is_2rx_2tx = ch1 && iio_channel_find_attr(ch1, "hardwaregain");

    //    //----------------------------------------------------------------

    //    /* Transmit Chain */

    //    dacDataManager *dacManager=new dacDataManager();
    //    if (dac_tx_manager)
    //        ui->dds_container_group->setLayout(dacManager->dac_data_manager_get_gui_container(dac_tx_manager));


    //    rx_phase_rotation[0] = ui->rx1_phase_rotation;
    //    rx_phase_rotation[1] = ui->rx2_phase_rotation;

    //    //----------------------------------------------------------------

    //    glb_widgets = widgets;

    //    IIO_Widget iio_w;

    //    /* Global settings */

    //    iio_w.iio_combo_box_init(&glb_widgets[num_glb++], dev, NULL, "ensm_mode",
    //            "ensm_mode_available", ui->ensm_mode_available, NULL);

    //    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //    //                                 "calibrate_dpd_en", ui->calibrate_dpd_en, 0);
    //    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //    //                                 "calibrate_clgc_en", ui->calibrate_clgc_en, 0);
    //    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //    //                                 "calibrate_vswr_en", ui->calibrate_vswr_en, 0);

    //    num_glb+=3;

    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //            "calibrate_rx_qec_en", ui->calibrate_rx_qec_en, 0);

    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //            "calibrate_tx_qec_en", ui->calibrate_tx_qec_en, 0);

    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //            "calibrate_tx_lol_en", ui->calibrate_tx_lol_en, 0);

    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //            "calibrate_tx_lol_ext_en", ui->calibrate_tx_lol_ext_en, 0);

    //    iio_w.iio_button_init(&glb_widgets[num_glb++], dev, NULL,
    //            "calibrate", ui->calibrate);

    //    rx_widgets = &glb_widgets[num_glb];

    //    /* Receive Chain */

    //    iio_w.iio_combo_box_init(&rx_widgets[num_rx++], dev, ch0, "gain_control_mode",
    //            "gain_control_mode_available", ui->gain_control_mode_available_rx1, NULL);
    //    //    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch0,
    //    //            "temp_comp_gain", ui->temp_comp_gain_rx1, NULL);
    //    //    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch1,
    //    //            "temp_comp_gain", ui->temp_comp_gain_rx2, NULL);

    //    rx1_gain = num_rx;
    //    //    qInfo()<<"Num Rx -> "<<num_rx;

    //    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch0,
    //            "hardwaregain", ui->hardware_gain_rx1, NULL);

    //    //    if (is_2rx_2tx) {
    //    //        rx2_gain = num_rx;
    //    //        iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch1,
    //    //                "hardwaregain", ui->hardware_gain_rx2, NULL);
    //    //    }

    //    rx_sample_freq = num_rx;

    //    //    num_rx++;
    //    //        iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch0,
    //    //                "sampling_frequency", ui->sampling_freq_rx, &mhz_scale);
    //    //    iio_w.iio_spin_button_add_progress(&rx_widgets[num_rx - 1]);

    //    iio_w.iio_toggle_button_init(&rx_widgets[num_rx++], dev, ch0,
    //            "quadrature_tracking_en", ui->rx1_quadrature_tracking_en, 0);

    //    //    iio_w.iio_toggle_button_init(&rx_widgets[num_rx++], dev, ch1,
    //    //            "quadrature_tracking_en", ui->rx2_quadrature_tracking_en, 0);

    //    rx_lo = num_rx;

    //    ch1 = iio_device_find_channel(dev, "altvoltage0", true);
    //    if (iio_channel_find_attr(ch1, "frequency"))
    //        freq_name = "frequency";
    //    else
    //        freq_name = "RX_LO_frequency";

    //    //    iio_w.iio_spin_button_s64_init(&rx_widgets[num_rx++], dev, ch1,
    //    //            freq_name, ui->rx_lo_freq, &mhz_scale);
    //    //    iio_w.iio_spin_button_add_progress(&rx_widgets[num_rx - 1]);


    //    /* Observation Receiver Chain */

    //    obsrx_widgets = &rx_widgets[num_rx];

    //    //    iio_w.iio_combo_box_init(&obsrx_widgets[num_obsrx++], dev, ch2,
    //    //            "gain_control_mode", "gain_control_mode_available",
    //    //            ui->cmb_gain_control_mode_obs, NULL);

    //    iio_w.iio_combo_box_init(&obsrx_widgets[num_obsrx++], dev, ch2, "rf_port_select",
    //            "rf_port_select_available", ui->rf_port_select_obs, NULL);

    //    //    iio_w.iio_spin_button_init(&obsrx_widgets[num_obsrx++], dev, ch2,
    //    //            "temp_comp_gain", ui->temp_comp_gain_obs, NULL);

    //    obs_gain = num_obsrx;
    //    iio_w.iio_spin_button_init(&obsrx_widgets[num_obsrx++], dev, ch2,
    //            "hardwaregain", ui->hardware_gain_obs, NULL);

    //    iio_w.iio_toggle_button_init(&obsrx_widgets[num_obsrx++], dev, ch2,
    //            "quadrature_tracking_en", ui->obs_quadrature_tracking_en, 0);

    //    sn_lo = num_obsrx;
    //    ch2 = iio_device_find_channel(dev, "altvoltage2", true);
    //    if (iio_channel_find_attr(ch2, "frequency"))
    //        freq_name = "frequency";
    //    else
    //        freq_name = "RX_SN_LO_frequency";

    //    iio_w.iio_spin_button_s64_init(&obsrx_widgets[num_obsrx++], dev, ch2,
    //            freq_name, ui->sn_lo_freq, &mhz_scale);
    //    iio_w.iio_spin_button_add_progress(&obsrx_widgets[num_obsrx - 1]);

    //    //    iio_w.iio_toggle_button_init(&obsrx_widgets[num_obsrx++],
    //    //     		dev, ch2, "external",
    //    //     		"obs_lo_external,0)

    //    /* Transmit Chain */

    //    tx_widgets = &obsrx_widgets[num_obsrx];

    //    ch0 = iio_device_find_channel(dev, "voltage0", true);
    //    if (is_2rx_2tx)
    //        ch1 = iio_device_find_channel(dev, "voltage1", true);

    //    iio_w.iio_spin_button_init(&tx_widgets[num_tx++], dev, ch0,
    //            "hardwaregain", ui->hardware_gain_tx1, &inv_scale);

    //    //    if (is_2rx_2tx)
    //    //        iio_w.iio_spin_button_init(&tx_widgets[num_tx++], dev, ch1,
    //    //                "hardwaregain", ui->hardware_gain_tx2, &inv_scale);
    //    tx_sample_freq = num_tx;

    //    num_tx++;
    //    //        iio_w.iio_spin_button_int_init(&tx_widgets[num_tx++], dev, ch0,
    //    //                "sampling_frequency", ui->sampling_freq_tx, &mhz_scale);
    //    //    iio_w.iio_spin_button_add_progress(&tx_widgets[num_tx - 1]);

    //    iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], dev, ch0,
    //            "quadrature_tracking_en", ui->tx1_quadrature_tracking_en, 0);

    //    iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], dev, ch0,
    //            "lo_leakage_tracking_en", ui->tx1_lo_leakage_tracking_en, 0);

    //    //    if (is_2rx_2tx) {
    //    //        iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], dev, ch1,
    //    //                "quadrature_tracking_en", ui->tx2_quadrature_tracking_en,
    //    //                0);

    //    //        iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], dev, ch1,
    //    //                "lo_leakage_tracking_en", ui->tx2_lo_leakage_tracking_en,
    //    //                0);
    //    //    }

    //    has_dpd = !!iio_channel_find_attr(ch0, "dpd_tracking_en");

    //    if (has_dpd) {
    //        /* DPD */
    //        // TODO: First Build UI
    //    }
    //    else {
    //        // ui->frame_dpd.hide();
    //        // ui->frame_clgc.hide();
    //        // ui->frame_vswr.hide();
    //        //        ui->calibrate_dpd_en->setVisible(false);
    //        //        ui->calibrate_clgc_en->setVisible(false);
    //        //        ui->calibrate_vswr_en->setVisible(false);
    //    }

    //    tx_lo = num_tx;
    //    ch1 = iio_device_find_channel(dev, "altvoltage1", true);

    //    if (iio_channel_find_attr(ch1, "frequency"))
    //        freq_name = "frequency";
    //    else
    //        freq_name = "TX_LO_frequency";

    //    //    iio_w.iio_spin_button_s64_init(&tx_widgets[num_tx++], dev, ch1,
    //    //            freq_name, ui->tx_lo_freq, &mhz_scale);

    //    iio_w.iio_spin_button_add_progress(&tx_widgets[num_tx - 1]);

    //    /* FPGA widgets */
    //    fpga_widgets = &tx_widgets[num_tx];

    //    ch0 = iio_device_find_channel(dds, "voltage0", true);
    //    if (iio_channel_find_attr(ch0, "sampling_frequency_available")) {
    //        iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++], dds, ch0,
    //                "sampling_frequency", "sampling_frequency_available",
    //                ui->fpga_tx_frequency_available, NULL);
    //    } else {
    //        // TODO: Create UI if need
    //        //ui->transmit_frame_dma_buf.setVisible(false);
    //    }

    //    ch0 = iio_device_find_channel(cap, "voltage0_i", false);
    //    if (iio_channel_find_attr(ch0, "sampling_frequency_available")) {
    //        iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++], cap, ch0,
    //                "sampling_frequency", "sampling_frequency_available",
    //                ui->fpga_rx_frequency_available, NULL);
    //    } else {
    //        // TODO: Create UI if need
    //        //ui->receive_frame_dma_buf.setVisible(false);
    //    }

    //    /* Update all widgets with current values */
    //    printf("Updating widgets...\n");
    //    update_widgets();

    //    rx_freq_info_update();
    //    printf("Updating FIR filter...\n");

    //    glb_settings_update_labels();
    //    rssi_update_labels();

    //    /* Connect signals */


    //    make_widget_update_signal_based(glb_widgets, num_glb);
    //    make_widget_update_signal_based(rx_widgets, num_rx);
    //    make_widget_update_signal_based(obsrx_widgets, num_obsrx);
    //    make_widget_update_signal_based(tx_widgets, num_tx);
    //    make_widget_update_signal_based(fpga_widgets, num_fpga);



#pragma endregion }

    return NULL;
}

#pragma endregion }

#pragma region Buttons {

/**
 * @brief adrv9009::on_btnReloadSettings_clicked
 */


void adrv9009::on_btnReloadSettings_clicked()
{
    update_widgets();
    profile_update();
    rx_freq_info_update();
    glb_settings_update_labels();
    rssi_update_labels();
    rx_phase_rotation_update();
}

void adrv9009::printHello()
{
    std::cout << "hellooooooooooooooooooo" <<std::endl;
}


/**
 * @brief adrv9009::reload_button_clicked
 */
void adrv9009::reload_button_clicked()
{
    update_widgets();
    //profile_update();
    //    rx_freq_info_update();
    glb_settings_update_labels();
    //    rssi_update_labels();
    //    rx_phase_rotation_update();
}

#pragma endregion }

#pragma region Hopping {

void adrv9009::startHopping(bool status,double min,double max,double step,int delay)
{
    hopping=status;

    if(!hopping)
        return;

    hoppingFuture=QtConcurrent::run([=]{

        if(hopping)
        {
            double next=min;

            while(hopping){

                globals::hopping=true;

                ui->fhm_enable->setChecked(true);

                for(int i=0;i<4;i++)
                {
                    ui->tx_lo_freq_hop->setValue(next);
                }

                if(next<max)
                    next+=step;
                else
                    next=min;

                QThread::msleep(delay);

                globals::hopping=false;

                QThread::msleep(delay/3);

            }
        }

    });
}

void adrv9009::test(QString)
{
    int i;
}

#pragma endregion }

#pragma region Utility {

/**
 * @brief adrv9009::LogValue
 * @param value
 */
void adrv9009::LogValue(QString value){

    //    qInfo()<<QObject::sender()->objectName()<<" Changed. New Value: "<<value;
}

#pragma endregion }

#pragma region Other {

/**
 * @brief rx_phase_rotation_set
 * @param user_data
 */
void adrv9009::rx_phase_rotation_set(QDoubleSpinBox *spinbutton,gpointer user_data)
{
    uintptr_t offset = (uintptr_t) user_data;
    struct iio_channel *out0, *out1;
    gdouble val, phase;

    if (!cap)
        return;

    val = spinbutton->value();

    phase = val * 2 * M_PI / 360.0;

    if (offset == 2) {
        out0 = iio_device_find_channel(cap, "voltage1_i", false);
        out1 = iio_device_find_channel(cap, "voltage1_q", false);
    } else {
        out0 = iio_device_find_channel(cap, "voltage0_i", false);
        out1 = iio_device_find_channel(cap, "voltage0_q", false);
    }

    if (out1 && out0) {
        iio_channel_attr_write_double(out0, "calibscale", (double) cos(phase));
        iio_channel_attr_write_double(out0, "calibphase",
                                      (double) (-1 * sin(phase)));
        iio_channel_attr_write_double(out1, "calibscale", (double) cos(phase));
        iio_channel_attr_write_double(out1, "calibphase", (double) sin(phase));
    }


}

/**
 * @brief adrv9009::tx_sample_rate_changed
 * @param spinbutton
 */
void adrv9009::tx_sample_rate_changed(const int value)
{
    gdouble rate;

    rate = value / 2.0;
    dacDataManager dacManager;
    dacManager.dac_data_manager_freq_widgets_range_update(dac_tx_manager, rate);
}

/**
 * @brief adrv9009::int_dec_freq_update
 */
void adrv9009::int_dec_freq_update(void)
{
    struct iio_channel *ch;
    double freq;
    gchar *text;

    ch = iio_device_find_channel(cap, "voltage0_i", false);
    iio_channel_attr_read_double(ch, "sampling_frequency", &freq);

    text = g_strdup_printf ("%f", freq / mhz_scale);
    ui->label_sampling_freq_rx->setText(text);
    g_free(text);

    ch = iio_device_find_channel(dds, "voltage0", true);
    iio_channel_attr_read_double(ch, "sampling_frequency", &freq);

    text = g_strdup_printf ("%f", freq / mhz_scale);
    ui->label_sampling_freq_tx->setText(g_strdup_printf ("%f", freq / mhz_scale));
    g_free(text);
}

/**
 * @brief adrv9009::int_dec_update_cb
 */
void adrv9009::int_dec_update_cb()
{
    //    int_dec_freq_update();
    rx_freq_info_update();
}

/**
 * @brief adrv9009::get_gui_tx_sampling_freq
 * @return
 */
double adrv9009::get_gui_tx_sampling_freq()
{
    if(subcomponents[0].tx_widgets[subcomponents[0].tx_sample_freq].widget)
    {
        QString className=subcomponents[0].tx_widgets[subcomponents[0].tx_sample_freq].widget->metaObject()->className();
        if(className=="QDoubleSpinBox")
            return ((QDoubleSpinBox*)(subcomponents[0].tx_widgets[subcomponents[0].tx_sample_freq].widget))->value();
    }

    return 0;
}

/**
 * @brief adrv9009::rx_phase_rotation_update
 */
void adrv9009::rx_phase_rotation_update()
{
    struct iio_channel *out[4];
    gdouble val[4];
    int i, d = 0;

    if (!cap)
        return;

    out[0] = iio_device_find_channel(cap, "voltage0_i", false);
    out[1] = iio_device_find_channel(cap, "voltage0_q", false);

    if (is_2rx_2tx) {
        out[2] = iio_device_find_channel(cap, "voltage1_i", false);
        out[3] = iio_device_find_channel(cap, "voltage1_q", false);
        d = 2;
    }

    for (i = 0; i <= d; i += 2) {
        iio_channel_attr_read_double(out[i], "calibscale", &val[0]);
        iio_channel_attr_read_double(out[i], "calibphase", &val[1]);
        iio_channel_attr_read_double(out[i + 1], "calibscale", &val[2]);
        iio_channel_attr_read_double(out[i + 1], "calibphase", &val[3]);

        val[0] = acos(val[0]) * 360.0 / (2.0 * M_PI);
        val[1] = asin(-1.0 * val[1]) * 360.0 / (2.0 * M_PI);
        val[2] = acos(val[2]) * 360.0 / (2.0 * M_PI);
        val[3] = asin(val[3]) * 360.0 / (2.0 * M_PI);

        if (val[1] < 0.0)
            val[0] *= -1.0;
        if (val[3] < 0.0)
            val[2] *= -1.0;
        if (val[1] < -90.0)
            val[0] = (val[0] * -1.0) - 180.0;
        if (val[3] < -90.0)
            val[0] = (val[0] * -1.0) - 180.0;

        if (fabs(val[0]) > 90.0) {
            if (val[1] < 0.0)
                val[1] = (val[1] * -1.0) - 180.0;
            else
                val[1] = 180 - val[1];
        }
        if (fabs(val[2]) > 90.0) {
            if (val[3] < 0.0)
                val[3] = (val[3] * -1.0) - 180.0;
            else
                val[3] = 180 - val[3];
        }

        if (round(val[0]) != round(val[1]) &&
                round(val[0]) != round(val[2]) &&
                round(val[0]) != round(val[3])) {
            printf("error calculating phase rotations\n");
            val[0] = 0.0;
        } else
            val[0] = (val[0] + val[1] + val[2] + val[3]) / 4.0;

        ((QDoubleSpinBox*)(rx_phase_rotation[i/2]))->setValue(val[0]);
    }
}

#pragma endregion }

#pragma region Profile {


/**
 * @brief adrv9009::profile_update
 */
void adrv9009::profile_update(void)
{
    //    if (plugin_osc_running_state() == true) {
    //        plugin_osc_stop_capture();
    //        plugin_osc_start_capture();
    //    }

    glb_settings_update_labels();
    update_widgets();
    rx_freq_info_update();
}


static void trigger_advanced_plugin_reload(void)
{
    struct osc_plugin *plugin;
    GSList *node;

    //	for (node = plugin_list; node; node = g_slist_next(node)) {
    //		plugin = node->data;

    //		if (plugin && (!strncmp(plugin->name, "ADRV9009 Advanced", 17))) {
    //			if (plugin->handle_external_request) {
    //				plugin->handle_external_request(plugin, "RELOAD");
    //			}
    //		}
    //	}

}


int adrv9009::load_tal_profile(QString file_name)
{
    int ret = -ENOMEM;
    QString  path = "";
    QStringList ptr;
    FILE *f;

    progress->setValue(10);

    if (!file_name.contains("filters/"))
        path = OSC_FILTER_FILE_PATH+ file_name;
    else
        path = file_name;

    if (path=="")
        goto err_set_filename;

    ptr = file_name.split('/');

    file_name=ptr[ptr.length()-1];

    f = fopen(path.toLocal8Bit().data(), "r");

    progress->setValue(20);

    if (f)
    {
        char *buf;
        ssize_t len;
        int ret2;

        fseek(f, 0, SEEK_END);
        len = ftell(f);
        buf = (char*)malloc(len);
        fseek(f, 0, SEEK_SET);
        len = fread(buf, 1, len, f);
        fclose(f);

        iio_context_set_timeout(globals::ctx, 30000);

        ret = INT_MAX;
        guint i = 0;
        for (; i < phy_devs_count; i++) {
            ret2 = iio_device_attr_write_raw(subcomponents[i].iio_dev, "profile_config", buf, len);
            ret = (ret > ret2) ? ret2 : ret;
        }

        iio_context_set_timeout(globals::ctx, 3000);
        free(buf);
    }

    progress->setValue(40);

    if (ret < 0) {
        fprintf(stderr, "Profile config failed: %s\n", path.toLocal8Bit().data());

        QMessageBox *msg=new QMessageBox(this);
        msg->setWindowTitle("Profile Configuration Failed");
        msg->setText("\nFailed to load profile using the selected file.");
        msg->show();

        QThread::msleep(2000);

        msg->close();

    } else {
        if (last_profile!="")
            path=last_profile;
    }
    progress->setValue(60);

    profile_update();

    printf("Profile loaded: %s (ret = %i)\n", path.toLocal8Bit().data(), ret);

    if (ret >= 0)
        ui->profile_config->setText(file_name);
    progress->setValue(80);

    setting.setLastFilter(path);
    setting.SaveToFile();

    //    g_free(path.toLocal8Bit().data());
    trigger_advanced_plugin_reload();

err_set_filename:

    if (ret < 0) {
        if (last_profile!="")
            ui->profile_config->setText(last_profile);
        else
            ui->profile_config->setText("(Choose File)");
    }
    progress->setValue(100);
    progress->close();
    return ret;
}


#pragma endregion }

//void adrv9009::on_profile_config_clicked()
//{

//    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open File"),
//                                                    QDir::homePath(),
//                                                    tr("All Files (*)"),
//                                                    nullptr,
//                                                    QFileDialog::DontUseNativeDialog);


//    if(fileName!="")
//    {
//        if(progress==nullptr)
//            progress=new QProgressDialog();

//        QtConcurrent::run([=](){
//            load_tal_profile(fileName);
//        });
//        progress->exec();
//    }

//    QDir::setCurrent(fileName.remove(QUrl(fileName).fileName()));
//}

void adrv9009::on_profile_config_clicked(QString fileName)
{
    fileName = fileName.trimmed();
    if (fileName.isEmpty())
    {
        QMessageBox::warning(this, tr("Profile"), tr("Profile file was not specified."));
        return;
    }

    /* load_tal_profile() updates Qt widgets and the progress dialog.  It must
     * therefore run on the GUI thread.  The previous QtConcurrent::run()
     * implementation updated QWidget objects from a worker thread, which is
     * undefined behavior and can terminate the process with SIGABRT/SIGSEGV. */
    QString resolvedPath = fileName;
    if (!QFileInfo::exists(resolvedPath))
    {
        QString fallback = QDir(QString::fromLocal8Bit(OSC_FILTER_FILE_PATH)).filePath(fileName);
        if (QFileInfo::exists(fallback))
            resolvedPath = fallback;
    }

    if (!QFileInfo::exists(resolvedPath))
    {
        qWarning() << "ADRV9009 profile file not found:" << fileName;
        return;
    }

    if (progress == nullptr)
        progress = new QProgressDialog(this);

    progress->setWindowModality(Qt::ApplicationModal);
    progress->setAutoClose(false);
    progress->show();
    QApplication::processEvents();

    load_tal_profile(resolvedPath);

    const QFileInfo info(resolvedPath);
    if (info.exists())
        QDir::setCurrent(info.absolutePath());
}



QString adrv9009::setFile(QString fileName,double scale)
{
        changingDac("");
        // Put every TX pair into "DAC Buffer Output" mode, not just TX1,
// so the loaded waveform drives all four channels voltage0..3.
for (guint i = 0; i < dac_tx_manager->dac1.tx_count; i++)
    dac_tx_manager->dac1.txs[i].dds_mode_widget->setCurrentIndex(4);
for (guint i = 0; i < dac_tx_manager->dac2.tx_count; i++)
    dac_tx_manager->dac2.txs[i].dds_mode_widget->setCurrentIndex(4);
        dac_tx_manager->dac_buffer_module.scale->setValue(scale);
        QTreeWidget *treeview =dac_tx_manager->dac_buffer_module.tx_channels_view;
        for(int i=0;i<treeview->topLevelItemCount();i++)
            treeview->topLevelItem(i)->setCheckState(0,Qt::CheckState::Checked);

        dac_data_manager_instance->dac_buffer_config_file_set_cb(fileName,dac_tx_manager->dac_buffer_module);
        dac_data_manager_instance->waveform_load_button_clicked_cb(&dac_tx_manager->dac_buffer_module);
//        QString p =dac_tx_manager->dac_buffer_module.load_status_buf->toPlainText();
        return dac_tx_manager->dac_buffer_module.load_status_buf->toPlainText();

//    return "";
}

/**
 * @brief dacDataManager::changingDac
 */
QString adrv9009::changingDac(QString mode)
{
    if(mode == "set-cw" || mode == "set-sweep" )
    {
        dac_tx_manager->dac1.txs[0].dds_mode_widget->setCurrentIndex(1);
        dac_tx_manager->dac1.txs[1].dds_mode_widget->setCurrentIndex(1);

        return mode == "set-cw" ? "CW is set successfully": "...";
    }
//    if(mode == "disable-cw")
//    {
//        dac_tx_manager->dac1.txs[0].dds_mode_widget->setCurrentIndex(0);
//        dac_tx_manager->dac1.txs[1].dds_mode_widget->setCurrentIndex(0);
//    }
    else
    {
        dac_tx_manager->dac1.txs[0].dds_mode_widget->setCurrentIndex(0);
        dac_tx_manager->dac1.txs[1].dds_mode_widget->setCurrentIndex(0);
    }
        return mode +" is turned off.";
}
