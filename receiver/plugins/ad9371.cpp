#include "ad9371.h"
#include <receiver/osc.h>

#define HANNING_ENBW 1.50

#define THIS_DRIVER "AD9371"
#define PHY_DEVICE "ad9371-phy"
#define DDS_DEVICE "axi-ad9371-tx-hpc"
#define CAP_DEVICE "axi-ad9371-rx-hpc"
#define CAP_DEVICE_2 "axi-ad9371-rx-obs-hpc"
#define UDC_RX_DEVICE "adf4351-udc-rx-pmod"
#define UDC_TX_DEVICE "adf4351-udc-tx-pmod"

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
static struct iio_widget *glb_widgets, *tx_widgets, *rx_widgets, *obsrx_widgets, *fpga_widgets;
static unsigned int rx1_gain, rx2_gain, obs_gain, tx1_clgc_desired_gain, tx2_clgc_desired_gain;
static unsigned int num_glb, num_tx, num_rx, num_obsrx, num_fpga;
static unsigned int rx_lo, tx_lo, sn_lo;
static unsigned int rx_sample_freq, tx_sample_freq;
static unsigned int tx1_dpd, tx2_dpd, tx1_clgc, tx2_clgc, tx1_vswr, tx2_vswr;;
static double updn_freq_span;
static double updn_freq_mix_sign;
static char last_profile[PATH_MAX];

    static struct iio_device *dds, *cap, *udc_rx, *udc_tx, *cap_obs;

    enum {
        SECTION_GLOBAL,
        SECTION_TX,
        SECTION_RX,
        SECTION_OBS,
        SECTION_FPGA,
        SECTION_NUM,
        };

    static QAction *section_toggle[SECTION_NUM];
static QWidget *section_setting[SECTION_NUM];

    static gint this_page;
static gboolean plugin_detached;

    const char *clgc_status_strings[] = {
        "No Error",
        "Error: TX is disabled",
        "Error: ORx is disabled",
        "Error: Loopback switch is closed",
        "Error: Data measurement aborted during capture",
        "Error: No initial calibration was done",
        "Error: Path delay not setup",
        "Error: No apply control is possible",
        "Error: ontrol value is out of range",
        "Error: CLGC feature is disabled",
        "Error: TX attenuation is capped",
        "Error: Gain measurement",
        "Error: No GPIO configured in single ORx configuration",
        "Error: Tx is not observable with any of the ORx Channels",
        };

    const char *dpd_status_strings[] = {
        "No Error",
        "Error: ORx disabled",
        "Error: Tx disabled",
        "Error: DPD initialization not run",
        "Error: Path delay not setup",
        "Error: ORx signal too low",
        "Error: ORx signal saturated",
        "Error: Tx signal too low",
        "Error: Tx signal saturated",
        "Error: Model error high",
        "Error: AM AM outliers",
        "Error: Invalid Tx profile",
        "Error: ORx QEC Disabled",
        };

    const char *vswr_status_strings[] = {
        "No Error",
        "Error: TX disabled",
        "Error: ORx disabled",
        "Error: Loopback switch is closed",
        "Error: No initial calibration was done",
        "Error: Path delay not setup",
        "Error: Data capture aborted",
        "Error: VSWR is disabled",
        "Error: Entering Cal",
        "Error: No GPIO configured in single ORx configuration",
        "Error: Tx is not observable with any of the ORx Channels",
        };

    static const char *ad9371_sr_attribs[] = {

        PHY_DEVICE".ensm_mode",
        PHY_DEVICE".calibrate_dpd_en",
        PHY_DEVICE".calibrate_clgc_en",
        PHY_DEVICE".calibrate_rx_qec_en",
        PHY_DEVICE".calibrate_tx_qec_en",
        PHY_DEVICE".calibrate_tx_lol_en",
        PHY_DEVICE".calibrate_tx_lol_ext_en",
        PHY_DEVICE".calibrate_vswr_en",
        PHY_DEVICE".in_voltage0_gain_control_mode",
        PHY_DEVICE".in_voltage0_hardwaregain",
        PHY_DEVICE".in_voltage0_quadrature_tracking_en",
        PHY_DEVICE".in_voltage0_temp_comp_gain",
        PHY_DEVICE".in_voltage1_gain_control_mode",
        PHY_DEVICE".in_voltage1_hardwaregain",
        PHY_DEVICE".in_voltage1_temp_comp_gain",
        PHY_DEVICE".in_voltage1_quadrature_tracking_en",
        PHY_DEVICE".in_voltage2_hardwaregain",
        PHY_DEVICE".in_voltage2_rf_port_select",
        PHY_DEVICE".in_voltage2_temp_comp_gain",
        PHY_DEVICE".in_voltage_rf_port_select_available",
        PHY_DEVICE".out_altvoltage0_RX_LO_frequency",
        PHY_DEVICE".out_altvoltage1_TX_LO_frequency",
        PHY_DEVICE".out_altvoltage2_RX_SN_LO_frequency",
        PHY_DEVICE".out_voltage0_hardwaregain",
        PHY_DEVICE".out_voltage0_lo_leakage_tracking_en",
        PHY_DEVICE".out_voltage0_quadrature_tracking_en",
        PHY_DEVICE".out_voltage1_hardwaregain",
        PHY_DEVICE".out_voltage1_lo_leakage_tracking_en",
        PHY_DEVICE".out_voltage1_quadrature_tracking_en",

        DDS_DEVICE".out_altvoltage0_TX1_I_F1_frequency",
        DDS_DEVICE".out_altvoltage0_TX1_I_F1_phase",
        DDS_DEVICE".out_altvoltage0_TX1_I_F1_raw",
        DDS_DEVICE".out_altvoltage0_TX1_I_F1_scale",
        DDS_DEVICE".out_altvoltage1_TX1_I_F2_frequency",
        DDS_DEVICE".out_altvoltage1_TX1_I_F2_phase",
        DDS_DEVICE".out_altvoltage1_TX1_I_F2_raw",
        DDS_DEVICE".out_altvoltage1_TX1_I_F2_scale",
        DDS_DEVICE".out_altvoltage2_TX1_Q_F1_frequency",
        DDS_DEVICE".out_altvoltage2_TX1_Q_F1_phase",
        DDS_DEVICE".out_altvoltage2_TX1_Q_F1_raw",
        DDS_DEVICE".out_altvoltage2_TX1_Q_F1_scale",
        DDS_DEVICE".out_altvoltage3_TX1_Q_F2_frequency",
        DDS_DEVICE".out_altvoltage3_TX1_Q_F2_phase",
        DDS_DEVICE".out_altvoltage3_TX1_Q_F2_raw",
        DDS_DEVICE".out_altvoltage3_TX1_Q_F2_scale",
        DDS_DEVICE".out_altvoltage4_TX2_I_F1_frequency",
        DDS_DEVICE".out_altvoltage4_TX2_I_F1_phase",
        DDS_DEVICE".out_altvoltage4_TX2_I_F1_raw",
        DDS_DEVICE".out_altvoltage4_TX2_I_F1_scale",
        DDS_DEVICE".out_altvoltage5_TX2_I_F2_frequency",
        DDS_DEVICE".out_altvoltage5_TX2_I_F2_phase",
        DDS_DEVICE".out_altvoltage5_TX2_I_F2_raw",
        DDS_DEVICE".out_altvoltage5_TX2_I_F2_scale",
        DDS_DEVICE".out_altvoltage6_TX2_Q_F1_frequency",
        DDS_DEVICE".out_altvoltage6_TX2_Q_F1_phase",
        DDS_DEVICE".out_altvoltage6_TX2_Q_F1_raw",
        DDS_DEVICE".out_altvoltage6_TX2_Q_F1_scale",
        DDS_DEVICE".out_altvoltage7_TX2_Q_F2_frequency",
        DDS_DEVICE".out_altvoltage7_TX2_Q_F2_phase",
        DDS_DEVICE".out_altvoltage7_TX2_Q_F2_raw",
        DDS_DEVICE".out_altvoltage7_TX2_Q_F2_scale",
        };

    static const char * ad9371_driver_attribs[] = {
        "load_myk_profile_file",
        "dds_mode_tx1",
        "dds_mode_tx2",
        "global_settings_show",
        "tx_show",
        "rx_show",
        "fpga_show",
        "tx_channel_0",
        "tx_channel_1",
        "tx_channel_2",
        "tx_channel_3",
        "dac_buf_filename",
        };

dacDataManager *dac_data_manager_instance_9371;


AD9371::AD9371(QApplication *app,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AD9371)
{
    ui->setupUi(this);
}

AD9371::~AD9371()
{
    delete ui;

    refreshThread->quit();
}

/**
 * @brief update_widgets
 */
static void update_widgets(void) {

    IIO_Widget iio_w;

    iio_w.iio_update_widgets_of_device(widgets, num_glb + num_tx + num_rx + num_obsrx,
                                       globals::dev);
    if (dds)
        iio_w.iio_update_widgets_of_device(widgets,
                                           num_glb + num_tx + num_rx + num_obsrx, dds);

    dacDataManager dacManager;
    dacManager.dac_data_manager_update_iio_widgets(dac_tx_manager);

    iio_w.iio_update_widgets(fpga_widgets, num_fpga);
}

/**
 * @brief AD9371::rx_freq_info_update
 */
void AD9371::rx_freq_info_update(void) {
    double lo_freq;

    osc oscInstance;
    if (cap) {
        oscInstance.rx_update_device_sampling_freq(CAP_DEVICE,
                                                    USE_INTERN_SAMPLING_FREQ);
        lo_freq = mhz_scale
                  * ((QDoubleSpinBox*)(rx_widgets[rx_lo].widget))->value();

        lo_freq=0;
        oscInstance.rx_update_channel_lo_freq(CAP_DEVICE, "all", lo_freq);

    }

    if (cap_obs) {
        oscInstance.rx_update_device_sampling_freq(CAP_DEVICE_2,
                                                    USE_INTERN_SAMPLING_FREQ);

        // Keep the QString alive. toLocal8Bit().data() dangles after the
        // statement and strstr() is a heap-use-after-free under ASan.
        const QString source = static_cast<QComboBox*>(ui->rf_port_select_obs)->currentText();

        if (source.contains(QLatin1String("TX"))) {
            lo_freq = mhz_scale
                      * ((QDoubleSpinBox*)(tx_widgets[tx_lo].widget))->value();
        } else {
            lo_freq = mhz_scale
                      * ((QDoubleSpinBox*)(obsrx_widgets[sn_lo].widget))->value();
        }

        lo_freq=0;
        oscInstance.rx_update_channel_lo_freq(CAP_DEVICE_2, "all", lo_freq);
        //g_free(source);
    }
}

/**
 * @brief rssi_update_label
 * @param label
 * @param chn
 * @param is_tx
 */
static void rssi_update_label(QWidget *label, const char *chn, bool is_tx) {
    char buf[1024];
    int ret;

    /* don't update if it is hidden (to quiet down SPI) */
    if (!(label->isVisible()))
        return;

    ret = iio_channel_attr_read(iio_device_find_channel(globals::dev, chn, is_tx),
                                "rssi", buf, sizeof(buf));
    if (ret > 0)
        ((QLabel*)(label))->setText(buf);
    else
        ((QLabel*)(label))->setText("<error>");
}

/**
 * @brief AD9371::rssi_update_labels
 */
void AD9371::rssi_update_labels(void) {
    rssi_update_label(ui->rssi_rx1, "voltage0", false);

    if (is_2rx_2tx) {
        rssi_update_label(ui->rssi_rx2, "voltage1", false);
    }

    rssi_update_label(ui->rssi_obs, "voltage2", false);
}

/**
 * @brief AD9371::save_widget_value
 * @param iio_w
 */
void AD9371::save_widget_value(struct iio_widget *iio_w) {

    //    glb_widgets[0].value=ui->ensm_mode_available->currentText().toLatin1().data();
    //    save_widget_value(&glb_widgets[0]);

    iio_w->save(iio_w);
}

/**
 * @brief AD9371::LogValue
 * @param value
 */
void AD9371::LogValue(QString value){

    qInfo()<<QObject::sender()->objectName()<<" Changed. New Value: "<<value;
}

/**
 * @brief AD9371::make_widget_update_signal_based
 * @param widgets
 * @param num_widgets
 */
void AD9371::make_widget_update_signal_based(struct iio_widget *widgets,
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

/**
 * @brief AD9371::refreshTimerStarted
 */
void AD9371::refreshTimerStarted()
{
    qInfo()<<"refreshTimer started on "<<refreshTimer.thread();
}

/**
 * @brief rx_phase_rotation_set
 * @param user_data 
 */
void AD9371::rx_phase_rotation_set(QDoubleSpinBox *spinbutton,gpointer user_data)
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
 * @brief AD9371::reload_button_clicked
 */
void AD9371::reload_button_clicked()
{
    update_widgets();
    //profile_update();
    rx_freq_info_update();
    glb_settings_update_labels();
    rssi_update_labels();
    rx_phase_rotation_update();
}

/**
 * @brief Initialize all widgets for AD9371
 * @return
 */
QWidget *AD9371::init()
{

    struct iio_channel *ch0, *ch1, *ch2;

    can_update_widgets = false;

    if (!globals::ctx)
        return NULL;

    globals::dev = iio_context_find_device(globals::ctx, PHY_DEVICE);
    if (!globals::dev) {
        fprintf(stderr, "Failed to find phy device %s\n", PHY_DEVICE);
        globals::osc_destroy_context();
        return NULL;
    }

    dds = iio_context_find_device(globals::ctx, DDS_DEVICE);
    if (!dds) {
        fprintf(stderr, "Failed to find dds device %s\n", DDS_DEVICE);
        globals::osc_destroy_context();
        return NULL;
    }


    cap = iio_context_find_device(globals::ctx, CAP_DEVICE);
    if (!cap) {
        fprintf(stderr, "Failed to find cap device %s\n", CAP_DEVICE);
        globals::osc_destroy_context();
        return NULL;
    }

    cap_obs = iio_context_find_device(globals::ctx, CAP_DEVICE_2);
    if (!cap_obs) {
        fprintf(stderr, "Failed to find cap obs device %s\n", CAP_DEVICE_2);
        globals::osc_destroy_context();
        return NULL;
    }

    udc_rx = iio_context_find_device(globals::ctx, UDC_RX_DEVICE);
    udc_tx = iio_context_find_device(globals::ctx, UDC_TX_DEVICE);

    has_udc_driver = (udc_rx && udc_tx);

    ch0 = iio_device_find_channel(globals::dev, "voltage0", false); /* RX1 */
    ch1 = iio_device_find_channel(globals::dev, "voltage1", false); /* RX2 */
    ch2 = iio_device_find_channel(globals::dev, "voltage2", false); /* OBS-RX */

    dac_tx_manager = dac_data_manager_instance_9371->dac_data_manager_new(dds, NULL, globals::ctx);
    dac_data_manager_instance_9371->dac_data_manager_set_buffer_size_alignment(dac_tx_manager, 16);

    const char *env_freq_span = getenv("OSC_UPDN_FREQ_SPAN");
    const char *env_freq_mix_sign = getenv("OSC_UPDN_FREQ_MIX_SIGN");

    if (!env_freq_span) {
        updn_freq_span = 2;
    } else {
        errno = 0;
        updn_freq_span = g_strtod(env_freq_span, NULL);
        if (errno)
            updn_freq_span = 2;
    }

    if (!env_freq_mix_sign) {
        updn_freq_mix_sign = 1;
    } else {
        if (!strncmp(env_freq_mix_sign, "-", 1))
            updn_freq_mix_sign = -1;
        else
            updn_freq_mix_sign = 1;
    }

    is_2rx_2tx = ch1 && iio_channel_find_attr(ch1, "hardwaregain");

    //----------------------------------------------------------------

    /* Transmit Chain */

    dacDataManager *dacManager=new dacDataManager();
    if (dac_tx_manager)
        ui->dds_container_group->setLayout(dacManager->dac_data_manager_get_gui_container(dac_tx_manager));


    rx_phase_rotation[0] = ui->rx1_phase_rotation;
    rx_phase_rotation[1] = ui->rx2_phase_rotation;

    //----------------------------------------------------------------

    glb_widgets = widgets;

    IIO_Widget iio_w;

    /* Global settings */

    iio_w.iio_combo_box_init(&glb_widgets[num_glb++], globals::dev, NULL, "ensm_mode",
                             "ensm_mode_available", ui->ensm_mode_available, NULL);

    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //                                 "calibrate_dpd_en", ui->calibrate_dpd_en, 0);
    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //                                 "calibrate_clgc_en", ui->calibrate_clgc_en, 0);
    //    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], dev, NULL,
    //                                 "calibrate_vswr_en", ui->calibrate_vswr_en, 0);

    num_glb+=3;

    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], globals::dev, NULL,
                                 "calibrate_rx_qec_en", ui->calibrate_rx_qec_en, 0);

    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], globals::dev, NULL,
                                 "calibrate_tx_qec_en", ui->calibrate_tx_qec_en, 0);

    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], globals::dev, NULL,
                                 "calibrate_tx_lol_en", ui->calibrate_tx_lol_en, 0);

    iio_w.iio_toggle_button_init(&glb_widgets[num_glb++], globals::dev, NULL,
                                 "calibrate_tx_lol_ext_en", ui->calibrate_tx_lol_ext_en, 0);

    iio_w.iio_button_init(&glb_widgets[num_glb++], globals::dev, NULL,
                          "calibrate", ui->calibrate);

    rx_widgets = &glb_widgets[num_glb];

    /* Receive Chain */

    iio_w.iio_combo_box_init(&rx_widgets[num_rx++], globals::dev, ch0, "gain_control_mode",
                             "gain_control_mode_available", ui->gain_control_mode_available_rx1, NULL);
    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], globals::dev, ch0,
                               "temp_comp_gain", ui->temp_comp_gain_rx1, NULL);
    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], globals::dev, ch1,
                               "temp_comp_gain", ui->temp_comp_gain_rx2, NULL);

    rx1_gain = num_rx;
    //    qInfo()<<"Num Rx -> "<<num_rx;

    iio_w.iio_spin_button_init(&rx_widgets[num_rx++], globals::dev, ch0,
                               "hardwaregain", ui->hardware_gain_rx1, NULL);

    if (is_2rx_2tx) {
        rx2_gain = num_rx;
        iio_w.iio_spin_button_init(&rx_widgets[num_rx++], globals::dev, ch1,
                                   "hardwaregain", ui->hardware_gain_rx2, NULL);
    }

    rx_sample_freq = num_rx;

    //    num_rx++;
    //        iio_w.iio_spin_button_init(&rx_widgets[num_rx++], dev, ch0,
    //                "sampling_frequency", ui->sampling_freq_rx, &mhz_scale);
    //    iio_w.iio_spin_button_add_progress(&rx_widgets[num_rx - 1]);

    iio_w.iio_toggle_button_init(&rx_widgets[num_rx++], globals::dev, ch0,
                                 "quadrature_tracking_en", ui->rx1_quadrature_tracking_en, 0);

    iio_w.iio_toggle_button_init(&rx_widgets[num_rx++], globals::dev, ch1,
                                 "quadrature_tracking_en", ui->rx2_quadrature_tracking_en, 0);

    rx_lo = num_rx;

    ch1 = iio_device_find_channel(globals::dev, "altvoltage0", true);
    if (iio_channel_find_attr(ch1, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "RX_LO_frequency";

    iio_w.iio_spin_button_s64_init(&rx_widgets[num_rx++], globals::dev, ch1,
                                   freq_name, ui->rx_lo_freq, &mhz_scale);
    iio_w.iio_spin_button_add_progress(&rx_widgets[num_rx - 1]);


    /* Observation Receiver Chain */

    obsrx_widgets = &rx_widgets[num_rx];

    iio_w.iio_combo_box_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2,
                             "gain_control_mode", "gain_control_mode_available",
                             ui->cmb_gain_control_mode_obs, NULL);

    iio_w.iio_combo_box_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2, "rf_port_select",
                             "rf_port_select_available", ui->rf_port_select_obs, NULL);

    iio_w.iio_spin_button_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2,
                               "temp_comp_gain", ui->temp_comp_gain_obs, NULL);

    obs_gain = num_obsrx;
    iio_w.iio_spin_button_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2,
                               "hardwaregain", ui->hardware_gain_obs, NULL);

    iio_w.iio_toggle_button_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2,
                                 "quadrature_tracking_en", ui->obs_quadrature_tracking_en, 0);

    sn_lo = num_obsrx;
    ch2 = iio_device_find_channel(globals::dev, "altvoltage2", true);
    if (iio_channel_find_attr(ch2, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "RX_SN_LO_frequency";

    iio_w.iio_spin_button_s64_init(&obsrx_widgets[num_obsrx++], globals::dev, ch2,
                                   freq_name, ui->sn_lo_freq, &mhz_scale);
    iio_w.iio_spin_button_add_progress(&obsrx_widgets[num_obsrx - 1]);

    //    iio_w.iio_toggle_button_init(&obsrx_widgets[num_obsrx++],
    //     		dev, ch2, "external", builder,
    //     		"obs_lo_external", 0);

    /* Transmit Chain */

    tx_widgets = &obsrx_widgets[num_obsrx];

    ch0 = iio_device_find_channel(globals::dev, "voltage0", true);
    if (is_2rx_2tx)
        ch1 = iio_device_find_channel(globals::dev, "voltage1", true);

    iio_w.iio_spin_button_init(&tx_widgets[num_tx++], globals::dev, ch0,
                               "hardwaregain", ui->hardware_gain_tx1, &inv_scale);

    if (is_2rx_2tx)
        iio_w.iio_spin_button_init(&tx_widgets[num_tx++], globals::dev, ch1,
                                   "hardwaregain", ui->hardware_gain_tx2, &inv_scale);
    tx_sample_freq = num_tx;

    num_tx++;
    //        iio_w.iio_spin_button_int_init(&tx_widgets[num_tx++], dev, ch0,
    //                "sampling_frequency", ui->sampling_freq_tx, &mhz_scale);
    //    iio_w.iio_spin_button_add_progress(&tx_widgets[num_tx - 1]);

    iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], globals::dev, ch0,
                                 "quadrature_tracking_en", ui->tx1_quadrature_tracking_en, 0);

    iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], globals::dev, ch0,
                                 "lo_leakage_tracking_en", ui->tx1_lo_leakage_tracking_en, 0);

    if (is_2rx_2tx) {
        iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], globals::dev, ch1,
                                     "quadrature_tracking_en", ui->tx2_quadrature_tracking_en,
                                     0);

        iio_w.iio_toggle_button_init(&tx_widgets[num_tx++], globals::dev, ch1,
                                     "lo_leakage_tracking_en", ui->tx2_lo_leakage_tracking_en,
                                     0);
    }

    has_dpd = !!iio_channel_find_attr(ch0, "dpd_tracking_en");

    if (has_dpd) {
        /* DPD */
        // TODO: First Build UI
    }
    else {
        // ui->frame_dpd.hide();
        // ui->frame_clgc.hide();
        // ui->frame_vswr.hide();
        //        ui->calibrate_dpd_en->setVisible(false);
        //        ui->calibrate_clgc_en->setVisible(false);
        //        ui->calibrate_vswr_en->setVisible(false);
    }

    tx_lo = num_tx;
    ch1 = iio_device_find_channel(globals::dev, "altvoltage1", true);

    if (iio_channel_find_attr(ch1, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "TX_LO_frequency";

    iio_w.iio_spin_button_s64_init(&tx_widgets[num_tx++], globals::dev, ch1,
                                   freq_name, ui->tx_lo_freq, &mhz_scale);

    iio_w.iio_spin_button_add_progress(&tx_widgets[num_tx - 1]);

    /* FPGA widgets */
    fpga_widgets = &tx_widgets[num_tx];

    ch0 = iio_device_find_channel(dds, "voltage0", true);
    if (iio_channel_find_attr(ch0, "sampling_frequency_available")) {
        iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++], dds, ch0,
                                 "sampling_frequency", "sampling_frequency_available",
                                 ui->fpga_tx_frequency_available, NULL);
    } else {
        // TODO: Create UI if need
        //ui->transmit_frame_dma_buf.setVisible(false);
    }

    ch0 = iio_device_find_channel(cap, "voltage0_i", false);
    if (iio_channel_find_attr(ch0, "sampling_frequency_available")) {
        iio_w.iio_combo_box_init(&fpga_widgets[num_fpga++], cap, ch0,
                                 "sampling_frequency", "sampling_frequency_available",
                                 ui->fpga_rx_frequency_available, NULL);
    } else {
        // TODO: Create UI if need
        //ui->receive_frame_dma_buf.setVisible(false);
    }

    /* Update all widgets with current values */
    printf("Updating widgets...\n");
    update_widgets();

    rx_freq_info_update();
    printf("Updating FIR filter...\n");

    glb_settings_update_labels();
    rssi_update_labels();

    /* Connect signals */


    QObject::connect(ui->rx1_phase_rotation,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [=](){
                         rx_phase_rotation_set(ui->rx1_phase_rotation,(gpointer *)0);
                     });

    QObject::connect(ui->rx2_phase_rotation,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [=](){
                         rx_phase_rotation_set(ui->rx2_phase_rotation,(gpointer *)2);
                     });

    QObject::connect(ui->tx_lo_freq,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [=](){
                         tx_sample_rate_changed(ui->tx_lo_freq);
                     });

    QObject::connect(ui->fpga_rx_frequency_available,
                     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [=](){
                         int_dec_update_cb();
                     });

    QObject::connect(ui->cmb_gain_control_mode_obs,
                     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [=](){
                         glb_settings_update_labels();
                     });

    QObject::connect(ui->gain_control_mode_available_rx1,
                     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [=](){
                         glb_settings_update_labels();
                     });


    QObject::connect(ui->fpga_tx_frequency_available,
                     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [=](){
                         int_dec_update_cb();
                     });

    QObject::connect(ui->ensm_mode_available,
                     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [=](){
                         glb_settings_update_labels();
                     });

    make_widget_update_signal_based(glb_widgets, num_glb);
    make_widget_update_signal_based(rx_widgets, num_rx);
    make_widget_update_signal_based(obsrx_widgets, num_obsrx);
    make_widget_update_signal_based(tx_widgets, num_tx);
    make_widget_update_signal_based(fpga_widgets, num_fpga);

    QTimer* timer1 = new QTimer();

    timer1->setInterval(interval);

    QObject::connect(timer1, &QTimer::timeout, [=]()
                     {
//                         glb_settings_update_labels();
//                         rssi_update_labels();

//                         int_dec_update_cb();
//                         dacManager->dac_data_manager_freq_widgets_range_update(dac_tx_manager,
//                                                                                get_gui_tx_sampling_freq() / 2.0);
//                         dacManager->dac_data_manager_update_iio_widgets(dac_tx_manager);
                     });

    timer1->start();

    return NULL;
}


void AD9371::tx_sample_rate_changed(QDoubleSpinBox *spinbutton)
{
    gdouble rate;

    rate = spinbutton->value() / 2.0;
    dacDataManager dacManager;
    dacManager.dac_data_manager_freq_widgets_range_update(dac_tx_manager, rate);
}


void AD9371::int_dec_freq_update(void)
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


void AD9371::int_dec_update_cb()
{
    int_dec_freq_update();
    rx_freq_info_update();
}


double AD9371::get_gui_tx_sampling_freq()
{
    if(tx_widgets[tx_sample_freq].widget)
    {
        QString className=tx_widgets[tx_sample_freq].widget->metaObject()->className();
        if(className=="QDoubleSpinBox")
            return ((QDoubleSpinBox*)(tx_widgets[tx_sample_freq].widget))->value();

        qInfo()<<"get_gui_tx_sampling_freq -> "<<className;
    }

    return 0;
}

/**
 * @brief update_lable_from
 * @param label
 * @param channel
 * @param attribute
 * @param output
 * @param unit
 * @param scale
 */
static void update_lable_from(QLabel *label, const char *channel,
                              const char *attribute, bool output, const char *unit, int scale) {
    char buf[80];
    long long val = 0;

    int ret = iio_channel_attr_read_longlong(
        iio_device_find_channel(globals::dev, channel, output), attribute, &val);

    if (scale == 1)
        snprintf(buf, sizeof(buf), "%lld %s", val, unit);
    else if (scale > 0 && scale <= 10)
        snprintf(buf, sizeof(buf), "%.1f %s", (float) val / scale, unit);
    else if (scale > 10)
        snprintf(buf, sizeof(buf), "%.2f %s", (float) val / scale, unit);
    else if (scale > 100)
        snprintf(buf, sizeof(buf), "%.3f %s", (float) val / scale, unit);

    if (ret >= 0)
        label->setText(buf);
    else
        label->setText("<error>");

}
/**
 * @brief AD9371::profile_update_labels
 */
void AD9371::profile_update_labels(void) {

    update_lable_from(ui->label_rf_bandwidth_rx, "voltage0", "rf_bandwidth", false,
                      "MHz", 1000000);
    update_lable_from(ui->label_rf_bandwidth_obs, "voltage2", "rf_bandwidth", false,
                      "MHz", 1000000);
    update_lable_from(ui->label_rf_bandwidth_tx, "voltage0", "rf_bandwidth", true,
                      "MHz", 1000000);

    update_lable_from(ui->label_sampling_freq_rx, "voltage0", "sampling_frequency",
                      false, "MSPS", 1000000);
    update_lable_from(ui->label_sampling_freq_obs, "voltage2", "sampling_frequency",
                      false, "MSPS", 1000000);
    update_lable_from(ui->label_sampling_freq_tx, "voltage0", "sampling_frequency",
                      true, "MSPS", 1000000);

}

/**
 * @brief AD9371::glb_settings_update_labels
 */
void AD9371::glb_settings_update_labels()
{

    char buf[1024];
    ssize_t ret;

    ret = iio_device_attr_read(globals::dev, "ensm_mode", buf, sizeof(buf));

    if (ret > 0)
        ui->ensm_mode->setText(buf);
    else
        ui->ensm_mode->setText("<error>");

    ret = iio_channel_attr_read(iio_device_find_channel(globals::dev, "voltage0", false),
                                "gain_control_mode", buf, sizeof(buf));

    if (ret > 0)
        ui->gain_control_mode_rx1->setText(buf);
    else
        ui->gain_control_mode_rx1->setText("<error>");

    if (is_2rx_2tx) {
        ret = iio_channel_attr_read(
                    iio_device_find_channel(globals::dev, "voltage1", false),
            "gain_control_mode", buf, sizeof(buf));
        if (ret > 0)
            ui->gain_control_mode_rx2->setText(buf);
        else
            ui->gain_control_mode_rx2->setText("<error>");
    }

    ret = iio_channel_attr_read(iio_device_find_channel(globals::dev, "voltage2", false),
                                "gain_control_mode", buf, sizeof(buf));
    if (ret > 0)
        ui->lbl_gain_control_mode->setText(buf);
    else
        ui->lbl_gain_control_mode->setText("<error>");

    profile_update_labels();

    IIO_Widget iio_w;
    iio_w.iio_widget_update(&rx_widgets[rx1_gain]);

    if (is_2rx_2tx)
        iio_w.iio_widget_update(&rx_widgets[rx2_gain]);

    iio_w.iio_widget_update(&obsrx_widgets[obs_gain]);

}

void AD9371::rx_phase_rotation_update()
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


void AD9371::profile_update(void)
{
    //    if (plugin_osc_running_state() == true) {
    //        plugin_osc_stop_capture();
    //        plugin_osc_start_capture();
    //    }

    glb_settings_update_labels();
    update_widgets();
    rx_freq_info_update();
}

void AD9371::on_btnReloadSettings_clicked()
{
    update_widgets();
    profile_update();
    rx_freq_info_update();
    glb_settings_update_labels();
    rssi_update_labels();
    rx_phase_rotation_update();
}

