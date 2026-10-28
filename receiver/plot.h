// ============================================================================
// File: plot.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for plot.h.
// ============================================================================

#ifndef PLOT_H
#define PLOT_H

#include <QMainWindow>
#include <stdio.h>
#include <complex>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <matio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <QStyledItemDelegate>
#include <QToolButton>
#include <QTreeWidgetItem>
#include "fftw3.h"
#include <complex.h>
#include <iio.h>
#include <QDateTime>
#include <cstring>

#include "osc.h"
#include "datatypes.h"
#include "osc_plugin.h"
#include "math.h"
#include "iostream"

#include <glib.h>
#include <glib-object.h>
#include <receiver/oscpreferences.h>
#include <QWidget>
#include <receiver/datatypes.h>
#include <glib/gtypes.h>
#include <sys/types.h>
#include <QtConcurrent>
#include <receiver/clustring/detector.h>
#include <receiver/config.h>
#include "constants/project_constans.h"
//#include "receiver/utils/filedialog.h"
#include "components/GLChart/QGLchart.h"
#include "components/GLChart/UpdateData.h"
#include "constants/project_constans.h"


#include <QAbstractButton>
#include <QAbstractButton>
#include <QFuture>
#include <QItemDelegate>
#include <QList>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QThread>
#include <QList>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidgetItem>


G_BEGIN_DECLS
#define OSC_PLOT_TYPE              (osc_plot_get_type())
#define OSC_PLOT(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), OSC_PLOT_TYPE, OscPlot))
#define OSC_PLOT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), OSC_PLOT_TYPE, OscPlotClass))
#define IS_OSC_PLOT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), OSC_PLOT_TYPE))
#define IS_OSC_PLOT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), OSC_PLOT_TYPE))

/* Max 1 Meg (2^20) */
#define MAX_SAMPLES 1048576
#define TMP_INI_FILE "/tmp/.%s.tmp"
#ifndef MAX_MARKERS
#define MAX_MARKERS 10
#endif


#define TIME_PLOT 0
#define FFT_PLOT 1
#define FFT_SEEK_PLOT 2
#define XY_PLOT 3
#define XCORR_PLOT 4
#define SPECTRUM_PLOT 5

#define OFF_MRK    "Markers Off"
#define PEAK_MRK   "Peak Markers"
#define FIX_MRK    "Fixed Markers"
#define SINGLE_MRK "Single Tone Markers"
#define DUAL_MRK   "Two Tone Markers"
#define IMAGE_MRK  "Image Markers"
#define ADD_MRK    "Add Marker"
#define REMOVE_MRK "Remove Marker"

/* Helpers */
#define TIME_SETTINGS(obj) ((struct _time_settings *)obj->settings)
#define FFT_SETTINGS(obj) ((struct _fft_settings *)obj->settings)
#define CONSTELLATION_SETTINGS(obj) ((struct _constellation_settings *)obj->settings)
#define XCORR_SETTINGS(obj) ((struct _cross_correlation_settings *)obj->settings)
#define FREQ_SPECTRUM_SETTINGS(obj) ((struct _freq_spectrum_settings *)obj->settings)
#define MATH_SETTINGS(obj) ((struct _math_settings *)obj->settings)

#define PLOT_CHN(obj) ((PlotChn *)obj)
#define PLOT_IIO_CHN(obj) ((PlotIioChn *)obj)
#define PLOT_MATH_CHN(obj) ((PlotMathChn *)obj)

#define THIS_DRIVER "adrv9009"
#define PHY_DEVICE "adrv9009-phy"
#define DDS_DEVICE "axi-adrv9009-tx-hpc"
#define CAP_DEVICE "axi-adrv9009-rx-hpc"
#define CAP_DEVICE_2 "axi-adrv9009-rx-obs-hpc"

typedef struct _OscPlot            OscPlot;
typedef struct _OscPlotPrivate     OscPlotPrivate;
typedef struct _OscPlotClass       OscPlotClass;

struct  _OscPlot
{
    QObject widget;
    OscPlotPrivate *priv;
};

struct _OscPlotClass
{
    QWidget parent_class;

    // Executes void routine.
    void (* capture_event) (OscPlot *plot, gboolean start_event);
    // Executes void routine.
    void (* destroy_event) (OscPlot *plot);
    // Executes void routine.
    void (* newplot_event) (OscPlot *plot, OscPlot *new_plot);
};

// Executes osc plot get type routine.
GType osc_plot_get_type (void);

G_END_DECLS

/* add backwards compat for <matio-1.5.0 */
#if MATIO_MAJOR_VERSION == 1 && MATIO_MINOR_VERSION < 5
typedef int mat_dim;
#else
typedef size_t mat_dim;
#endif

/* timersub, macros are _BSD_SOURCE, and aren't included in windows */
#ifndef timersub
#define timersub(a, b, result) \
    do { \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
    if ((result)->tv_usec < 0) { \
    --(result)->tv_sec; \
    (result)->tv_usec += 1000000; \
    } \
    } while (0)
#endif /* timersub */

/* IDs of signals */
enum {
    CAPTURE_EVENT_SIGNAL,
    DESTROY_EVENT_SIGNAL,
    NEWPLOT_EVENT_SIGNAL,
    LAST_SIGNAL
};

/* Columns of the device treestore */
enum {
    ELEMENT_NAME,
    IS_DEVICE,
    IS_CHANNEL,
    CHANNEL_TYPE,
    DEVICE_SELECTABLE,
    DEVICE_ACTIVE,
    CHANNEL_ACTIVE,
    ELEMENT_REFERENCE,
    EXPANDED,
    CHANNEL_SETTINGS,
    CHANNEL_COLOR_ICON,
    SENSITIVE,
    PLOT_TYPE,
    NUM_COL
};

/* Horizontal Scale Types */
enum {
    HOR_SCALE_SAMPLES,
    HOR_SCALE_TIME,
    HOR_SCALE_NUM_OPTIONS
};

/* Types of channels that can be displayed on a plot */
enum {
    PLOT_IIO_CHANNEL = 0,
    PLOT_MATH_CHANNEL,
    NUM_PLOT_CHANNELS_TYPES
};

#define MATH_CHANNELS_DEVICE "Math"

static QColor color_graph[] = {

    QColor(115, 210, 22),
    QColor(204, 0, 0),
    QColor(52, 101, 164),
    QColor(245, 121, 0),
    QColor(186, 189, 182),
    QColor(237, 212, 0),
    QColor(117, 80, 123),
    QColor(193, 125, 17),

    QColor(201,18,23,255),
    QColor(198,88,16,255),
    QColor(213,217,35,255),
    QColor(34,218,29,255),
    QColor(211, 215, 208),
    QColor(209,11,200,255),
    QColor(173, 127, 168),
    QColor(233, 185, 110),
};

#define NUM_GRAPH_COLORS (sizeof(color_graph) / sizeof(color_graph[0]))

static bool restart_capture = FALSE;

static int num_devices;

static QColor *color_grid = new QColor(0, 0, 0);

static QColor *color_background = new QColor(0,0,0);

static QColor *color_marker = new QColor( 255,255,255);

typedef struct channel_settings PlotChn;
typedef struct iio_channel_settings PlotIioChn;
typedef struct math_channel_settings PlotMathChn;

struct channel_settings {
    unsigned type;
    char *name;
    char *parent_name;
    struct iio_context *ctx;
    QColor graph_color;

    struct iio_device * (*get_iio_parent)(PlotChn *);
    QVector<double> * (*get_data_ref)(PlotChn *);
    // Executes void routine.
    void (*assert_used_iio_channels)(PlotChn *, bool);
    // Executes void routine.
    void (*destroy)(PlotChn *);
};

struct iio_channel_settings {
    PlotChn base;
    struct iio_channel *iio_chn;
    bool apply_inverse_funct;
    bool apply_multiply_funct;
    bool apply_add_funct;
    double multiply_value;
    double add_value;
};

struct math_channel_settings {
    PlotChn base;
    GSList *iio_channels;
    gfloat  ***iio_channels_data;
    int num_channels;
    char *iio_device_name;
    char *txt_math_expression;
    // Executes void routine.
    void (*math_expression)(float ***channels_data, float *out_data, unsigned long long chn_sample_cnt);
    void *math_lib_handler;
    float *data_ref;
};

struct ch_tr_params {
    QTreeWidgetItem *item;
    int enabled_channels;
    GSList *ch_settings;
};

struct ChannelItemData{
    PlotChn *chn;
    QString name;
    QString parent;
    bool isChecked;
};


/* Helpers */
#define TIME_SETTINGS(obj) ((struct _time_settings *)obj->settings)
#define FFT_SETTINGS(obj) ((struct _fft_settings *)obj->settings)
#define CONSTELLATION_SETTINGS(obj) ((struct _constellation_settings *)obj->settings)
#define XCORR_SETTINGS(obj) ((struct _cross_correlation_settings *)obj->settings)
#define FREQ_SPECTRUM_SETTINGS(obj) ((struct _freq_spectrum_settings *)obj->settings)
#define MATH_SETTINGS(obj) ((struct _math_settings *)obj->settings)

#define PLOT_CHN(obj) ((PlotChn *)obj)
#define PLOT_IIO_CHN(obj) ((PlotIioChn *)obj)
#define PLOT_MATH_CHN(obj) ((PlotMathChn *)obj)

#define SAVE_CSV 0
#define SAVE_MAT 1
#define SAVE_VSA 2
#define SAVE_PNG 3

static QString win="";
static int min=0,max=0;
static int fftMin=0,fftMax=0,fftMaxIndex=-1;
static QVector<int> fftMaxIndexes;
static int fftMarkersCount=5;
static int fftSize=0;
//static int plotCount=0;
//static QList<QVector<double>> seekxItemList,seekyItemList;
static int seekPeriod=1;
static int rowCounter=0;
static double dltValue=0;
static QVector <double> tbFrqValue;
static QVector<QString> tbFrqMode;
static QVector<double> frqTwo;
static QVector<double>frqPower;

static int seekPeriodFailed=-1;
static bool seekStart=false;
static gdouble freqStep;
static gdouble baseFreq;
static gdouble RFBandwidth;
static gdouble RFBandwidthOBS;
static gdouble activeBandwidthMHz = 0.0;
static bool selectTimeChart=false,selectFFTChart=false;
static int sections=1;
//static int currentSections=-1;
//static int seekSection=0;
static int seekStartFreq=0;
//static int seekStopFreq=0;
//static int seekNextFreq=0;
static double seekStep=0;
static int num_capturing_plots=0;
static int tblRowNum{};

namespace Ui {

class Plot;
}

class Plot : public QMainWindow
{
    Q_OBJECT

public:

    // Constructor: Initializes Plot subsystem components, default parameters, and state.
    explicit Plot(QApplication *app,QString name="",QWidget *parent = nullptr);
    // Destructor: Cleans up Plot resources, timers, and allocated memory.
    ~Plot();
    double dltTime{};
    //saeid raziani
    QComboBox      * fftCmb;
    QComboBox      * windowCmb;
    QSpinBox       * avgSpn;
    QDoubleSpinBox * pwrSpn;
    QDoubleSpinBox * txt_start_freq;
    QDoubleSpinBox * txt_stop_freq;
    QDoubleSpinBox * txt_freq_step;
    QDoubleSpinBox * txt_freq_delay;
    QComboBox      * cmb_plot_type;
    QRadioButton   * RdioBtnSmartNoise;
    bool isOtherNoiseOn{};

    QPushButton * btn_plot_frq;

    QPushButton * btn_plot_seek;
    QPushButton * btnHideShow;

    gboolean button_state=false;
    //
    QTimer *timerSeek;
    QTimer *timerChart;
    QTimer *timerChartSweep;
    QTimer *cleanTableTimer;

    bool isSettingShow{};

    QFuture<void> timeFuture;
    QFuture<void> fftFuture;
    QFuture<void> tableFuture;

    QCPRange rangeXTimeChart;
    QCPRange rangeYTimeChart;

    QCPRange rangeXFFTChart;
    QCPRange rangeYFFTChart;

    QPushButton * fillTable;

    QPushButton * btn_capt;
    QPushButton * btnfftcapt;
    QDoubleSpinBox * txtSelectedFreq;

    QCustomPlot * fftChart ;
    double tmpVal{500};
    QDoubleSpinBox * deltaValue ;
    QString userName{};
    // Executes frq value is changed routine.
    void frqValueIsChanged(bool);

    QString Mode{};
    QSpinBox * samplTime;

    bool firstRun{true};

    bool isCaptureOn{false};
    double holdPreFrq{};

    // Executes stop capture routine.
    void stopCapture();
    bool isExciterOn{true}; //it is set by exciter and when it is true, plot doesnt show
    bool isSeekClicked{false};

    QTimer * timerRemoveUnupdatedFrq  = new QTimer;
    QTimer * timerRemoveUnupdatedSeek = new QTimer;

    QTimer * timerRefreshTable = new QTimer;

    bool isBandRejectOn{};
    bool isSmartNoisetOn{};
    QString frqValueStr;
    std::string ipCard {};
    // Retrieves current delta time state.
    double getDeltaTime();


    bool selectingNoiseIsActive{false};

    //waterfal
    QVector<QGLchart *> vectorOfCharts;
    // Sets q glcharts parameters value and updates internal subsystem configuration.
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    UpdateData *waterfallUpdateData;
    QGLchart *waterfallChart ;

    // Executes waterfall update routine.
    void waterfallUpdate();

    QVector<double> maxHold_X, maxHold_Y;

    // Executes exciter mode on routine.
    void exciterModeOn();

    // Executes create text on plot routine.
    QCPItemText * createTextOnPlot();
    double maximumFrq{};
    QTimer sendMaximumFrqTimer;
    QCPItemText * textOnPlot;
    QColor originalColor ;
    QCPItemText *txtLbl;
    double selectedFreqValue{0};
    bool   selectedFreqValid{false};

    QVector<double> *cwFrqValue;
    QVector<double> *cwPowerValue;
    QVector<QString> *cwModeValue;

    double powerFrq{};
    double frqValue{};

    QVector<double> frqMaxCalib;
    QVector<double> powerMaxCalib;
    bool isMaxVectorFilled{false};
    double frqCalib{-10};
    double powerCalib{};



    //void osc_plot_draw_stop(OscPlot *plot);
    //void osc_plot_destroy(OscPlot *plot);
    Plot * osc_plot_new_with_pref(OscPlotPreferences *pref);
    // Executes osc plot new routine.
    Plot * osc_plot_new();
    // Executes osc plot set visible routine.
    void osc_plot_set_visible (Plot *plot, bool visible);

    //    void osc_plot_update_rx_lbl(OscPlot *plot, bool initial_update);
    //    static ssize_t demux_sample(const iio_channel *chn, void *sample, size_t size, void *d);
    void DrawPlot();
    bool isFrqClicked{false};
    // Executes do fft routine.
    static void do_fft(Transform *tr);

    QPushButton * selectedBtn = new QPushButton;

    double maxPower{-600};
    bool isCalibAllowed{false};
    int cntCalib{};

    // Phase 5: spectrum x-axis window = selected freq +/- active bandwidth/2
    // (called from the receiver Profile tab "Set" handler).
    void setActiveBandwidth(double bwMHz);
    // Executes apply bandwidth window routine.
    void applyBandwidthWindow();

public slots:
    // Executes on item clicked routine.
    void onItemClicked();
    //    void onItemClicked(QTreeWidgetItem *item, int column);
    void on_btn_capture_clicked();
    // Event/Signal handler for on btnfftcapture clicked.
    void on_btnfftcapture_clicked();
    // Executes frq value changed slot routine.
    void frqValueChangedSlot(QString);
    // Event/Signal handler for on btn select fft clicked.
    void on_btn_select_fft_clicked();

    // Executes check box unched routine.
    void checkBoxUnched();
private slots:
    // Event/Signal handler for on action save as triggered.
    void on_actionSaveAs_triggered();
    // Event/Signal handler for on action save  whole  page triggered.
    void on_actionSave_Whole_Page_triggered();
    // Event/Signal handler for on action close triggered.
    void on_actionClose_triggered();
    // Event/Signal handler for on action quit triggered.
    void on_actionQuit_triggered();
    // Event/Signal handler for on action plot  title triggered.
    void on_actionPlot_Title_triggered();
    // Event/Signal handler for on action show  settings triggered.
    void on_actionShow_Settings_triggered();
    // Event/Signal handler for on action full  screen triggered.
    void on_actionFull_Screen_triggered();
    // Event/Signal handler for on btn new plot clicked.
    void on_btn_new_plot_clicked();
    // Event/Signal handler for on chk auto scale state changed.
    void on_chk_auto_scale_stateChanged(int arg1);
    //    void on_btn_fullscreen_clicked();

    // Event/Signal handler for on cmb plot type current index changed.
    void on_cmb_plot_type_currentIndexChanged(int index);
    // Executes channel toggled routine.
    void channel_toggled(gchar *pathStr, gpointer plot);

    // Event/Signal handler for on btn zoom in clicked.
    void on_btn_zoom_in_clicked();
    // Event/Signal handler for on btn zoom out clicked.
    void on_btn_zoom_out_clicked();
    // Event/Signal handler for on chk auto scale fft state changed.
    void on_chk_auto_scale_fft_stateChanged(int arg1);
    // Event/Signal handler for on btn zoom in fft clicked.
    void on_btn_zoom_in_fft_clicked();
    // Event/Signal handler for on btn zoom out fft clicked.
    void on_btn_zoom_out_fft_clicked();
    // Event/Signal handler for on txt selected freq value changed.
    void on_txtSelectedFreq_valueChanged(double arg1);
    // Event/Signal handler for on btn select clicked.
    void on_btn_select_clicked();

    // Event/Signal handler for on cmb graph type current index changed.
    void on_cmb_graph_type_currentIndexChanged(int index);

    // Event/Signal handler for on btn capture triggered.
    void on_btn_capture_triggered(QAction *arg1);
    // Event/Signal handler for on btn plot frq clicked.
    void on_btn_plot_frq_clicked();
    // Event/Signal handler for on btn clear clicked.
    void on_btnClear_clicked();
    // Event/Signal handler for on btn hidden clicked.
    void on_btnHidden_clicked();
    // Event/Signal handler for on btn hidden clicked.
    void on_btnHidden_clicked(bool checked);
    // Event/Signal handler for on time data table cell clicked.
    void on_timeDataTable_cellClicked(int row, int column);
    // Event/Signal handler for on time data table item clicked.
    void on_timeDataTable_itemClicked(QTableWidgetItem *item);


    // Event/Signal handler for on btn hide show clicked.
    void on_btnHideShow_clicked(bool checked);

    // Event/Signal handler for on btn save table data clicked.
    void on_btnSaveTableData_clicked();

    // Event/Signal handler for on chk max hold clicked.
    void on_chk_maxHold_clicked(bool checked);
    // Executes select channel with frq routine.
    int selectChannelWithFrq(double);

signals:
    //-------------- [saeid raziani] -----------------
    void ValueSignal(Pulse);
    // Executes frq domain is selected signal routine.
    void frqDomainIsSelectedSignal();
    // Executes seek is selected signal routine.
    void seekIsSelectedSignal();
    // Updates and synchronizes update data data stream.
    void updateData(QCustomSeries a);
    // Transmits send max frq to card signal command/packet over communication interface.
    void sendMaxFrqToCardSignal(double);
    // Executes amp value signal routine.
    void ampValueSignal(double);
    // Transmits send time domain to card signal command/packet over communication interface.
    void sendTimeDomainToCardSignal(QString);
    // Executes selected smart noise status signal routine.
    void selectedSmartNoiseStatusSignal(bool);
    // Sets ting hide show signal value and updates internal subsystem configuration.
    void settingHideShowSignal(bool);
    // Executes cw table value signal routine.
    void cwTableValueSignal(QVector<double>,QVector<double>,QVector<QString>,QVector<long long int>);
    // Retrieves current max value signal state.
    void getMaxValueSignal(double,double);
    //    void test(modes);


private:
    Ui::Plot *ui;

    OscPlotPrivate *priv;
     int cntWait{};
     double holdPreX[30] = {};
     double holdPreY[30] = {};
    QApplication *mApp;
    struct osc_plugin *spect_analyzer_plugin;
    int cntp{};

    QMenu *fftMenu=nullptr;
    QFuture<void> hoppingFuture;
    //bool hopping=false;
    bool freqChanged=false;
    Detector *detector;
    bool detectorEnabled=false;
    int detectorSourceIndex=0;
    iio_context *ctx;
    //saeid raziani
    int prePulseCount{0};
    QList<Pulse> prePulse;
    bool duplicate = false;
    //    int toachannel1[10]={5};
    std::vector<std::vector<long double>> toaChannel1;
    std::vector<std::vector<int>> toaChannel;
    std::vector<std::vector<long double>> deltaTime;
    // Executes calculating p r i routine.
    void calculatingPRI(Pulse*, double);
    int cnt{0};

    int countArray[32] ={};
    // Executes remove unupdated row routine.
    void removeUnupdatedRow();
    QTimer selectedSmartNoiseTimer;

    QMap<int, QString> timeMap;
    // Executes save to c s v in thread routine.
    void saveToCSVInThread(QTableWidget *tableWidget, const QString &fileName) ;


    QList<ChannelItemData*> ChannelItemDataList;
    gboolean stop_capture;

    // Executes create plot routine.
    void create_plot();
    //static void osc_plot_class_init(OscPlotClass *klass);
    //void osc_plot_data_update (OscPlot *plot);
    //static bool call_all_transform_functions(OscPlotPrivate *priv);
    //void osc_plot_restart (OscPlot *plot);
    void device_list_treeview_init();
    // Event/Signal handler for single shot clicked cb.
    void single_shot_clicked_cb(gpointer data);
    // Executes plot channels add device routine.
    void plot_channels_add_device( const char *dev_name);
    // Executes plot channel add to plot routine.
    void plot_channel_add_to_plot(PlotChn *settings);
    // Executes plot channels add channel routine.
    void plot_channels_add_channel(PlotChn *pchn);
    // Executes treeview expand update routine.
    void treeview_expand_update();
    // Executes saveas channels list fill routine.
    void saveas_channels_list_fill();
    // Executes check valid setup routine.
    gboolean check_valid_setup();
    // Executes check valid setup of all devices routine.
    gboolean check_valid_setup_of_all_devices();
    // Executes check valid setup of device routine.
    gboolean check_valid_setup_of_device(const char *name);
    // Executes num of channels of device routine.
    int num_of_channels_of_device(const char *name);
    // Executes enabled channels count routine.
    int enabled_channels_count();
    // Executes enabled channels of device routine.
    int enabled_channels_of_device(QString name, unsigned *enabled_mask);
    // Executes plot channels update routine.
    void plot_channels_update();
    // Executes plot get sample count of device routine.
    int plot_get_sample_count_of_device(const char *device);
    // Executes osc plot get sample count routine.
    double osc_plot_get_sample_count();
    // Executes collect parameters from plot routine.
    void collect_parameters_from_plot();
    // Executes remove all transforms routine.
    void remove_all_transforms();
    // Executes remove transform from list routine.
    void remove_transform_from_list(Transform *tr);
    // Executes devices transform assignment routine.
    void devices_transform_assignment();
    // Executes plot setup routine.
    void plot_setup();
    // Executes markers init routine.
    void markers_init();
    // Executes capture start routine.
    void capture_start();
    // Executes dispose parameters from plot routine.
    void dispose_parameters_from_plot();
    // Executes plot redraw routine.
    gboolean plot_redraw();
    // Executes deassert used channels routine.
    void deassert_used_channels();
    // Executes osc plot update rx lbl routine.
    void osc_plot_update_rx_lbl(bool initial_update);
    // Executes device rx info update routine.
    void device_rx_info_update();
    // Executes fps counter routine.
    void fps_counter();
    // Executes draw marker values routine.
    void draw_marker_values(Transform *tr);
    static iio_device *transform_get_device_parent(Transform *transform);
    // Executes markers phase diff show routine.
    void markers_phase_diff_show();
    // Retrieves current frequency_transform state.
    bool is_frequency_transform();
    // Executes transform add plot markers routine.
    void transform_add_plot_markers(Transform *transform);
    // Executes transform add own markers routine.
    void transform_add_own_markers(Transform *transform);
    // Updates and synchronizes update_grid data stream.
    void update_grid(gfloat left, gfloat right);
    // Executes show controls routine.
    void ShowControls(int index);
    // Event/Signal handler for count changed cb.
    void count_changed_cb();
    // Executes call all transform functions routine.
    bool call_all_transform_functions();
    // Executes osc plot data update routine.
    void osc_plot_data_update();
    // Executes start routine.
    void start(gboolean start_event);
    // Executes capture setup routine.
    int capture_setup();
    // Executes channels transform assignment routine.
    void channels_transform_assignment(PlotChn * chn,ch_tr_params *user_data);
    Transform *add_transform_to_list(int tr_type, GSList *channels);
    // Updates and synchronizes update_transform_settings data stream.
    void update_transform_settings(Transform *transform);
    // Executes plot get sample count for transform routine.
    int plot_get_sample_count_for_transform(Transform *transform);
    // Executes xcorr routine.
    void xcorr(fftw_complex *signala, fftw_complex *signalb, fftw_complex *result, int N, double avg);
    // Executes cross correlation transform function routine.
    bool cross_correlation_transform_function(Transform *tr, gboolean init_transform);
    static QVector<double> *plot_channels_get_nth_data_ref(GSList *list, guint n);
    // Executes time transform function routine.
    static bool time_transform_function(Transform *tr, gboolean init_transform);
    // Executes freq spectrum transform function routine.
    static bool freq_spectrum_transform_function(Transform *tr, gboolean init_transform);
    // Executes fft transform function routine.
    static bool fft_transform_function(Transform *tr, gboolean init_transform);
    // Executes constellation transform function routine.
    static bool constellation_transform_function(Transform *tr, gboolean init_transform);
    // Executes capture process routine.
    void capture_process();
    iio_buffer *osc_plot_get_buffer();
    // Updates and synchronizes update_plot data stream.
    void update_plot(iio_buffer *buf);
    // Executes capture process ended routine.
    void capture_process_ended();

    // Executes initialize plots routine.
    void InitializePlots();

    // Executes get base freq routine.
    bool GetBaseFreq();
    // Executes re scale plot routine.
    void reScale_plot(int type);
    // Executes draw f f t plot routine.
    void DrawFFTPlot();
    // Executes start capture routine.
    void startCapture();

    //saeid raziani
    void fillingTable();
    QTimer callingFillTableTimer;
    bool isSmartNoiseActive{};
    int selectedRow{-1};

    //    class CustomDelegate : public QStyledItemDelegate {
    //    public:
    //        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //    };


    //    CustomDelegate customDelegate;





    // Executes change frequency hopping routine.
    void ChangeFrequencyHopping(iio_channel *ch1,double freq);
    // Executes demux sample routine.
    static ssize_t demux_sample(const iio_channel *chn, void *sample, size_t size, void *d);
    // Retrieves current max state.
    void getMax(Transform *tr);
    // Retrieves current min state.
    void getMin(Transform *tr);
    // Retrieves current f f t max state.
    static void getFFTMax(Transform *tr,bool is_seek=false);
    // Retrieves current f f t min state.
    static void getFFTMin(Transform *tr,bool is_seek=false);
    // Executes stop sampling routine.
    void stop_sampling();
    // Executes close active buffers routine.
    void close_active_buffers();
    double y{};
    std::vector<double> tableData;
    QTimer sendTimeDataToCardTimer;


    //    bool isFrqClicked{false};

    // Executes save y axis to file routine.
    void saveYAxisToFile(const QVector<double> &, const QVector<double> &,const QString &filename);
    QVector<double>  timeDataYaxis_i;
    QVector<double>  timeDataYaxis_q;

    // Executes smart time domain routine.
    void smartTimeDomain();


signals:
    // Executes hopping signal routine.
    void HoppingSignal(bool,double min=0,double max=0,double step=0,int delay=0);
    // Executes seeking is off routine.
    void seekingIsOff();
    // Transmits send channel to card signal command/packet over communication interface.
    void sendChannelToCardSignal(double);
    // Executes t h switch signal routine.
    void THSwitchSignal(bool);
    // Executes check box status routine.
    void checkBoxStatus(int);
    // Executes selected frq min max signal routine.
    void selectedFrqMinMaxSignal(double, double);
    // Executes clean time signal routine.
    void cleanTimeSignal(double);


};





#endif // PLOT_H
