#include "ui_plot.h"
#include "plot.h"
#include <QDateTime>
#include <QString>
#include <receiver/oscmain.h>
#include "qtconcurrentrun.h"



#pragma region Properties {

static unsigned object_count = 0;


///* Helpers */
#define TIME_SETTINGS(obj) ((struct _time_settings *)obj->settings)
#define FFT_SETTINGS(obj) ((struct _fft_settings *)obj->settings)
#define CONSTELLATION_SETTINGS(obj) ((struct _constellation_settings *)obj->settings)
#define XCORR_SETTINGS(obj) ((struct _cross_correlation_settings *)obj->settings)
#define FREQ_SPECTRUM_SETTINGS(obj) ((struct _freq_spectrum_settings *)obj->settings)
#define MATH_SETTINGS(obj) ((struct _math_settings *)obj->settings)

#define PLOT_CHN(obj) ((PlotChn *)obj)
#define PLOT_IIO_CHN(obj) ((PlotIioChn *)obj)
#define PLOT_MATH_CHN(obj) ((PlotMathChn *)obj)

/* signals will be configured during class init */
//static guint oscplot_signals[LAST_SIGNAL] = { 0 };

struct int_and_plot
{
    int int_obj;
    OscPlot *plot;
};

struct string_and_plot {
    char *string_obj;
    OscPlot *plot;
};

struct plot_geometry {
    gint width;
    gint height;
};

struct _OscPlotPrivate
{
    //	GtkBuilder *builder;

    QList<QString> deviceList;
    int object_id;

    /* Graphical User Interface */
    QWidget *window;
    QTreeWidget *channel_list_view;
    unsigned int sample_count;

    QTextDocument* tbuf;
    QTextDocument* devices_buf;
    QTextDocument* phase_buf;
    QTextDocument* math_expression;

    OscPlotPreferences *preferences;

    unsigned int nb_input_devices;
    unsigned int nb_plot_channels;

    struct plot_geometry size;

    int frame_counter;
    double fps;
    struct timeval last_update;

    int last_hor_unit;

    int do_a_rescale_flag;

    gulong capture_button_hid;
    gint deactivate_capture_btn_flag;

    bool single_shot_mode;

    /* A reference to the device holding the most recent created transform */
    struct iio_device *current_device;

    /* List of transforms for this plot */
    QList<Transform*> *transform_list;

    /* Active transform type for this window */
    int active_transform_type;

    /* Transform currently holding the fft marker */
    Transform *tr_with_marker;

    /* Type of "Save As" currently selected*/
    gint active_saveas_type;

    /* The set of markers */
    struct marker_type markers[MAX_MARKERS + 2];
    struct marker_type *markers_copy;
    enum marker_types marker_type;

    /* Settings list of all channel */
    GSList *ch_settings_list;

    /* Databox data */
    //GtkDataboxGraph *grid;
    gfloat gridy[25], gridx[25];

    /* Spectrum mode - Parameters */
    unsigned fft_count;
    double start_freq;
    double filter_bw;

    gint line_thickness;

    gint redraw_function=1;
    gboolean stop_redraw=false;
    gboolean redraw;

    bool spectrum_data_ready;

    gboolean fullscreen_state;

    bool profile_loaded_scale;

    bool save_as_png;

    char *saveas_filename;

    struct int_and_plot fix_marker;
    struct string_and_plot add_mrk;
    struct string_and_plot remove_mrk;
    struct string_and_plot peak_mrk;
    struct string_and_plot fix_mrk;
    struct string_and_plot single_mrk;
    struct string_and_plot dual_mrk;
    struct string_and_plot image_mrk;
    struct string_and_plot off_mrk;

    gulong fixed_marker_hid;

    gint plot_x_pos;
    gint plot_y_pos;

    gfloat plot_left;
    gfloat plot_right;
    gfloat plot_top;
    gfloat plot_bottom;
    int read_scale_params;

    GMutex g_marker_copy_lock;

    void (*quit_callback)(void *user_data);
    void *qcb_user_data;
};

//G_DEFINE_TYPE_WITH_PRIVATE(OscPlot, osc_plot, GTK_TYPE_WIDGET)

#pragma endregion }

#pragma region Constructor {

Plot::Plot(QApplication *app,QString name,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Plot)
{
    ui->setupUi(this);

    ui->prgWaitingFrq->setVisible(false);
    //    connect()

    dltValue = ui->dspnDeltabu->value();

    connect(ui->dspnDeltabu,QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){
        dltValue = val;
    });


    cwModeValue = new QVector<QString>;


    // this timer is for clealing table time
    cleanTableTimer = new QTimer;
    cleanTableTimer->setInterval(ui->spnClear->value()*1000);
    cleanTableTimer->start();

    connect(ui->spnClear,QOverload<int>::of(&QSpinBox::valueChanged), [&](int val){
        cleanTableTimer->setInterval(val*1000);
        emit cleanTimeSignal(val*1000);
    });
    connect(cleanTableTimer, &QTimer::timeout, [&](){

        ui->btnClear->clicked();

    });

    //-----------------------------------------

    selectedBtn = ui->btn_select_fft;
    //    prePulse = new QList<Pulse*>();

    //saeid raziani
    //    ui->btn_plot_seek->setVisible(false);

    //    const int vecRowSize{32};
    //    const int vecColSize{10};

    //active in smart noise -----------------------------------




    //    layout->addWidget(lbl);
    //    lbl->setText("Active");
    //    lbl->setStyleSheet("margin:-10px;color:black");

    //    //    cellWidget->resize(100,100);

    //    layout->setAlignment(Qt::AlignCenter);

    //    activeWig->setLayout(layout);

    //-----------------------------------------------

    //    toaChannel1(vecRowSize,std::vector<int>(vecColSize));

    //    textOnPlot = createTextOnPlot();


    //this timer send noise data to card
    selectedSmartNoiseTimer.setInterval(1500);
    connect(&selectedSmartNoiseTimer, &QTimer::timeout,[=]{ emit sendMaxFrqToCardSignal(maximumFrq);});


    toaChannel1.resize(32);
    deltaTime.resize(32);

    connect(&callingFillTableTimer, &QTimer::timeout, this, &Plot::fillingTable);
    callingFillTableTimer.setInterval(30);
    callingFillTableTimer.start();

    fftCmb    = findChild<QComboBox      *> ("cmb_fft_size");
    windowCmb = findChild<QComboBox      *> ("cmb_fft_win");
    avgSpn    = findChild<QSpinBox       *> ("txt_fft_avg");
    pwrSpn    = findChild<QDoubleSpinBox *> ("txt_pwr_offset");

    txt_start_freq  = ui->txt_start_freq;
    txt_stop_freq   = ui->txt_stop_freq;
    txt_freq_step   = ui->txt_freq_step;
    txt_freq_delay  = ui->txt_freq_delay;
    txtSelectedFreq = ui->txtSelectedFreq;
    btnHideShow = ui->btnHidden;


    cmb_plot_type= ui->cmb_plot_type;

    btn_capt = ui->btn_capture;

    fillTable = ui->btn_fill_table_data;
    btnfftcapt = ui->btnfftcapture;

    btn_plot_frq = ui->btn_plot_frq;
    btn_plot_seek = ui->btn_plot_seek;

    samplTime = ui->sample_count_widget;

    ui->btn_fill_table_data->setVisible(false);
    ui->btn_capture->setVisible(false);

    ui->btn_plot_seek->setText("sweep\n8-12");

    ui->cmbChannels->setVisible(false);
    //    bool f= connect(ui->cmbChannels, QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int t){
    ui->cmbChannels->setCurrentIndex(0);
    ui->cmbChannels->currentIndexChanged(0);
    //    });
    ui->menubar->setVisible(false);
    ui->btnfftcapture->setVisible(false);


    //
    setObjectName(name);
    RdioBtnSmartNoise = new QRadioButton();

    //ip address
    //    ctx=iio_create_context_from_uri("ip:5.190.48.246");
    //getting ip with this command in terminal
    //sudo picocom -b 115200 -l -r /dev/ttyUSB0

    // Reuse the receiver's already validated IIO context.
    // Creating a second network context here can fail independently and
    // leaves the plots disconnected from the context used by the plugins.
    ctx = globals::ctx;
    if(!ctx)
        qCritical() << "Plot created without a valid IIO context";

    mApp=app;
    priv=new _OscPlotPrivate;

    // Signal hide or show The time domain chart in Frequncy Domain mode
    QObject::connect(ui->btn_fill_table_data,&QPushButton::clicked,[=](){

        if(!detector->isRunning())
        {
            ui->btn_fill_table_data->setToolTip ("Stop");
            ui->btn_fill_table_data->setIcon(QIcon(":/icon/stop"));
            detector->start();
            detectorEnabled=true;
        }

        else
        {
            detectorEnabled=false;
            ui->btn_fill_table_data->setToolTip ("Capture");
            ui->btn_fill_table_data->setIcon(QIcon(":/icon/start"));
            detector->setStop(true);
        }
    });

    //    QObject::connect(ui->btnToggleTime,&QPushButton::clicked,[=](){

    //        if(ui->cmb_plot_type->currentIndex()>0)
    //        {
    //            ui->timeChart->setVisible(!ui->timeChart->isVisible());
    //            ui->TimeTable->setVisible(!ui->TimeTable->isVisible());

    //            ui->btnToggleTime->setText(ui->timeChart->isVisible()?"Hide":"Show");
    //        }
    //        else
    //        {
    //            ui->timeChart->setVisible(!ui->timeChart->isVisible());
    //        }

    //    });

    // Create timer for ploting The time domain data
    timerChart=new QTimer();
    timerChart->setInterval(5);
    QObject::connect(timerChart,&QTimer::timeout,this,[=](){
        capture_process();
        //        plot_redraw();
        //        DrawPlot();
    });


    //raziani
    timerChartSweep = new QTimer();
    timerChartSweep->setInterval(5);
    QObject::connect(timerChartSweep,&QTimer::timeout,this,[=](){

        plot_redraw();
        DrawPlot();
    });



    // Time domain chart show grid signal
    QObject::connect(ui->chk_show_grid,&QCheckBox::stateChanged,this,[=](int state){

        if(state==2)
        {
            ui->timeChart->xAxis->grid()->setVisible(true);
            ui->timeChart->yAxis->grid()->setVisible(true);
        }
        else
        {
            ui->timeChart->xAxis->grid()->setVisible(false);
            ui->timeChart->yAxis->grid()->setVisible(false);
        }
    });

    // Frequency domain chart show grid signal
    QObject::connect(ui->chk_show_grid_fft,&QCheckBox::stateChanged,this,[=](int state){

        if(state==2)
        {
            ui->fftChart->xAxis->grid()->setVisible(true);
            ui->fftChart->yAxis->grid()->setVisible(true);
        }
        else
        {
            ui->fftChart->xAxis->grid()->setVisible(false);
            ui->fftChart->yAxis->grid()->setVisible(false);
        }
    });



    // Get Current frequency
    GetBaseFreq();
    // Track the board base (LO) frequency so the spectrum diagram follows
    // retuning (it used to freeze at the construction-time value).
    QTimer *baseFreqTimer = new QTimer(this);
    connect(baseFreqTimer, &QTimer::timeout, this, [=](){ GetBaseFreq(); });
    baseFreqTimer->start(500);

    //===========================================================================
    //saeid raziani ==> Mode variable stores the value of the mode and specifies which mode the plot should be run in
    ui->btn_capture->setStyleSheet("background-color:red; color:white");
    ui->btnfftcapture->setStyleSheet("background-color:red; color:white");

    Mode = name;
    ui->timeChart->setVisible(Mode == SMART_TIME_DOMAIN?true:false);

    ui->settingsMenu->setVisible(false);
    ui->TimeTable->setVisible(false);
    ui->fftChart->setVisible(false);
    ui->fftSettings->setVisible(false);
    ui->fftTable->setVisible(false);
    ui->timeSettings->setVisible(false);
    ui->btn_zoom_in->setVisible(false);
    ui->btn_zoom_out->setVisible(false);
    ui->btn_select->setVisible(false);
    ui->timewidget->setVisible(false);
    ui->fftSettings->setVisible(false);
    ui->grbFFTChart->setVisible(false);

    //    ui->timeDataTable->setItemDelegate(&customDelegate);

    fftChart = ui->fftChart;



    if(Mode == TIME_DOMAIN)
    {
        ui->timewidget->setVisible(true);
        ui->TimeTable->setVisible(true);
    }

    else
    {
        ui->fftChart->setVisible(true);
        ui->fftSettings->setVisible(true);
        ui->grbFFTChart->setVisible(true);

        //this connection is work just in frq domain
        connect(ui->btn_plot_frq, &QPushButton::clicked, [&](){
            timerChartSweep->setInterval(5);
            isSeekClicked = false;
            ui->btn_plot_seek->setEnabled(false);

            //this variable (isFrqClicked) is for holding state.false means if user click on frq btn again it doesnt work
            //            if(!isFrqClicked)
            //            {
            //                QTimer::singleShot(3000,[&]{
            isFrqClicked = true;
            //            ui->btn_plot_frq->setStyleSheet("background-color:green" );
            //            ui->btn_plot_seek->setStyleSheet("background-color:black");
            emit frqDomainIsSelectedSignal();
            //                });

            //            }
            QTimer::singleShot(4000,[&]{
                ui->btn_plot_seek->setEnabled(true);});
        });


        connect(ui->btn_plot_seek, &QPushButton::clicked, [&]{


            //            holdPreFrq = baseFreq;
            timerChartSweep->setInterval(9000);
            isFrqClicked = false;
            ui->btn_plot_frq->setEnabled(false);
            if(!isSeekClicked)
            {
                //                 QTimer::singleShot(3000,[&]{
                isSeekClicked = true;
                //                ui->btn_plot_seek->setStyleSheet("background-color:green");
                //                ui->btn_plot_frq->setStyleSheet("background-color:black");
                if(isSeekClicked) emit seekIsSelectedSignal();
                //                 });
            }
            //            else {
            QTimer::singleShot(4000,[&]{
                ui->btn_plot_frq->setEnabled(true);
                //                emit seekingIsOn();
            });
            //            }

        }
        );
    }

    timerRemoveUnupdatedFrq->setInterval(1500);
    timerRemoveUnupdatedSeek->setInterval(15000);

    //    connect(timerRemoveUnupdatedFrq,&QTimer::timeout, this, &Plot::removeUnupdatedRow);
    //    connect(timerRemoveUnupdatedSeek,&QTimer::timeout, this, &Plot::removeUnupdatedRow);



    //update table



    if(Mode == TIME_DOMAIN)
    {

        connect(ui->spnRefresh,QOverload<int>::of(&QSpinBox::valueChanged),[&](int val){ timerRefreshTable->setInterval(val*1000);});

        //this code checks if the table doesnt update for 4 second then clicks on capture button in order to refreh table
        timerRefreshTable->setInterval(ui->spnRefresh->value()*1000);
        timerRefreshTable->start();


        connect(timerRefreshTable, &QTimer::timeout, this, [this]{

            if (timeMap.isEmpty())
                return;

            QTime t1 = QTime::currentTime();
            uint currentTime =  t1.toString("ss").toUInt();
            uint rowDataTome = timeMap.value(timeMap.size() -1).toUInt();

            //qInfo()<< Mode <<"=============================> " << ui->timeDataTable->rowCount();
            tblRowNum = ui->timeDataTable->rowCount();
            if(currentTime - rowDataTome > 4)
            {
                on_btn_capture_clicked();
                timeMap.clear();
            }

        });
    }

    //===========================================================================

    //this timer if for sending time data to card when user press btn
    //smart noise

    sendTimeDataToCardTimer.setInterval(6000);
    connect(&sendTimeDataToCardTimer, &QTimer::timeout,[&]
    {
        saveYAxisToFile(timeDataYaxis_i,timeDataYaxis_q, "y_axis.txt");
        emit THSwitchSignal(false); //false means tx be on
        emit sendTimeDomainToCardSignal("y_axis.txt");


        QTimer::singleShot(3000,[&]{
            emit THSwitchSignal(true);
        });
    });

    // ------------------------- waterfall plot --------------------------------
    QVBoxLayout *vbLayoutWaterFall = new QVBoxLayout();
    waterfallChart = new QGLchart();
    vectorOfCharts.append(waterfallChart);
    setQGlchartsParameters(waterfallChart, ChartType::Waterfall);
    vbLayoutWaterFall->addWidget(waterfallChart);
    ui->wdgWaterFall->setLayout(vbLayoutWaterFall);
    connect(this, SIGNAL(updateData(QCustomSeries)), waterfallChart, SLOT(updateSeriesData(QCustomSeries)));
    //------------------------------------------------------------

    //    txtLbl = new QCPItemText(ui->fftChart);

    //    ui->timeDataTable->setStyleSheet("background-color:");

    deltaValue = ui->dspnDeltabu;

}

//void Plot::setTunerClickedSlot()
//{


//    //raziani
//    waterfallChart->axes(Qt::AlignBottom).at(0)->setRange((spbCenterFrequency->value() - spbBandWidth->value() / 2),
//                                                          (spbCenterFrequency->value() + spbBandWidth->value() / 2));
//    panoramaUpdateData->start();
//    detectedAmpUpdateData->start();
//    waterfallUpdateData->start();
//}


void Plot::waterfallUpdate()
{
    //    QCustomSeries a;
    //    QVector2D v2d;


    //    if(prePulse.size() > 0)
    //    {

    //        for (int i=0; i< prePulse.size(); i++)
    //        {
    //            for(int i = 0; i < 10000; i++)
    //            {
    //                v2d.setX(i);
    //                v2d.setY(prePulse.at(i).Freq);
    //                a.addPoints(v2d);
    //            }

    //            emit(updateData(a));
    //            a.removeAllPoints();
    //        }

    //    }
}

void Plot::setQGlchartsParameters(QGLchart *glChart, ChartType glChartType)
{
    //    glChart->setMinimumSize(QSize(2000, 300));
    QCustomAxis *verticalAxis = new QCustomAxis(this);
    verticalAxis->setAlignment(Qt::AlignLeft);
    verticalAxis->setOrientation(Qt::Vertical);
    verticalAxis->setGridLineColor(Qt::white);
    verticalAxis->setVisible(false);
    verticalAxis->setRange(-87,-64);


    glChart->addAxis(verticalAxis, Qt::AlignLeft);

    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    horizantalAxis->setAlignment(Qt::AlignBottom);
    horizantalAxis->setOrientation(Qt::Horizontal);
    horizantalAxis->setGridLineColor(Qt::white);
    horizantalAxis->setVisible(false);
    //        horizantalAxis->setRange(-110,-12);
    //    horizantalAxis->setTitleText("Frequency");
    //    horizantalAxis->setLabelsText("(MHz)");
    //====================
    //    horizantalAxis->setTickPen(QPen(Qt::red));

    //    horizantalAxis->set
    //=========================
    glChart->addAxis(horizantalAxis, Qt::AlignBottom);
    glChart->enableMasking(true);
    glChart->setMaskingModifiable(true);
    glChart->setMouseTracking(true);

    //        glChart->setVerticalLowerRangeMargin(-87);
    //        glChart->setVerticalUpperRangeMargin(-64);

    //    glChart->enableToolTip(true);
    glChart->setChartType(glChartType);
    glChart->enableSyncedZoom(false);
    //    glChart->zoomEnable(true);
}



Plot::~Plot()
{
    delete ui;
}

void Plot::frqValueIsChanged(bool state)
{
    ui->prgWaitingFrq->setVisible(state);
    ui->prgWaitingFrq->setMinimum(0);
    ui->prgWaitingFrq->setMaximum(0);
    ui->prgWaitingFrq->setValue(0);
    //    if(!state) ui->prgWaitingFrq->
}

#pragma endregion }

#pragma region Static Functions {

/* Ref:
 *    A Family of Cosine-Sum Windows for High-Resolution Measurements
 *    Hans-Helge Albrecht
 *    Physikalisch-Technische Bendesanstalt
 *   Acoustics, Speech, and Signal Processing, 2001. Proceedings. (ICASSP '01).
 *   2001 IEEE International Conference on   (Volume:5 )
 *   pgs. 3081-3084
 *
 * While this doesn't use any of his code - I did find the coeffients that were nicely
 * typed in by Joe Henning as part of his MATLAB Window Utilities
 * (https://www.mathworks.com/matlabcentral/fileexchange/46092-window-utilities)
 *
 */
static double window_function(gchar * win, int j, int n)
{
    /* Strings need to match what is in glade */
    if (!g_strcmp0(win, "Hanning")) {
        double a = 2.0 * M_PI / (n - 1);
        return 0.5 * (1.0 - cos(a * j));
    } else if (!g_strcmp0(win, "Boxcar")) {
        return 1.0;
    } else if (!g_strcmp0(win, "Triangular")) {
        double a = fabs(j - (n - 1)/ 2.0) / ((n - 1.0) / 2.0);
        return 1.0 - a;
    } else if (!g_strcmp0(win, "Welch")) {
        double a = (j - (n - 1.0) / 2.0) / ((n - 1.0) / 2.0);
        return 1.0 - (a * a);
    } else if (!g_strcmp0(win, "Cosine")) {
        double a = M_PI * j / (n - 1);
        return sin(a);
    } else if (!g_strcmp0(win, "Hamming")) {
        double a0 = 0.5383553946707251, a1 = .4616446053292749;
        return a0 - a1 * cos(j * 2.0 * M_PI / (n - 1));
    } else if (!g_strcmp0(win, "Exact Blackman")) {
        /* https://ieeexplore.ieee.org/document/940309 */
        double a0 = 7938.0/18608.0, a1 = 9240.0/18608.0, a2 = 1430.0/18608.0;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a);
    } else if (!g_strcmp0(win, "3 Term Cosine")) {
        double a0 = 4.243800934609435e-1, a1 = 4.973406350967378e-1, a2 = 7.827927144231873e-2;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a);
    } else if (!g_strcmp0(win, "4 Term Cosine")) {
        double a0 = 3.635819267707608e-1, a1 = 4.891774371450171e-1, a2 = 1.365995139786921e-1,
                a3 = 1.064112210553003e-2;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a);
    } else if (!g_strcmp0(win, "5 Term Cosine")) {
        double a0 = 3.232153788877343e-1, a1 = 4.714921439576260e-1, a2 = 1.755341299601972e-1,
                a3 = 2.849699010614994e-2, a4 = 1.261357088292677e-3;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a) + a4 * cos(4.0 * a);
    } else if (!g_strcmp0(win, "6 Term Cosine")) {
        double a0 = 2.935578950102797e-1, a1 = 4.519357723474506e-1, a2 = 2.014164714263962e-1,
                a3 = 4.792610922105837e-2, a4 = 5.026196426859393e-3, a5 = 1.375555679558877e-4;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(1.0 * a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a) + a4 * cos(4.0 * a) -
                a5 * cos(5.0 * a);
    } else if (!g_strcmp0(win, "7 Term Cosine")) {
        double a0 = 2.712203605850388e-1, a1 = 4.334446123274422e-1, a2 = 2.180041228929303e-1,
                a3 = 6.578534329560609e-2, a4 = 1.076186730534183e-2, a5 = 7.700127105808265e-4,
                a6 = 1.368088305992921e-5;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(1.0 * a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a) + a4 * cos(4.0 * a) -
                a5 * cos(5.0 * a) + a6 * cos(6.0 * a);
    } else if (!g_strcmp0(win, "Blackman-Harris")) {
        double a0 = 3.58750287312166e-1, a1 = 4.88290107472600e-1, a2 = 1.41279712970519e-1,
                a3 = 1.16798922447150e-2;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a);
    } else if (!g_strcmp0(win, "Flat Top")) {
        double a0 = 2.1557895e-1, a1 = 4.1663158e-1, a2 = 2.77263158e-1,
                a3 = 8.3578947e-2, a4 = 6.947368e-3;
        double a = j * 2.0 * M_PI / (n - 1);
        return a0 - a1 * cos(a) + a2 * cos(2.0 * a) - a3 * cos(3.0 * a) + a4 * cos(4.0 * a);
    }

    printf("unknown window function\n");
    return 0;
}

/* This equalized power, so full scale is always 0dBFS */
static double window_function_offset(QString win)
{
    /* Strings need to match what is in glade */
    if (win == "Hanning") {
        return 1.77;
    } else if (win == "Boxcar") {
        return -4.25;
    } else if (win == "Triangular") {
        return 1.77;
    } else if (win == "Welch") {
        return -0.73;
    } else if (win == "Cosine") {
        return -0.33;
    } else if (win == "Hamming") {
        return 1.13;
    } else if (win == "Exact Blackman") {
        return 3.15;
    } else if (win == "3 Term Cosine") {
        return 3.19;
    } else if (win == "4 Term Cosine") {
        return 4.54;
    } else if (win == "5 Term Cosine") {
        return 5.56;
    } else if (win == "6 Term Cosine") {
        return 6.39;
    } else if (win == "7 Term Cosine") {
        return 7.08;
    } else if (win == "Blackman-Harris") {
        return 4.65;
    } else if (win == "Flat Top") {
        return 9.08;
    }
    //    printf("missed\n");
    //    return 0;
    return 1.77;
}

static struct iio_device * transform_get_device_parent(Transform *transform)
{
    struct iio_device *iio_dev = NULL;
    PlotChn *plot_ch;

    if (!transform || !transform->plot_channels)
        return NULL;

    plot_ch = static_cast<PlotChn *>(transform->plot_channels->data);

    if (plot_ch)
        iio_dev = plot_ch->get_iio_parent(plot_ch);

    return iio_dev;
}

static void transform_remove_own_markers(Transform *transform)
{
    struct marker_type *markers;

    if (transform->has_the_marker)
        return;

    if (transform->type_id == FFT_TRANSFORM ||
            transform->type_id == COMPLEX_FFT_TRANSFORM) {
        markers = FFT_SETTINGS(transform)->markers;
    } else if (transform->type_id == CROSS_CORRELATION_TRANSFORM) {
        markers = XCORR_SETTINGS(transform)->markers;
    } else {
        return;
    }

    if (markers)
        free(markers);
}

static unsigned int max_sample_count_from_plots(struct extra_dev_info *info)
{
    unsigned int max_count = 0;
    struct plot_params *prm;
    GSList *node;
    GSList *list = info->plots_sample_counts;

    for (node = list; node; node = g_slist_next(node))
    {
        prm =(plot_params *) node->data;
        if (prm->sample_count > max_count)
            max_count = prm->sample_count;
    }

    return max_count;
}

static double read_sampling_frequency(const struct iio_device *dev)
{
    double freq = 400.0;
    int ret = -1;
    unsigned int i, nb_channels = iio_device_get_channels_count(dev);
    const char *attr;
    char buf[1024];

    for (i = 0; i < nb_channels; i++) {
        struct iio_channel *ch = iio_device_get_channel(dev, i);

        if (iio_channel_is_output(ch) || strncmp(iio_channel_get_id(ch),
                                                 "voltage", sizeof("voltage") - 1))
            continue;

        ret = iio_channel_attr_read(ch, "sampling_frequency",
                                    buf, sizeof(buf));
        if (ret > 0)
            break;
    }

    if (ret < 0)
        ret = iio_device_attr_read(dev, "sampling_frequency",
                                   buf, sizeof(buf));
    if (ret < 0) {
        const struct iio_device *trigger;

        ret = osc_iio_device_get_trigger(dev, &trigger);

        if (ret == 0 && trigger) {
            attr = iio_device_find_attr(trigger, "sampling_frequency");
            if (!attr)
                attr = iio_device_find_attr(trigger, "frequency");
            if (attr)
                ret = iio_device_attr_read(trigger, attr, buf,
                                           sizeof(buf));
            else
                ret = -ENOENT;
        }
    }

    if (ret > 0)
        sscanf(buf, "%lf", &freq);

    if (freq < 0)
        freq += 4294967296.0;

    return freq;
}

/**
 * @brief demux_sample
 * @param chn
 * @param sample
 * @param size
 * @param d
 * @return
 */
ssize_t Plot::demux_sample(const struct iio_channel *chn,
                           void *sample, size_t size, void *d)
{
    if (!chn || !sample)
        return 0;

    struct extra_info *info =(extra_info *) iio_channel_get_data(chn);
    if (!info || !info->dev || !info->data_ref)
        return 0;

    struct extra_dev_info *dev_info =(extra_dev_info *) iio_device_get_data(info->dev);
    if (!dev_info)
        return 0;

    const struct iio_data_format *format = iio_channel_get_data_format(chn);
    if (!format)
        return 0;

    /* Prevent buffer overflow */
    if (info->offset < 0 ||
        static_cast<unsigned long>(info->offset) >= static_cast<unsigned long>(dev_info->sample_count) ||
        info->offset >= info->data_ref->size())
        return 0;

    if (size == 1) {
        int8_t val;
        iio_channel_convert(chn, &val, sample);
        if (format->is_signed)
            (*info->data_ref)[info->offset++]= (gfloat) val;
        else
            (*info->data_ref)[info->offset++]= (gfloat) (uint8_t)val;
    } else if (size == 2) {
        int16_t val;
        iio_channel_convert(chn, &val, sample);

        //        set_plot_items(info->plotType,iio_channel_get_id(chn),info->listIndex,val,info->offset,format->is_signed);

        if (format->is_signed)
            (*info->data_ref)[info->offset++] = (gfloat) val;
        else
            (*info->data_ref)[info->offset++] = (gfloat) (uint16_t)val;

    } else {
        int32_t val;
        iio_channel_convert(chn, &val, sample);
        if (format->is_signed)
            (*info->data_ref)[info->offset++]= (gfloat) val;
        else
            (*info->data_ref)[info->offset++]= (gfloat) (uint32_t)val;
    }

    return size;
}

/**
 * @brief apply_trigger_offset
 * @param chn
 * @param offset
 */
static void apply_trigger_offset(const struct iio_channel *chn, off_t offset)
{
    if (offset)
    {
        struct extra_info *info =(extra_info *) iio_channel_get_data(chn);

        memmove(info->data_ref, (const char *)info->data_ref + offset,
                info->offset * sizeof(gfloat) - offset);
    }
}

static void do_fft_for_spectrum(Transform *tr)
{
    //    struct _freq_spectrum_settings *settings =(_freq_spectrum_settings *) tr->settings;
    //    struct _fft_alg_data *fft = &settings->ffts_alg_data[settings->fft_index];
    //    struct marker_type *markers = settings->markers;
    //    enum marker_types marker_type = MARKER_OFF;
    //    int fft_clip_size = settings->fft_upper_clipping_limit -
    //                settings->fft_lower_clipping_limit;
    //    gfloat *in_data = settings->real_source;
    //    gfloat *in_data_c = settings->imag_source;
    //    gfloat *out_data = tr->y_axis + (settings->fft_index * fft_clip_size);
    //    int fft_size = settings->fft_size;
    //    int i, j, k, m;
    //    int cnt;
    //    gfloat mag;
    //    double avg, pwr_offset;
    //    gfloat plugin_fft_corr;
    //    unsigned int *maxX = settings->maxXaxis;
    //    gfloat *maxY = settings->maxYaxis;

    //    if (settings->marker_type)
    //        marker_type = *((enum marker_types *)settings->marker_type);

    //    if ((fft->cached_fft_size == -1) || (fft->cached_fft_size != fft_size) ||
    //        (fft->cached_num_active_channels != fft->num_active_channels)) {

    //        if (fft->cached_fft_size != -1) {
    //            fftw_destroy_plan(fft->plan_forward);
    //            fftw_free(fft->win);
    //            fftw_free(fft->out);
    //            if (fft->in != NULL)
    //                fftw_free(fft->in);
    //            if (fft->in_c != NULL)
    //                fftw_free(fft->in_c);
    //            fft->in_c = NULL;
    //            fft->in = NULL;
    //        }

    //        fft->win =(double*) fftw_malloc(sizeof(double) * fft_size);
    //        fft->m = fft_size;
    //        fft->in_c =(double (*)[2]) fftw_malloc(sizeof(fftw_complex) * fft_size);
    //        fft->in = NULL;
    //        fft->out =(double (*)[2]) fftw_malloc(sizeof(fftw_complex) * (fft->m + 1));
    //        fft->plan_forward = fftw_plan_dft_1d(fft_size, fft->in_c, fft->out, FFTW_FORWARD, FFTW_ESTIMATE);

    //        for (i = 0; i < fft_size; i ++)
    //            fft->win[i] = window_function(settings->fft_win, i, fft_size);

    //        fft->cached_fft_size = fft_size;
    //        fft->cached_num_active_channels = fft->num_active_channels;
    //    }

    //    for (cnt = 0, i = 0; cnt < fft_size; cnt++) {
    //        /* normalization and scaling see fft_corr */
    ////        fft->in_c[cnt] = in_data[i] * fft->win[cnt] + I * in_data_c[i] * fft->win[cnt];
    //        i++;
    //    }

    struct iio_device *iio_dev = transform_get_device_parent(tr);
    struct extra_dev_info *dev_info =(extra_dev_info *) iio_device_get_data(iio_dev);
    //    plugin_fft_corr = dev_info->plugin_fft_corr;

    //    fftw_execute(fft->plan_forward);
    //    avg = (double)settings->fft_avg;
    //    if (avg && avg != 128 )
    //        avg = 1.0f / avg;

    //    pwr_offset = settings->fft_pwr_off;

    //    for (i = 0, k = 0; i < fft->m; ++i) {
    //        if ((unsigned)i < settings->fft_lower_clipping_limit || (unsigned)i >= settings->fft_upper_clipping_limit)
    //            continue;
    //        if (i < (fft->m / 2))
    //            j = i + (fft->m / 2);
    //        else
    //            j = i - (fft->m / 2);

    ////        if (creal(fft->out[j]) == 0 && cimag(fft->out[j]) == 0)
    ////            fft->out[j] = FLT_MIN + I * FLT_MIN;

    ////        mag = 10 * log10((creal(fft->out[j]) * creal(fft->out[j]) +
    ////                cimag(fft->out[j]) * cimag(fft->out[j])) / ((unsigned long long)fft->m * fft->m)) +
    ////            settings->fft_corr + pwr_offset + plugin_fft_corr;

    //        /* it's better for performance to have separate loops,
    //         * rather than do these tests inside the loop, but it makes
    //         * the code harder to understand... Oh well...
    //         ***/
    //        if (out_data[k] == FLT_MAX) {
    //            /* Don't average the first iteration */
    //             out_data[k] = mag;
    //        } else if (!avg) {
    //            /* keep peaks */
    //            if (out_data[k] <= mag)
    //                out_data[k] = mag;
    //        } else if (avg == 128) {
    //            /* keep min */
    //            if (out_data[k] >= mag)
    //                out_data[k] = mag;
    //        } else {
    //            /* do an average */
    //            out_data[k] = ((1 - avg) * out_data[k]) + (avg * mag);
    //        }

    //        if (MAX_MARKERS && marker_type == MARKER_PEAK) {
    //            if (settings->fft_index == 0 && k <= 2) {
    //                maxX[0] = 0;
    //                maxY[0] = out_data[0];
    //            } else {
    //                for (j = 0; j <= MAX_MARKERS && markers[j].active; j++) {
    //                    if  ((*(out_data + k - 1) > maxY[j]) &&
    //                        ((!((*(out_data + k - 2) > *(out_data + k - 1)) &&
    //                         (*(out_data + k - 1) > *(out_data + k)))) &&
    //                         (!((*(out_data + k - 2) < *(out_data + k - 1)) &&
    //                         (*(out_data + k - 1) < *(out_data + k)))))) {

    //                        for (m = MAX_MARKERS; m > j; m--) {
    //                            maxY[m] = maxY[m - 1];
    //                            maxX[m] = maxX[m - 1];
    //                        }
    //                        maxY[j] = *(out_data + k - 1);
    //                        maxX[j] = k + (settings->fft_index * fft_clip_size) - 1;
    //                        break;
    //                    }
    //                }
    //            }
    //        }

    //        k++;
    //    }

}

/* sections of the xcorr function are borrowed (under the GPL) from
 * http://blog.dmaggot.org/2010/06/cross-correlation-using-fftw3/
 * which is copyright 2010 David E. Narváez
 */
void xcorr(fftw_complex *signala, fftw_complex *signalb, fftw_complex *result, int N, double avg)
{
    fftw_complex * signala_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * signalb_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outa = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outb = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex scale;
    fftw_complex *cross;

    int i;
    double peak_a = 0.0, peak_b = 0.0;

    if (!signala_ext || !signalb_ext || !outa || !outb || !out)
        return;

    if (avg > 1)
        cross = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (2 * N));
    else
        cross = result;

    fftw_plan pa = fftw_plan_dft_1d(2 * N - 1, signala_ext, outa, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan pb = fftw_plan_dft_1d(2 * N - 1, signalb_ext, outb, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan px = fftw_plan_dft_1d(2 * N - 1, out, cross, FFTW_BACKWARD, FFTW_ESTIMATE);

    //zeropadding
    memset(signala_ext, 0, sizeof(fftw_complex) * (N - 1));
    memcpy(signala_ext + (N - 1), signala, sizeof(fftw_complex) * N);
    memcpy(signalb_ext, signalb, sizeof(fftw_complex) * N);
    memset(signalb_ext + N, 0, sizeof(fftw_complex) * (N - 1));

    /* find the peaks of the time domain, for normalization */
    //    	for (i = 0; i < N; i++) {
    //    		if (peak_a < cabs(signala[i]))
    //    			peak_a = cabs(signala[i]);

    //    		if (peak_b < cabs(signalb[i]))
    //    			peak_b = cabs(signalb[i]);
    //    	}

    /* Move the two signals into the fourier domain */
    fftw_execute(pa);
    fftw_execute(pb);

    /* Compute the dot product, and scale them */
    //	scale = (2 * N -1) * peak_a * peak_b * 2;
    //	for (i = 0; i < 2 * N - 1; i++)
    //		out[i] = outa[i] * conj(outb[i]) / scale;

    /* Inverse FFT on the dot product */
    fftw_execute(px);

    fftw_destroy_plan(pa);
    fftw_destroy_plan(pb);
    fftw_destroy_plan(px);

    fftw_free(signala_ext);
    fftw_free(signalb_ext);
    fftw_free(out);
    fftw_free(outa);
    fftw_free(outb);

    if(avg > 1) {
        //		if (result[0] == FLT_MAX) {
        //			for (i = 0; i < 2 * N -1; i++)
        //				result[i] = cross[i];
        //		} else {
        //			for (i = 0; i < 2 * N -1; i++)
        //				result[i] = (result[i] * (avg - 1) + cross[i]) / avg;
        //		}
        fftw_free(cross);
    }

    fftw_cleanup();

    return;
}

static gdouble prefix2scale (char adc_scale)
{
    switch (adc_scale) {
    case 'M':
        return 1000000.0;
    case 'k':
        return 1000.0;
    default:
        return 1.0;
        break;
    }
}

static struct osc_plugin * get_plugin_from_name(const char *name)
{
    GSList *node;

    GSList *plugin_list = NULL;
    for (node = plugin_list; node; node = g_slist_next(node)) {
        struct osc_plugin *plugin =(osc_plugin *) node->data;
        if (plugin && !strcmp(plugin->name, name))
            return plugin;
    }

    return NULL;
}

bool plugin_installed(const char *name)
{
    return !!get_plugin_from_name(name);
}

/*
 * Fill in an array, of about num times
 */
static void fill_axis(gfloat *buf, gfloat start, gfloat inc, int num)
{
    int i;
    gfloat val = start;

    for (i = 0; i < num; i++) {
        buf[i] = val;
        val += inc;
    }

}

static bool device_is_oneshot(struct iio_device *dev)
{
    const char *name = iio_device_get_name(dev);

    if (strncmp(name, "ad-mc-", 5) == 0)
        return true;

    return false;
}

static void disable_all_channels(struct iio_device *dev)
{
    unsigned int i, nb_channels = iio_device_get_channels_count(dev);
    for (i = 0; i < nb_channels; i++)
        iio_channel_disable(iio_device_get_channel(dev, i));
}

void Plot::close_active_buffers(void)
{
    unsigned int i;

    for (i = 0; i < num_devices; i++) {
        struct iio_device *dev = iio_context_get_device(ctx, i);
        struct extra_dev_info *info =(extra_dev_info *) iio_device_get_data(dev);
        if (info->buffer) {
            iio_buffer_destroy(info->buffer);
            info->buffer = NULL;
        }

        disable_all_channels(dev);
    }
}

void Plot::stop_sampling(void)
{
    stop_capture = TRUE;
    close_active_buffers();
    //	G_TRYLOCK(buffer_full);
    //	G_UNLOCK(buffer_full);
}

static off_t get_trigger_offset(const struct iio_channel *chn,
                                bool falling_edge, float trigger_value)
{
    struct extra_info *info =(extra_info *) iio_channel_get_data(chn);
    size_t i;

    //    if (iio_channel_is_enabled(chn)) {
    //        for (i = info->offset / 2; i >= 1; i--) {
    //            if (!falling_edge && info->data_ref[i - 1] < trigger_value &&
    //                    info->data_ref[i] >= trigger_value)
    //                return i * sizeof(gfloat);
    //            if (falling_edge && info->data_ref[i - 1] >= trigger_value &&
    //                    info->data_ref[i] < trigger_value)
    //                return i * sizeof(gfloat);
    //        }
    //    }
    return 0;
}

static struct iio_device *plot_iio_channel_get_iio_parent(PlotChn *obj)
{
    PlotIioChn *channel = (PlotIioChn *)obj;
    struct iio_device *iio_dev = NULL;
    struct extra_info *ch_info;

    if (channel && channel->iio_chn) {
        ch_info =(extra_info *) iio_channel_get_data(channel->iio_chn);
        if (ch_info)
            iio_dev = ch_info->dev;
    }

    return iio_dev;
}

static QVector<double>* plot_iio_channel_get_data_ref(PlotChn *obj)
{
    PlotIioChn *channel = reinterpret_cast<PlotIioChn*>(obj);
    QVector<double> *ref = NULL;

    if (channel && channel->iio_chn) {
        struct extra_info *ch_info;

        ch_info =(extra_info *)(iio_channel_get_data(channel->iio_chn));
        if (ch_info)
        {
            ref = ch_info->data_ref;
        }
    }

    return ref;
}

static void set_channel_shadow_of_enabled(gpointer data, gpointer user_data)
{
    struct iio_channel *chn =static_cast<struct iio_channel *>(data);
    struct extra_info *ch_info;

    if (!chn)
        return;

    ch_info =static_cast<extra_info *>(iio_channel_get_data(chn));
    ch_info->lo_freq=0;
    ch_info->shadow_of_enabled = ((bool)user_data) ? 1 : -1;
}

static void plot_iio_channel_assert_channels(PlotChn *obj, bool assert)
{
    PlotIioChn *channel =reinterpret_cast<PlotIioChn*>(obj);

    if (channel && channel->iio_chn)
        set_channel_shadow_of_enabled(channel->iio_chn,
                                      (gpointer)assert);
}

static void plot_iio_channel_destroy(PlotChn *obj)
{
    PlotIioChn *channel = reinterpret_cast<PlotIioChn*>(obj);

    if (!channel)
        return;

    if (channel->base.name)
        g_free(channel->base.name);

    if (channel->base.parent_name)
        g_free(channel->base.parent_name);

    free(channel);
}

static PlotIioChn * plot_iio_channel_new()
{
    PlotIioChn *obj;

    obj =reinterpret_cast<PlotIioChn*>(calloc(sizeof(PlotIioChn), 1));
    if (!obj) {
        fprintf(stderr, "Error in %s: %s", __func__, strerror(errno));
        return NULL;
    }

    obj->base.type = PLOT_IIO_CHANNEL;
    //    obj->base.ctx = ctx;
    obj->base.get_iio_parent = *plot_iio_channel_get_iio_parent;
    obj->base.get_data_ref = *plot_iio_channel_get_data_ref;
    obj->base.assert_used_iio_channels = *plot_iio_channel_assert_channels;
    obj->base.destroy = *plot_iio_channel_destroy;

    return obj;
}

static bool show_channel(struct iio_channel *chn)
{
    const char *id = iio_channel_get_id(chn);

    if (iio_channel_is_output(chn) || !strcmp(id, "timestamp"))
        return false;
    else
        return iio_channel_is_scan_element(chn);
}

#pragma endregion }

#pragma region Time Domain Plot Operations {

void Plot::getMax(Transform *tr){
    max=-999999;
    for (int i=0;i<(*tr->y_axis).size() ;i++ ) {
        if((*tr->y_axis)[i]>max)
            max=(*tr->y_axis)[i];
    }
}

void Plot::getMin(Transform *tr){
    min=999999;
    for (int i=0;i<(*tr->y_axis).size() ;i++ ) {
        if((*tr->y_axis)[i]<min)
            min=(*tr->y_axis)[i];
    }
}




void Plot::saveYAxisToFile(const QVector<double>& y_axis_i, const QVector<double>& y_axis_q,const QString& filename) {
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "TEXT" << endl;
        int i{};

        while ( i < y_axis_i.size())
        {
            out << y_axis_i[i]<<"," << y_axis_q[i] <<endl;
            i++;
        }

        file.close();
    }
    else
    {
        return;
    }
}

/**
 * @brief Plot::DrawPlot
 */
void Plot::DrawPlot(){

    ui->lblAdrv9009Temp->setText(QString::number(globals::temp9009)+" °C");
    ui->lblAd7291Temp->setText(QString::number(globals::temp7291)+" °C");

    if(ui->timeChart->isVisible() || Mode == SMART_TIME_DOMAIN)
    {
        foreach (Transform *transform, *priv->transform_list) {
            {
                // ---------------------------------- [saeid raziani] ----------------------------------
                //this section is for sending time domain to card in smart noise, in order to send it we need to save it in a file then send it
                if(Mode == SMART_TIME_DOMAIN)
                {                    //when the vector fill, the timer save it and then send it to the card
                    timeDataYaxis_i = *(*priv->transform_list)[0]->y_axis;
                    timeDataYaxis_q = *(*priv->transform_list)[1]->y_axis;
                }

                //--------------------------------------------------------------------------------------------------
                //--------------------------------------------------------------------------------------------------
                //--------------------------------------------------------------------------------------------------

                // First clear time domain chart
                transform->graph->data().clear();

                if(ui->cmb_graph_type->currentIndex()==0)
                {
                    transform->graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 0));
                    transform->graph->setLineStyle(QCPGraph::lsLine);
                }

                else
                {
                    transform->graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
                    transform->graph->setLineStyle(QCPGraph::lsNone);
                }

                if(transform->x_axis->size()!= transform->y_axis->size())
                    return;

                transform->graph->setData(*transform->x_axis, *transform->y_axis);

                transform->graph->setSelectable(QCP::SelectionType::stDataRange);
                transform->graph->rescaleAxes();


                if(ui->chk_auto_scale->isChecked())
                {
                    getMax(transform);
                    getMin(transform);

                    max+=(abs(max)+abs(min))/4;
                    min-=(abs(max)+abs(min))/4;

                    ui->txt_y_axis_min->setValue(min);
                    ui->txt_y_axis_max->setValue(max);
                }
            }

            if(!selectTimeChart)
                ui->timeChart->yAxis->setRange(ui->txt_y_axis_min->value(),ui->txt_y_axis_max->value());
            else
                reScale_plot(1);

            ui->timeChart->replot();

        }
    }

    if(ui->cmb_plot_type->currentIndex()==1 || ui->cmb_plot_type->currentIndex()==2)
    {
        QString name=objectName();
        DrawFFTPlot();
    }
}

#pragma endregion }

#pragma region Frequency Domain Plot Operations {

void Plot::getFFTMax(Transform *tr,bool is_seek){
    fftMaxIndexes.clear();
    fftMax=-99999;

    if(!is_seek)
        for (int i=1;i<(*tr->y_axis).size() ;i++ )
        {
            if((*tr->y_axis)[i]>fftMax)
            {
                fftMax=(*tr->y_axis)[i];
                fftMaxIndex=i;
            }
        }
    else
        for (int i=0;i<(*tr->seek_y_axis).size() ;i++ ) {
            if((*tr->seek_y_axis)[i]>fftMax)
            {
                fftMax=(*tr->seek_y_axis)[i];
                fftMaxIndex=i;
            }
        }


    fftMaxIndexes.append(fftMaxIndex);

    int t=0;
    //while(fftMaxIndexes.size()<fftMarkersCount)
    //{
    //fftMax=-99999;
    //for (int i=0;i<(*tr->y_axis).size() ;i++ ) {
    //if((*tr->y_axis)[i]>fftMax)
    //{
    // bool f=false;

    if(!is_seek)
        while(fftMaxIndexes.size()<fftMarkersCount)
        {
            fftMax=-99999;
            for (int i=0;i<(*tr->y_axis).size() ;i++ ) {
                if((*tr->y_axis)[i]>fftMax)
                {
                    bool f=false;

                    for(int j=0;j<fftMaxIndexes.size();j++)
                    {

                        t=abs((*tr->y_axis)[i]-(*tr->y_axis)[fftMaxIndexes[j]]);
                        if(i==fftMaxIndexes[j] || t<10)
                        {
                            f=true;
                            break;
                        }
                    }

                    if(!f)
                    {
                        fftMax=(*tr->y_axis)[i];
                        fftMaxIndex=i;
                    }
                }
            }

            fftMaxIndexes.append(fftMaxIndex);
        }

    else
        while(fftMaxIndexes.size()<fftMarkersCount)
        {
            fftMax=-99999;
            for (int i=0;i<(*tr->seek_y_axis).size() ;i++ ) {
                if((*tr->seek_y_axis)[i]>fftMax)
                {
                    bool f=false;

                    for(int j=0;j<fftMaxIndexes.size();j++)
                    {
                        t=abs((*tr->seek_y_axis)[i]-(*tr->seek_y_axis)[fftMaxIndexes[j]]);
                        if(i==fftMaxIndexes[j] || t<12)
                        {
                            f=true;
                            break;
                        }
                    }

                    if(!f)
                    {
                        fftMax=(*tr->seek_y_axis)[i];
                        fftMaxIndex=i;
                    }
                }
            }
            fftMaxIndexes.append(fftMaxIndex);
        }

    //    fftMaxIndexes.append(fftMaxIndex);
    //}

}

void Plot::getFFTMin(Transform *tr,bool is_seek){
    fftMin=99999999;

    if(!is_seek)
        for (int i=0;i<(*tr->y_axis).size() ;i++ ) {
            if((*tr->y_axis)[i]<fftMin)
                fftMin=(*tr->y_axis)[i];
        }
    else
        for (int i=0;i<(*tr->seek_y_axis).size() ;i++ ) {
            if((*tr->seek_y_axis)[i]<fftMin)
                fftMin=(*tr->seek_y_axis)[i];
        }

}

void Plot::DrawFFTPlot()
{

    //        double d= abs(DC_6_UPTO_8_12 -baseFreq )- 250;
    waterfallChart->axes(Qt::AlignBottom).at(0)->setRange((abs(DC_6_UPTO_8_12 -baseFreq ) -250),(abs(DC_6_UPTO_8_12- baseFreq) + 250));

    int sizeData ;

    // Frequency Domain
    if(ui->cmb_plot_type->currentIndex()==1)
    {
        QCustomSeries a;
        QVector2D v2d;
        v2d.setX(0);
        v2d.setY(0);
        a.addPoints(v2d);

        QString name=objectName();

        foreach (Transform *transform, *priv->transform_list)
        {

            if(ui->cmb_graph_type->currentIndex()==0)
            {
                transform->fftgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 0));
                transform->fftgraph->setLineStyle(QCPGraph::lsLine);
            }

            else
            {
                transform->fftgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
                transform->fftgraph->setLineStyle(QCPGraph::lsNone);
            }


            QVector<double> vector;

            // Keep the frequency axis tied to the *current* base frequency.
            // It was filled once at transform init, so the spectrum diagram
            // never updated when the frequency changed.
            for(int i{}; i < transform->x_axis->size() ;i++)
            {
                (*transform->x_axis)[i]= baseFreq - 245.759999 + i*0.029999 ;
            }

            for(int i{}; i < transform->x_axis->size() ;i++)
            {
                vector.append(abs(DC_6_UPTO_8_12 - (*transform->x_axis)[i])) ;
            }


            //replacing 5 first and 5 last items since they are noise values
            if(transform->y_axis->size()>0)
            {
                (*transform->y_axis)[0]= (*transform->y_axis)[5];
                (*transform->y_axis)[1]= (*transform->y_axis)[5];
                (*transform->y_axis)[2]= (*transform->y_axis)[5];

                (*transform->y_axis)[transform->y_axis->size()-1] = (*transform->y_axis)[transform->y_axis->size()-5];
                (*transform->y_axis)[transform->y_axis->size()-2] = (*transform->y_axis)[transform->y_axis->size()-5];
                (*transform->y_axis)[transform->y_axis->size()-3] = (*transform->y_axis)[transform->y_axis->size()-5];
            }


            transform->fftgraph->data().clear();
            transform->fftgraph->setData(vector,*transform->y_axis);


            // ---------------------------------- [saeid raziani] ----------------------------------
            //this section is for sending time domain to card in smart noise, in order to send it we need to save it in a file then send it
            //            if(Mode == SMART_TIME_DOMAIN)
            //            {                    //when the vector fill, the timer save it and then send it to the card
            //                //                    timeDataYaxis = *transform->y_axis;
            //                timeDataYaxis_i = *(*priv->transform_list)[0]->y_axis;
            //                timeDataYaxis_q = *(*priv->transform_list)[1]->y_axis;
            //            }

            //--------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------


            //saeid raziani
            //==================== maxhold====================================
            if(maxHold_X.size()==0) maxHold_X = vector;
            if(maxHold_Y.size()==0) maxHold_Y = transform->y_axis[0];

            //==================waterfall=======================================
            for(int i{}; i < vector.size();i++)
            {
                if (maxHold_X[i] < vector[i]) maxHold_X[i] = vector[i];
                if (maxHold_Y[i] < transform->y_axis[0][i]) maxHold_Y[i] = transform->y_axis[0][i];

                //                if(DC_6_UPTO_8_12 !=0)
                //                {
                v2d.setX(vector[i]);
                //                }

                //                else
                //                {
                //                    v2d.setX(vector[vector.size()-1-i]);
                //                }

                v2d.setY(transform->y_axis[0][i]);
                a.addPoints(v2d);
                //                vector.clear();
            }

            emit(updateData(a));


            //=========================================================

            if(ui->chk_maxHold->isChecked())
            {
                ui->fftChart->graph(1)->setData(maxHold_X,maxHold_Y);
            }

            sizeData = transform->x_axis->size();
            transform->fftgraph->setSelectable(QCP::SelectionType::stDataRange);
            transform->fftgraph->rescaleAxes();
        }




        ui->txt_markers->clear();
        ui->fftChart->clearItems();

        if(fftMaxIndexes.count()>0)
        {
            if(priv->transform_list->size() != 0)
            {
                globals::peakValueDb = (*(*priv->transform_list)[0]->y_axis)[fftMaxIndexes[0]];
                globals::peakValue = (*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[0]];
            }
        }


        //saeid raziani == plotting text on chart
        //        QCPItemText *txtLbl = new QCPItemText(ui->fftChart);
        txtLbl = new QCPItemText(ui->fftChart);

        txtLbl->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
        txtLbl->position->setType(QCPItemPosition::ptAxisRectRatio);
        txtLbl->position->setCoords(0.02, 0);
        txtLbl->setFont(QFont(font().family(), 12));
        txtLbl->setColor(Qt::yellow);
        txtLbl->setText("");


        // saeid raziani ===========================
        for(int i=0; i<fftMaxIndexes.size(); i++)
        {
            if(priv->transform_list->isEmpty())
            {

                return;
            }

            if(abs((*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[i]] - tmpVal) < 10)
            {
                tmpVal = (*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[i]];
                fftMaxIndexes.remove(i);
            }
        }

        //showing number of peak in spectrom plot

        QVector<double> frqVal;
        QVector<double> powerVal;
        QVector<long long int>timeNow;
        QVector<QString> tableMode;
        int tmpInt{};

        bool isFrqEqual = false;






        //======================[ calibration ]========================================
        if(isCalibAllowed)
        {
            double po{};
            double   fr1{};
            double   fr2{-1};


            for(int i=0; i<fftMaxIndexes.size(); i++)
            {
                fr1 = abs(DC_6_UPTO_8_12 - (*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[i]]);
                if( abs(fr1 - frqCalib) < 1)
                {
                    po = (*(*priv->transform_list)[0]->y_axis)[fftMaxIndexes[i]];
                    if(po >maxPower )
                    {
                        fr2 = fr1;
                        maxPower = po;
                    }
                }
            }
            emit getMaxValueSignal(fr2,maxPower);
        }


        maxPower = -600;
        //==========================================================================







        for(int i=0; i<fftMaxIndexes.size(); i++)
        {
            //comparing frq value with frq values in the table. we need the ones that arent equal
            //=====================================cw table===================================================================
            powerFrq = (*(*priv->transform_list)[0]->y_axis)[fftMaxIndexes[i]];
            frqValue = abs(DC_6_UPTO_8_12 - (*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[i]]);


            ui->label_12->setText(QString::number(maxPower));
            ui->label_13->setText(QString::number(powerFrq));

            //            if(int(frqCalib) == int(frqValue) and maxPower <= powerFrq )
            //            {
            //                emit getMaxValueSignal(frqValue,powerFrq);
            //                maxPower=-600;
            //            }
            //


            if(isMaxVectorFilled)
            {
                frqMaxCalib.append(frqValue);
                powerMaxCalib.append(powerFrq);
            }


            //=======================================================================================================
            //accepted values are those that havent equal value in table and their level are greather than the level that is entered by the user
            isFrqEqual = false;

            if(ui->spnLevelFFTChart->value() < powerFrq)
            {
                frqVal.append(frqValue);
                powerVal.append(powerFrq);
                tableMode.append("");
                timeNow.append(QDateTime::currentMSecsSinceEpoch());
            }

            //=======================================================================================================

            if (ui->cmbNumberPeak->currentIndex() == 0) continue;

            //saeid raziani =========================================================================================
            if((ui->cmbNumberPeak->currentIndex()-1 < i ) || (ui->spnLevelFFTChart->value() > powerFrq)) continue;

            double diffFrq = abs(DC_6_UPTO_8_12 - (*(*priv->transform_list)[0]->x_axis)[fftMaxIndexes[0]]) - frqValue;
            if(i > 0 and diffFrq < ui->dspnDeltabu->value()) continue;

            //-----------------------------------------------------------------
            QCPItemTracer *phaseTracer = new QCPItemTracer(ui->fftChart);

            //itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
            phaseTracer->setGraph(ui->fftChart->graph(0));

            if(priv->transform_list->size() != 0) phaseTracer->setGraphKey(frqValue);

            phaseTracer->setInterpolating(true);
            phaseTracer->setStyle(QCPItemTracer::tsSquare);
            phaseTracer->setPen(QPen(Qt::red));
            phaseTracer->setBrush(Qt::red);
            phaseTracer->setSize(10);
            //-----------------------------------------------------------------





            //-----------------------------------------------------------------------------------------------
            int mul=-3;

            if(i%2==0) mul=3;
            // add the text label at the top:
            QCPItemText * textLabel = new QCPItemText(ui->fftChart);
            //            textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
            //            textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
            if(priv->transform_list->size() != 0) textLabel->position->setCoords(frqValue+mul, powerFrq+mul); // place position at center/top of axis rect

            textLabel->setText("p"+QString::number(i)+": "+  QString::number(powerFrq));
            textLabel->setFont(QFont(font().family(), 14));
            textLabel->setPen(QPen(Qt::yellow));
            textLabel->setColor(Qt::yellow);
            //-----------------------------------------------------------------------------------------------





            //-----------------------------------------------------------------------------------------------
            // add the arrow:
            QCPItemLine *arrow = new QCPItemLine(ui->fftChart);
            arrow->start->setParentAnchor(textLabel->bottom);
            if(priv->transform_list->size() != 0)
            {
                arrow->end->setCoords(frqValue, powerFrq); // point to (4, 1.6) in x-y-plot coordinates
            }
            //            arrow->setHead(QCPLineEnding::esSpikeArrow);
            arrow->setPen(QPen(Qt::white));

            if(priv->transform_list->size() != 0)
            {

                QString pVal ="P"+QString::number(i)+": " +
                        QString::number(powerFrq)+" dBFS" +
                        "    "+QString::number(frqValue)+" MHz";


                if(!selectingNoiseIsActive) txtLbl->setText (txtLbl->text()+"\n "+pVal);

                ui->txt_markers->appendPlainText("P"+QString::number(i)+": "+
                                                 QString::number(powerFrq)+" dBFS"+
                                                 " @"+QString::number(frqValue)+" MHz\n");

            }
            //-----------------------------------------------------------------------------------------------
        }



        //===================================================================
        if(frqVal.size() > 0)
        {
            for(int i{}; i < frqVal.size(); i++)
            {


                if(i != 0)
                {
                    double frqDiff   = abs (int(frqVal[0]) - int(frqVal[i]));
                    if( frqDiff <= ui->dspnDeltabu->value()) frqVal[i]=0;
                }

                //                frqTwo.append(frqVal[i]);
                //                frqPower.append(powerVal[i]);
                //                tbFrqMode.append("");
            }

            for(int i{}; i < tbFrqMode.size(); i++)
            {
                //                if( i )
                //                tableMode[i] = tbFrqMode[i].isNull()? "" : tbFrqMode[i];
            }

            emit cwTableValueSignal(frqVal,powerVal,tableMode,timeNow);
        }

        frqVal.clear();
        powerVal.clear();
        tableMode.clear();

        maxPower = -600;
        //====================================================================
    }





    // Seek Frequency
    else if(ui->cmb_plot_type->currentIndex()==2)
    {
        foreach (Transform *transform, *priv->transform_list)
        {
            if(transform->seek_x_axis!=NULL)
            {
                if(ui->cmb_graph_type->currentIndex()==0)
                {
                    transform->fftgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 0));
                    transform->fftgraph->setLineStyle(QCPGraph::lsLine);
                }

                else
                {
                    transform->fftgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
                    transform->fftgraph->setLineStyle(QCPGraph::lsNone);
                }

                QVector<double> vector;

                for(int i{}; i < transform->x_axis->size() ;i++)
                {
                    vector.append(abs(DC_6_UPTO_8_12 - (*transform->x_axis)[i]));
                }
                transform->fftgraph->data().clear();
                transform->fftgraph->setData(*transform->seek_x_axis,*transform->seek_y_axis);
                transform->fftgraph->setSelectable(QCP::SelectionType::stDataRange);
                transform->fftgraph->rescaleAxes();
            }
        }

        ui->txt_markers->clear();

        ui->fftChart->clearItems();

        if(fftMaxIndexes.count()>0)
        {
            if(priv->transform_list->size() != 0)
            {
                globals::peakValueDb = (*(*priv->transform_list)[0]->seek_y_axis)[fftMaxIndexes[0]];
                globals::peakValue = (*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[0]];
            }
        }



        //saeid raziani == plotting text on chart
        QCPItemText *txtLbl = new QCPItemText(ui->fftChart);
        txtLbl->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
        txtLbl->position->setType(QCPItemPosition::ptAxisRectRatio);
        txtLbl->position->setCoords(0.02, 0);
        txtLbl->setFont(QFont(font().family(), 12));
        txtLbl->setColor(Qt::yellow);
        txtLbl->setText("");




        // saeid raziani ===========================
        for(int i=0; i<fftMaxIndexes.size(); i++)
        {
            if(priv->transform_list->isEmpty())
            {

                return;
            }

            if(abs((*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[i]] - tmpVal) < 10)
            {
                tmpVal = (*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[i]];
                fftMaxIndexes.remove(i);
            }
        }

        //showing number of peak in spectrom plot

        QVector<double> frqVal;
        QVector<double> powerVal;
        QVector<long long int> timeNow;
        QVector<QString> tableMode;
        int tmpInt{};

        bool isFrqEqual = false;





        for(int i=0;i<fftMaxIndexes.size();i++)
        {

            //comparing frq value with frq values in the table. we need the ones that arent equal
            //=====================================cw table===================================================================
            double powerFrq = (*(*priv->transform_list)[0]->seek_y_axis)[fftMaxIndexes[i]];
            double frqValue = abs(DC_6_UPTO_8_12 - (*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[i]]);


            //            if(frqTwo.size()<5)
            //            {
            //                frqTwo.append(frqValue);
            //                frqPower.append(powerFrq);
            //            }


            //=======================================================================================================
            //accepted values are those that havent equal value in table and their level are greather than the level that is entered by the user
            isFrqEqual = false;

            if(ui->spnLevelFFTChart->value() < powerFrq)
            {
                frqVal.append(frqValue);
                powerVal.append(powerFrq);
                tableMode.append("");
                timeNow.append(QDateTime::currentMSecsSinceEpoch());
            }

            //=======================================================================================================







            if (ui->cmbNumberPeak->currentIndex() == 0) continue;

            //saeid raziani =========================================================================================
            if((ui->cmbNumberPeak->currentIndex()-1 < i ) || (ui->spnLevelFFTChart->value() > powerFrq)) continue;

            double diffFrq = abs(DC_6_UPTO_8_12 - (*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[0]]) - frqValue;
            if(i > 0 and diffFrq < ui->dspnDeltabu->value()) continue;

            //-----------------------------------------------------------------
            QCPItemTracer *phaseTracer = new QCPItemTracer(ui->fftChart);

            //itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
            phaseTracer->setGraph(ui->fftChart->graph(0));

            if(priv->transform_list->size() != 0) phaseTracer->setGraphKey(frqValue);

            phaseTracer->setInterpolating(true);
            phaseTracer->setStyle(QCPItemTracer::tsSquare);
            phaseTracer->setPen(QPen(Qt::red));
            phaseTracer->setBrush(Qt::red);
            phaseTracer->setSize(10);


            //-----------------------------------------------------------------------------------------------
            int mul=-3;

            if(i%2==0) mul=3;
            // add the text label at the top:
            QCPItemText * textLabel = new QCPItemText(ui->fftChart);
            //            textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
            //            textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
            if(priv->transform_list->size() != 0) textLabel->position->setCoords(frqValue+mul, powerFrq+mul); // place position at center/top of axis rect

            textLabel->setText("p"+QString::number(i)+": "+  QString::number(powerFrq));
            textLabel->setFont(QFont(font().family(), 14));
            textLabel->setPen(QPen(Qt::yellow));
            textLabel->setColor(Qt::yellow);


            //-----------------------------------------------------------------------------------------------
            // add the arrow:
            QCPItemLine *arrow = new QCPItemLine(ui->fftChart);
            arrow->start->setParentAnchor(textLabel->bottom);
            if(priv->transform_list->size() != 0)
            {
                arrow->end->setCoords(frqValue, powerFrq); // point to (4, 1.6) in x-y-plot coordinates
            }
            //            arrow->setHead(QCPLineEnding::esSpikeArrow);
            arrow->setPen(QPen(Qt::white));

            if(priv->transform_list->size() != 0)
            {

                //                if(cntWait >  40 or cntWait == 0)
                //                {
                //                    cntWait = 0;
                //                    holdPreX[i] = (*(*priv->transform_list)[0]->seek_x_axis)[fftMaxIndexes[i]];
                //                    holdPreY[i] = (*(*priv->transform_list)[0]->seek_y_axis)[fftMaxIndexes[i]];
                //                }

                //                cntWait++;

                QString pVal ="P"+QString::number(i)+": " +
                        QString::number(powerFrq)+" dBFS" +
                        "    "+QString::number(frqValue)+" MHz";


                if(!selectingNoiseIsActive) txtLbl->setText (txtLbl->text()+"\n "+pVal);

                ui->txt_markers->appendPlainText("P"+QString::number(i)+": "+
                                                 QString::number(powerFrq)+" dBFS"+
                                                 " @"+QString::number(frqValue)+" MHz\n");

            }

            //==================================================================================================
            //==================================================================================================
            //==================================================================================================
        }

        //===================================================================
        if(frqVal.size() > 0)
        {
            for(int i{}; i < frqVal.size(); i++)
            {
                if(i != 0)
                {
                    double frqDiff   = abs (int(frqVal[0]) - int(frqVal[i]));
                    if( frqDiff <= ui->dspnDeltabu->value()) frqVal[i]=0;
                }

                //                frqTwo.append(frqVal[i]);
                //                frqPower.append(powerVal[i]);
                //                tbFrqMode.append("");
            }

            for(int i{}; i < tbFrqMode.size(); i++)
            {
                //                if( i )
                //                tableMode[i] = tbFrqMode[i].isNull()? "" : tbFrqMode[i];
            }

            emit cwTableValueSignal(frqVal,powerVal,tableMode,timeNow);
        }

        frqVal.clear();
        powerVal.clear();
        tableMode.clear();
        //====================================================================

    }













    if(ui->chk_auto_scale_fft->isChecked())
    {
        if( ui->fftChart->yAxis->range().lower>fftMin && abs(ui->fftChart->yAxis->range().lower-fftMin)>20  &&
                ui->fftChart->yAxis->range().upper<fftMax && abs(ui->fftChart->yAxis->range().upper-fftMax)>20 )
        {
            ui->txt_y_axis_min_fft->setValue(fftMin);
            ui->txt_y_axis_max_fft->setValue(fftMax);
            ui->fftChart->yAxis->setRange(fftMin,fftMax);
        }
    }

    if(!selectFFTChart)
    {
        ui->fftChart->yAxis->setRange(ui->txt_y_axis_min_fft->value(),ui->txt_y_axis_max_fft->value());
    }

    else
    {
        //        reScale_plot(2);

        ui->fftChart->xAxis->setRange(rangeXFFTChart.lower, rangeXFFTChart.upper);
        ui->fftChart->yAxis->setRange(rangeYFFTChart.lower, rangeYFFTChart.upper);
    }

    ui->fftChart->replot();



}

#pragma endregion }

#pragma region Menu Actions {

void Plot::on_actionSaveAs_triggered()
{
    QPixmap pixmap(ui->timeChart->rect().size());
    ui->timeChart->render(&pixmap, QPoint(), QRegion(ui->timeChart->rect()));
    QFileDialog dialogFile(this);
    dialogFile.setDefaultSuffix("png");
    QString filename=dialogFile.getSaveFileName(this, QString::fromUtf8("Save file - Time Chart."), tr("timeChart.png"), tr(".png"));
    pixmap.save(filename);

}

void Plot::on_actionSave_Whole_Page_triggered()
{
    QPixmap pixmap(ui->centralwidget->rect().size());
    ui->centralwidget->render(&pixmap, QPoint(), QRegion(ui->centralwidget->rect()));
    QFileDialog dialogFile(this);
    dialogFile.setDefaultSuffix("png");
    QString filename=dialogFile.getSaveFileName(this, QString::fromUtf8("Save file - Chart."), tr("chart.png"), tr(".png"));
    pixmap.save(filename);
}

void Plot::on_actionClose_triggered()
{
    this->close();
}

void Plot::on_actionQuit_triggered()
{
    QApplication::quit();
}

void Plot::on_actionPlot_Title_triggered()
{
}

void Plot::on_actionShow_Settings_triggered()
{
    if(ui->settingsMenu->isVisible())
        ui->settingsMenu->setVisible(false);
    else
        ui->settingsMenu->setVisible(true);
}

void Plot::on_actionFull_Screen_triggered()
{
    if(this->isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

#pragma endregion }

#pragma region Initialize{

/**
 * @brief set plots settings
 */






void Plot::checkBoxUnched(){

    for(int i{}; i < ui->timeDataTable->rowCount(); i++)
    {
        QCheckBox *checkBox = qobject_cast<QCheckBox*>(ui->timeDataTable->cellWidget(i, 12));
        if (checkBox and checkBox->isChecked())
        {
            checkBox->setChecked(false);
        }
    }
}
//saeid raziani
void Plot::fillingTable()
{
    static QDateTime timeStamp;

    int cntTable{};


    if(prePulse.size() > 0)
    {
        for (int i=0; i< prePulse.size(); i++)
        {
            int index = 0;

            //====================================================================================
            //saeid raziani ==> all this signals are sent to main receiver in order to plot them
            //            QString * a1 = new QString (QString::number(prePulse.at(i).Freq,'g',10));
            emit ValueSignal(prePulse.at(i));

            for(int j{}; j < ui->timeDataTable->rowCount(); j++)
            {
                duplicate =false;


                //---------------------------- check same channel ---------------------------------
                QTableWidgetItem *t = ui->timeDataTable->item(j, 4);

                if(t)
                {
                    double v1 = t->text().toDouble();
                    //                    double v2 = prePulse.at(i).Channel;

                    double v2 = selectChannelWithFrq(prePulse.at(i).Freq);
                    if(v1 == v2)
                    {
                        index = j;
                        duplicate = true;
                        break;
                    }
                }
                //-------------------------------------------------------------------------------






                //------------------------- remove same frq -----------------------------------
                double frqOld = std::round(ui->timeDataTable->item(i,2)->text().toDouble());
                double frqNew = std::round(abs(DC_6_UPTO_8_12-prePulse.at(i).Freq2));
                double val = std::abs(frqOld - frqNew);

                int cntNew = prePulse.at(0).Count;
                int cntOld = ui->timeDataTable->item(i,11)->text().toInt();

                if(val <= 2 and cntNew > cntOld)
                {
                    ui->timeDataTable->removeRow(j);
                    if(ui->timeDataTable->rowCount() == 0) break;
                }
                //-----------------------------------------------------------------------------------


            }

            if(!duplicate)
            {
                index = ui->timeDataTable->rowCount();
                ui->timeDataTable->setRowCount(ui->timeDataTable->rowCount() +1);


                //=======================================================================================================
                //----------------- adding check box to each row - smart noise-------------------------------------------
                //======================================================================================================

                QCheckBox * checkBox = new QCheckBox;
                ui->timeDataTable->setCellWidget(index, 12, checkBox);


                double channelNumber = prePulse.at(i).Channel;

                if (!tableData.empty()  and tableData.at(3) == channelNumber)
                {
                    checkBox->setCheckState(Qt::Checked);
                }

                connect(checkBox,  &QCheckBox::stateChanged,[=](int state)
                {
                    //                    emit checkBoxStatus(state);

                    if(checkBox->isChecked())
                    {
                        for(int i{}; i < ui->timeDataTable->rowCount(); i++)
                        {
                            QCheckBox *checkBox = qobject_cast<QCheckBox*>(ui->timeDataTable->cellWidget(i, 12));
                            if (checkBox and checkBox->isChecked())
                            {
                                checkBox->setChecked(false);
                            }
                        }


                        tableData.clear();
                        checkBox->setChecked(true);

                        //TO DO

                        tableData.push_back(abs(DC_6_UPTO_8_12-ui->timeDataTable->item(index,1)->text().toDouble())); //frq
                        tableData.push_back(ui->timeDataTable->item(index,2)->text().toDouble());//frq peak
                        tableData.push_back(ui->timeDataTable->item(index,3)->text().toDouble());//pw
                        tableData.push_back(ui->timeDataTable->item(index,4)->text().toDouble());//channel

                        emit sendChannelToCardSignal(ui->timeDataTable->item(index,4)->text().toDouble());
                        emit ampValueSignal(tableData.at(2));
                    }


                    else
                    {
                        emit sendChannelToCardSignal(-1); // -1 means there is no channel
                        tableData.clear();
                    }
                });

                //=========================================================================================================
                //---------------------------------------------------------------------------------------------------------
                //=========================================================================================================


                ui->timeDataTable->setColumnWidth(2, 500);
            }

            //            qInfo()<<"===========> : "<< prePulse.at(i).Freq;
            //Mode
            QTableWidgetItem *pCell1 = ui->timeDataTable->item(index, 0);
            //Freq
            QTableWidgetItem *pCell2 = ui->timeDataTable->item(index, 1);
            //Freq 2
            QTableWidgetItem *pCell3 = ui->timeDataTable->item(index, 2);
            //Db 2
            QTableWidgetItem *pCell4 = ui->timeDataTable->item(index, 3);

            //Channel
            QTableWidgetItem *pCell5 = ui->timeDataTable->item(index, 4);
            //PRI
            QTableWidgetItem *pCell6 = ui->timeDataTable->item(index, 5);
            //PW1
            QTableWidgetItem *pCell7 = ui->timeDataTable->item(index, 6);
            //PW2
            QTableWidgetItem *pCell8 = ui->timeDataTable->item(index, 7);
            //PAPeak
            QTableWidgetItem *pCell9 = ui->timeDataTable->item(index, 8);
            //PASum
            QTableWidgetItem *pCell10 = ui->timeDataTable->item(index, 9);
            // TOA
            QTableWidgetItem *pCell11 = ui->timeDataTable->item(index, 10);
            //Count
            QTableWidgetItem *pCell12 = ui->timeDataTable->item(index, 11);



            if(!pCell1)
            {
                pCell1 = new QTableWidgetItem;
                pCell1->setTextAlignment(Qt::AlignCenter);
                //                pCell1->setBackgroundColor(QColor(255, 0, 0, 127));
                ui->timeDataTable->setItem(index, 0, pCell1);

            }

            if(!pCell2)
            {
                pCell2 = new QTableWidgetItem;
                pCell2->setTextAlignment(Qt::AlignCenter);

                //if mode was CW, this column in table shouldnt show any thing , this way we fill it with a line
                if (prePulse.at(i).Mode == 0) pCell2->setText("-");

                ui->timeDataTable->setItem(index, 1, pCell2);
            }

            if(!pCell3)
            {
                pCell3 = new QTableWidgetItem;
                pCell3->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 2, pCell3);
            }

            if(!pCell4)
            {
                pCell4 = new QTableWidgetItem;
                pCell4->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 3, pCell4);
            }

            if(!pCell5)
            {
                pCell5 = new QTableWidgetItem;
                pCell5->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 4, pCell5);
            }

            if(!pCell6)
            {
                pCell6 = new QTableWidgetItem;
                pCell6->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 5, pCell6);
            }

            if(!pCell7)
            {
                pCell7 = new QTableWidgetItem;
                pCell7->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 6, pCell7);
            }

            if(!pCell8)
            {
                pCell8 = new QTableWidgetItem;
                pCell8->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 7, pCell8);
            }

            if(!pCell9)
            {
                pCell9 = new QTableWidgetItem;
                pCell9->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 8, pCell9);
            }

            if(!pCell10)
            {
                pCell10 = new QTableWidgetItem;
                pCell10->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 9, pCell10);
            }

            if(!pCell11)
            {
                pCell11 = new QTableWidgetItem;
                pCell11->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 10, pCell11);
            }

            if(!pCell12)
            {
                pCell12 = new QTableWidgetItem;
                pCell12->setTextAlignment(Qt::AlignCenter);
                ui->timeDataTable->setItem(index, 11, pCell12);
            }





            //---------------------[ show on table ]---------------------------------
            cntTable++;
            if(cntTable<1000) {
                cntTable=0;
                int indx{};

                pCell1->setText(prePulse.at(i).Mode==0?"CW":prePulse.at(i).Mode==1?"Pulse":"GSM");

                pCell2->setText(QString::number(abs(DC_6_UPTO_8_12-prePulse.at(i).Freq),'g',10));


                //----------------------------------FRQ 2 ----------------------------------------------
                double frqOne = abs(DC_6_UPTO_8_12-prePulse.at(i).Freq);
                bool isEq{false};
                for(int x{}; x<cwFrqValue->size(); x++)
                {
                    if(abs((*cwFrqValue)[x] - frqOne) < ui->dspnDeltabu->value())
                    {
                        if(cwModeValue->size()>0) (*cwModeValue)[x] = pCell1->text();
                        indx = x;
                        isEq = true;
                        break;
                    }
                }

                //pCell3->setText(QString::number (isEq ? abs(DC_6_UPTO_8_12-prePulse.at(i).Freq2) : frqTwo.at(i) ,'g',10) );
                pCell3->setText(isEq ? QString::number ((*cwFrqValue)[indx] ,'g',10) : "Invalid" );


                //-----------------------------------------------------------------------------------

                //            double frqOne = abs(DC_6_UPTO_8_12-prePulse.at(i).Freq);

                //                for(int i=0; i<frqTwo.size(); i++)
                //                {
                //                    for(int j{}; j < ui->timeDataTable->rowCount(); j++)
                //                    {
                //                        QTableWidgetItem * frqTable = ui->timeDataTable->item(j, 1);
                //                        double frqOne = frqTable->text().toDouble();


                //                        if(abs(frqOne -  frqTwo.at(i)) < 4)
                //                        {
                //                            pCell3->setText(QString::number (frqTwo.at(i),'g',10));

                //                        }
                //                        else
                //                        {
                //                            pCell3->setText("Invalid");
                //                        }
                //                    }

                //                    frqTwo.clear();
                //                    frqPower.clear();
                //                double powerFrq = (*(*priv->transform_list)[0]->y_axis)[fftMaxIndexes[i]];

                //                }
                //                tbFrqValue.append(pCell3->text().toDouble());
                //            pCell3->setText(QString::number (abs(DC_6_UPTO_8_12-prePulse.at(i).Freq2),'g',10));

                //            if(  prePulse.at(i).PAPeak < ui->spnLevelFFTChart->value()){
                //                pCell3->setText("-");

                //            }


                //            if(prePulse.at(i).Mode==0)
                //            {
                //                pCell2->setText(QString::number(abs(DC_6_UPTO_8_12-prePulse.at(i).Freq2),'g',10));
                //            }
                //--------------------------------------------------------------------------------------

                pCell4->setText(QString::number (prePulse.at(i).Db2,'g',10));

                pCell5->setText(QString::number (selectChannelWithFrq(prePulse.at(i).Freq)));
                //            pCell5->setText(QString::number (prePulse.at(i).Freq));

                //                size_t t = toaChannel1[prePulse.at(i).Channel].size();
                //                unsigned long int toaSum{};
                //                if (t > 10)
                //                {
                //                    for(auto & val : toaChannel1[prePulse.at(i).Channel])
                //                    {
                //                        toaSum += val;
                //                    }
                //                }
                double v = prePulse.at(i).diffToa/2;
                pCell6->setText(QString::number (v,'f',3));
                pCell7->setText(QString::number (prePulse.at(i).PW1,'g',3));
                pCell8->setText(QString::number (prePulse.at(i).PW2,'g',3));

                //------------------------------------------------------------
                pCell9->setText(isEq ? QString::number ((*cwPowerValue)[indx] ,'g',10) : "Invalid" );
                //                pCell9->setText(QString::number (prePulse.at(i).PAPeak,'g',10));
                //------------------------------------------------------------------------

                pCell10->setText(QString::number(prePulse.at(i).PASum,'g',10));
                timeStamp.setTime_t(prePulse.at(i).TOA);
                pCell11->setText(timeStamp.toString(Qt::SystemLocaleShortDate)+" ("+QString::number(prePulse.at(i).TOA,'g',10)+")");
                int l=prePulse.at(i).Channel;
                pCell12->setText(QString::number(countArray[l] > 100000000 ? countArray[l]=0 : countArray[l]));
                //--------------------------------------------------------------------------------

                frqPower.clear();
                frqTwo.clear();
                tbFrqMode.clear();
            }

            //adding update time of each row
            QTime currentTime = QTime::currentTime();
            timeMap[index]= currentTime.toString("ss");
            prePulse.clear();
        }

        //        tbFrqValue.clear();

        //it fills an array that we want to use its value to comparing with cw table.lower value than delta shoudnot show
        for(int j{}; j < ui->timeDataTable->rowCount(); j++)
        {
            //            tbFrqValue.append(ui->timeDataTable->item(j,2)->text().toDouble());
            //            tbFrqMode.append( ui->timeDataTable->item(j,0)->text());
        }

    }
}




int Plot::selectChannelWithFrq(double val)
{
    double step{15};
    double  df{};
    int channel{0};



    if (val > abs(DC_6_UPTO_8_12 - baseFreq) )
    {

        df  = val - abs(DC_6_UPTO_8_12 - baseFreq);

        if(df <= (step/2))
        {
            channel =0;
        }

        else if(df > (step/2) and df < (step +step/2))
        {

            channel = 1;
        }

        else if(df >= (step + step/2) and df < (2*step +step/2))
        {

            channel = 2;
        }

        else if(df >= (2*step + step/2) and df < (3*step +step/2) )
        {

            channel = 3;
        }

        else if(df >= (3*step + step/2) and df < (4*step +step/2))
        {

            channel = 4;
        }

        else if(df >= (4*step + step/2) and df < (5*step +step/2))
        {

            channel = 5;
        }

        else if(df >= (5*step + step/2) and df < (6*step +step/2))
        {

            channel = 6;
        }

        else if(df >= (6*step + step/2) and df < (7*step +step/2))
        {

            channel = 7;
        }

        else if(df >= (7*step + step/2) and df < (8*step +step/2))
        {

            channel = 8;
        }

        else if(df >= (8*step + step/2) and df < (9*step +step/2))
        {

            channel = 9;
        }

        else if(df >= (9*step + step/2) and df < (10*step +step/2))
        {

            channel = 10;
        }

        else if(df >= (10*step + step/2) and df < (11*step +step/2))
        {

            channel = 11;
        }

        else if(df >= (11*step + step/2) and df < (12*step +step/2))
        {

            channel = 12;
        }

        else if(df >= (12*step + step/2) and df < (13*step +step/2))
        {

            channel = 13;
        }

        else if(df >= (13*step + step/2) and df < (14*step +step/2))
        {

            channel = 14;
        }

        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {

            channel = 15;
        }


    }

    else
    {
        df =  abs(DC_6_UPTO_8_12 - baseFreq )- val;

        if(df <= (step/2))
        {
            channel =0;
        }

        else if(df > (step/2) and df < (step +step/2))
        {

            channel = 31;

        }



        else if(df >= (step + step/2) and df < (2*step +step/2))
        {

            channel = 30;
        }

        else if(df >= (2*step + step/2) and df < (3*step +step/2) )
        {

            channel = 29;
        }

        else if(df >= (3*step + step/2) and df < (4*step +step/2))
        {

            channel = 28;
        }

        else if(df >= (4*step + step/2) and df < (5*step +step/2))
        {

            channel = 27;
        }

        else if(df >= (5*step + step/2) and df < (6*step +step/2))
        {

            channel = 26;
        }

        else if(df >= (6*step + step/2) and df < (7*step +step/2))
        {

            channel = 25;
        }

        else if(df >= (7*step + step/2) and df < (8*step +step/2))
        {

            channel = 24;
        }

        else if(df >= (8*step + step/2) and df < (9*step +step/2))
        {

            channel = 23;
        }

        else if(df >= (9*step + step/2) and df < (10*step +step/2))
        {

            channel = 22;
        }

        else if(df >= (10*step + step/2) and df < (11*step +step/2))
        {

            channel = 21;
        }

        else if(df >= (11*step + step/2) and df < (12*step +step/2))
        {

            channel = 20;
        }

        else if(df >= (12*step + step/2) and df < (13*step +step/2))
        {
            channel = 19;
        }

        else if(df >= (13*step + step/2) and df < (14*step +step/2))
        {
            channel = 18;
        }

        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {
            channel = 17;
        }


        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {
            channel = 16;
        }


    }


    return channel;

}



void Plot::calculatingPRI(Pulse * pls, double channel)
{
    cnt ++;
    long double toa{pls->TOA};

    size_t toaSize   = toaChannel1[channel].size();
    size_t deltaSize = deltaTime[channel].size();


    if(toaSize > 0)
    {

        long double endVal = toaChannel1[channel].back();
        long int s= static_cast<long int>(endVal);
        long int diff = static_cast<long int>(toa)  - static_cast<long int>(endVal);

        if( diff  < 80000)
        {
            if (deltaSize > 0)
            {
                if (abs(diff - deltaTime[channel].back()) < 1600)
                {
                    toaChannel1[channel].push_back(toa);
                    deltaTime[channel].push_back(diff);
                }

                else
                {
                    toaChannel1[channel].clear();
                    deltaTime[channel].clear();
                }
            }

            else
            {
                toaChannel1[channel].push_back(toa);
                deltaTime[channel].push_back(diff);
            }
        }

        else
        {
            toaChannel1[channel].clear();
            deltaTime[channel].clear();
        }

    }
    //10732831705.6925462084
    else
    {
        toaChannel1.at(channel).push_back(toa);
    }
}

void Plot::removeUnupdatedRow()
{
    for(int i{}; i < ui->timeDataTable->rowCount(); i++)
    {


        QTime currentTime = QTime::currentTime();

        uint currTime = currentTime.toString("ss").toUInt();


        if (timeMap.contains(i))
        {
            uint rowDataTome=timeMap.value(i).toUInt();
            if(currTime - rowDataTome > 2)
            {
                //                ui->timeDataTable->removeCellWidget(i, 12);
                ui->timeDataTable->removeRow(i);
                if(ui->timeDataTable->rowCount()== 0){
                    duplicate = false;
                }
            }
        }
    }
}


void Plot::create_plot()
{

#pragma region Detector Thread {

    detector=new Detector();
    ui->timeDataTable->setShowGrid(true);
    ui->timeDataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QObject::connect(detector,&Detector::newList,[=](QList<Pulse *> *pulseList)
    {
        //        for (int i=0; i< pulseList->size(); i++)
        //        {
        //            calculatingPRI(pulseList->at(i),pulseList->at(i)->Channel);
        //            prePulse->append(pulseList->at(i));
        //        }
    });


    //saeid raziani
    QObject::connect(detector,&Detector::showList,[=](QList<Pulse *> *pls)
    {
        int l = pls->at(0)->Channel;
        countArray[l] +=100;
        prePulse.append(*pls->at(0));
    });



#pragma endregion }

#pragma region Time domain {


    // Initialize time domain chart colors and settings
    ui->timeChart->setBackground(QBrush(Qt::black));
    ui->timeChart->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->timeChart->xAxis->setBasePen(QPen(QColor(255,255,255)));
    ui->timeChart->yAxis->setBasePen(QPen(QColor(255,255,255)));
    ui->timeChart->xAxis->setTickPen(QPen(QColor(255,255,255)));
    ui->timeChart->yAxis->setTickPen(QPen(QColor(255,255,255)));
    ui->timeChart->xAxis->setTickLabelColor(QColor(255,255,255));
    ui->timeChart->yAxis->setTickLabelColor(QColor(255,255,255));
    ui->timeChart->yAxis->setSelectedLabelColor(QColor(255,255,255));
    ui->timeChart->yAxis->setSelectedTickLabelColor(QColor(255,255,255));
    ui->timeChart->xAxis->grid()->setVisible(false);
    ui->timeChart->yAxis->grid()->setVisible(false);
    ui->timeChart->setSelectionRectMode(QCP::srmSelect);
    ui->timeChart->setSelectionTolerance(100);

    // When user select an area
    QObject::connect(ui->timeChart,&QCustomPlot::selectionChangedByUser,[=](){
        //        selectTimeChart=true;
        //        QCPSelectionRect *rectTimeChart = ui->timeChart->selectionRect();




        //        originalColor = rectTimeChart->brush().color();
        //        QColor newColor = QColor(255, 124, 0);
        //        rectTimeChart->setBrush(QBrush(newColor));


        //        rangeXTimeChart = rectTimeChart->range(ui->timeChart->xAxis);
        //        rangeYTimeChart = rectTimeChart->range(ui->timeChart->yAxis);

        //        reScale_plot(1);
    });

    // When user use mouse wheel
    QObject::connect(ui->timeChart,&QCustomPlot::mouseWheel,[=](QWheelEvent *event){

        if(event->delta()>0)
        {
            //            rangeXTimeChart.lower += 20;
            //            rangeYTimeChart.lower += 20;
            //            rangeXTimeChart.upper -= 20;
            //            rangeYTimeChart.upper -= 20;
        }

        else
        {
            //            rangeXTimeChart.lower -= 20;
            //            rangeYTimeChart.lower -= 20;
            //            rangeXTimeChart.upper += 20;
            //            rangeYTimeChart.upper += 20;
        }
    });

#pragma endregion }

#pragma region Frequcny domain {

    // Initialize frequency domain chart colors and settings
    ui->fftChart->xAxis->grid()->setVisible(false);
    ui->fftChart->yAxis->grid()->setVisible(false);
    ui->fftChart->setBackground(QBrush(Qt::black));
    ui->fftChart->xAxis->setBasePen(QPen(QColor(255,255,255)));
    ui->fftChart->yAxis->setBasePen(QPen(QColor(255,255,255)));
    ui->fftChart->xAxis->setTickPen(QPen(QColor(255,255,255)));
    ui->fftChart->yAxis->setTickPen(QPen(QColor(255,255,255)));
    ui->fftChart->xAxis->setTickLabelColor(QColor(255,255,255));
    ui->fftChart->yAxis->setTickLabelColor(QColor(255,255,255));
    ui->fftChart->yAxis->setSelectedLabelColor(QColor(255,255,255));
    ui->fftChart->yAxis->setSelectedTickLabelColor(QColor(255,255,255));
    ui->fftChart->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->fftChart->setSelectionRectMode(QCP::srmSelect);
    ui->fftChart->setSelectionTolerance(100);

    // When user select an area
    QObject::connect(ui->fftChart,&QCustomPlot::selectionChangedByUser,[=]()
    {

        QCPSelectionRect *rectFFTChart = ui->fftChart->selectionRect();
        rangeXFFTChart = rectFFTChart->range(ui->fftChart->xAxis);

        if(isBandRejectOn)
        {
            emit selectedFrqMinMaxSignal(rangeXFFTChart.lower, rangeXFFTChart.upper);
            return;
        }

        else if (isSmartNoisetOn)
        {
            //            selectFFTChart=true;
            reScale_plot(2);
        }

        else
        {
            selectFFTChart=true;

            rangeXFFTChart = rectFFTChart->range(ui->fftChart->xAxis);
            rangeYFFTChart = rectFFTChart->range(ui->fftChart->yAxis);

            //            ui->fftChart->xAxis->setRange(rangeXFFTChart.lower, rangeXFFTChart.upper);
            //            ui->fftChart->yAxis->setRange(rangeYFFTChart.lower, rangeYFFTChart.upper);
            //            ui->fftChart->replot();
        }

        //        rangeYFFTChart = rectFFTChart->range(ui->fftChart->yAxis);

    });

    // When user select an area
    QObject::connect(ui->fftChart,&QCustomPlot::mouseMove,[=](){

        //        selectFFTChart=true;
        //        QCPSelectionRect *rectFFTChart = ui->fftChart->selectionRect();

        //        rangeXFFTChart = rectFFTChart->range(ui->fftChart->xAxis);
        //        rangeYFFTChart = rectFFTChart->range(ui->fftChart->yAxis);
        //        reScale_plot(2);

    });

    // When user use mouse wheel
    QObject::connect(ui->fftChart,&QCustomPlot::mouseWheel,[=](QWheelEvent *event){

        if(event->delta()>0)
        {
            //            rangeXFFTChart.lower += 20;
            //            rangeYFFTChart.lower += 20;
            //            rangeXFFTChart.upper -= 20;
            //            rangeYFFTChart.upper -= 20;
        }
        else
        {
            //            rangeXFFTChart.lower -= 20;
            //            rangeYFFTChart.lower -= 20;
            //            rangeXFFTChart.upper += 20;
            //            rangeYFFTChart.upper += 20;
        }
    });

    // When user click on mouse buttons
    QObject::connect(ui->fftChart,&QCustomPlot::mousePress,[=](QMouseEvent *event){

        if(event->button()==Qt::RightButton){

            if(fftMenu==nullptr)
            {
                fftMenu=new QMenu(this);
                fftMenu->addAction(tr("Add Marker"));
                fftMenu->addAction(tr("Remove Marker"));
                fftMenu->addAction(tr("Peak Markers"));
                fftMenu->actions()[2]->setCheckable(true);
                fftMenu->actions()[2]->setChecked(true);

                QObject::connect(fftMenu->actions()[0],&QAction::triggered,[=](){
                    fftMarkersCount++;
                });

                QObject::connect(fftMenu->actions()[1],&QAction::triggered,[=](){
                    fftMarkersCount--;
                });

                QObject::connect(fftMenu->actions()[2],&QAction::triggered,[=](){

                    if(fftMarkersCount>0)
                    {
                        fftMarkersCount=0;
                        fftMenu->actions()[2]->setChecked(false);
                    }
                    else
                    {
                        fftMarkersCount=5;
                        fftMenu->actions()[2]->setChecked(true);
                    }
                });
            }

            QPoint p=event->pos();
            QPoint p2=ui->fftChart->pos();

            p.setX(p.x()+ui->settingsMenu->width());
            p.setY(p.y()+p2.y());

            fftMenu->popup(mapToGlobal(p));

            ui->fftChart->setSelectionRectMode(QCP::srmNone);
        }
        else
            ui->fftChart->setSelectionRectMode(QCP::srmSelect);
    });

#pragma endregion }

    char buf[50];

    priv->tbuf = NULL;
    priv->ch_settings_list = NULL;

    /* Count every object that is being created */
    object_count++;
    priv->object_id = object_count;

    // Set a different title for every plot
    snprintf(buf, sizeof(buf), "ADI IIO Oscilloscope - Capture%d", priv->object_id);
    this->setWindowTitle (buf);

    setObjectName(QString::number(priv->object_id));

    // An instance of DataTypes Class
    DataTypes dataTypes;

    // Create a empty list of transforms
    priv->transform_list = dataTypes.TrList_new();

    // No active transforms by default
    priv->active_transform_type = NO_TRANSFORM_TYPE;

    // Initialize device treeView
    device_list_treeview_init();

    saveas_channels_list_fill();

    /* Initialize text view for Devices Info */
    priv->devices_buf = new QTextDocument();
    ui->txt_devices->setPlainText (priv->devices_buf->toPlainText ());

    /* Initialize text view for Phase Info */
    priv->phase_buf = new QTextDocument();
    ui->txt_phase->setPlainText (priv->phase_buf->toPlainText ());

    priv->sample_count = 400;

    ui->sample_count_widget->setValue (priv->sample_count);

    // Check if current selections are valid
    check_valid_setup();

    //    g_mutex_init(&priv->g_marker_copy_lock);

    device_rx_info_update();

    // Set current plot type
    ui->cmb_plot_type->setCurrentIndex (TIME_PLOT);
}


/**
 * @brief Initialize plots
 */
void Plot::InitializePlots()
{
    // Hold count of checked channels
    //plotCount=0;

    // counting checked channels
    //foreach(ChannelItemData *item,ChannelItemDataList)
    //if(item->isChecked)
    //plotCount++;

    // Clear all arrays
    //    while (!seekxItemList.empty()) {seekxItemList.removeFirst();}
    //    while (!seekyItemList.empty()) {seekyItemList.removeFirst();}

    // Get fftsize from UI
    fftSize=ui->cmb_fft_size->currentText().toInt();

    freqStep=(RFBandwidthOBS*1.25)/(fftSize*2);

    // Clear charts
    //    ui->timeChart->clearGraphs();
    //        ui->fftChart->clearGraphs();
    //        ui->fftChart->clearPlottables();

    int i=0;
}

/**
 * @brief Change Frequency by hopping
 * @param freq : next hop frequency
 */
void Plot::ChangeFrequencyHopping(iio_channel *ch1,double freq){

    if (!ch1) {
        qWarning() << "ChangeFrequencyHopping: channel is null";
        return;
    }

    freq*=1000000.0;

    iio_channel_attr_write_bool(ch1,"frequency_hopping_mode_enable",true);

    for(int i=0;i<2;i++)
        iio_channel_attr_write_longlong(ch1,"frequency_hopping_mode", freq);

}

/**
 * @brief Get current frequency
 * @return freqChanged status
 */
bool Plot::GetBaseFreq()
{
    //    qInfo() << "GetBaseFreq" ;
    struct iio_device *dev;
    struct iio_channel *ch1,*ch2,*ch3;
    const char *freq_name;

    if (!ctx) {
        qWarning() << "GetBaseFreq: IIO context is null";
        return freqChanged;
    }

    dev = iio_context_find_device(ctx, PHY_DEVICE);
    if (!dev) {
        qWarning() << "GetBaseFreq: PHY device not found:" << PHY_DEVICE;
        return freqChanged;
    }

    ch1 = iio_device_find_channel(dev, "altvoltage0", true);
    if (!ch1) {
        qWarning() << "GetBaseFreq: altvoltage0 output channel not found";
        return freqChanged;
    }

    if (iio_channel_find_attr(ch1, "frequency"))
        freq_name = "frequency";
    else
        freq_name = "RX_LO_frequency";


    //    qInfo() << "========================= START ===========================" ;
    ssize_t ret;
    char buf[0x100];
    char buf1[0x100];
    char buf2[0x100];


    //    qInfo() << "buff1 " << buf;


    if (ch1)
    {
        //        qInfo() << "first if";
        ret = iio_channel_attr_read(ch1,freq_name, buf, sizeof(buf));
    };

    char *end;
    char *end1;
    char *end2;


    baseFreq = g_ascii_strtod(buf, &end);
    baseFreq/=1000000.0 ;
    //    y += 500;
    //    baseFreq +=  y;
    //    qInfo() << "buff1 " << baseFreq;

    //    qInfo() << "buff1 " << buf;


    ch2 = iio_device_find_channel(dev, "voltage0", false);
    ch3 = iio_device_find_channel(dev, "voltage2", false);

    if (ch2)
        //        qInfo() <<  "second if";
        ret = iio_channel_attr_read(ch2,"rf_bandwidth", buf1, sizeof(buf1));
    RFBandwidth = g_ascii_strtod(buf1, &end1);
    RFBandwidth/=1000000.0;

    if (ch3)
        //        qInfo() <<  " third if";
        ret = iio_channel_attr_read(ch3,"rf_bandwidth", buf2, sizeof(buf2));
    RFBandwidthOBS = g_ascii_strtod(buf2, &end2);
    RFBandwidthOBS/=1000000.0;
    //    qInfo() <<  " third if";

    // Update Ui if base frequecny has changed
    if(ui->lblFreqValue->text().toDouble()==baseFreq)
    {
        //        qInfo() << "for" ;
        freqChanged=false;
    }

    else
    {
        //        qInfo() << "-----------------------------------------------else" ;
        ui->lblFreqValue->setText(QString::number(abs(DC_6_UPTO_8_12-baseFreq)));

        frqValueStr =  ui->lblFreqValue->text();
        // Move the spectrum window with the base frequency
        double frqWin = abs(DC_6_UPTO_8_12 - baseFreq);
        ui->fftChart->xAxis->setRange(frqWin - 250, frqWin + 250);
        ui->fftChart->replot();

        //        waterfallChart->axes(Qt::AlignBottom).at(0)->setRange((baseFreq - 250),(baseFreq + 250));

        //    panoramaUpdateData->start();
        //    detectedAmpUpdateData->start();
        //    waterfallUpdateData->start();





        //        freqChanged=true;
    }

    //    qInfo() << "====================== END =========================="<<"\n";
    return freqChanged;
}
#pragma endregion }

#pragma region Capture Operations {

/**
 * @brief Plot::startCapture
 */
void Plot::startCapture(){

    button_state=true;
    ui->sample_count_widget->setEnabled(false);
    ui->tree_channel_list->setEnabled(false);
    ui->btn_capture->setToolTip ("Stop");
    ui->btn_capture->setIcon(QIcon(":/icon/stop"));
    plot_channels_update();
    collect_parameters_from_plot();
    remove_all_transforms();
    devices_transform_assignment();

    if (!priv || !priv->transform_list || priv->transform_list->isEmpty()) {
        qCritical() << "Capture start aborted: no valid transform/channel selection for" << Mode;
        button_state = false;
        isCaptureOn = false;
        ui->sample_count_widget->setEnabled(true);
        ui->tree_channel_list->setEnabled(true);
        return;
    }

    InitializePlots();
    start(true);
    plot_setup();

    if(ui->cmb_plot_type->currentIndex()==2 && priv->transform_list->size()>0)
        seekStart=true;

    priv->frame_counter = 0;
    priv->fps = 0.0;
    gettimeofday(&(priv->last_update), NULL);

    timerChart->start();
    timerChartSweep->start();
    if(Mode == SMART_TIME_DOMAIN)
    {
        sendTimeDataToCardTimer.start();
    }

    ui->cmbChannels->clear();
    foreach (Transform *tr, *priv->transform_list)
    {
        if (!tr || !tr->plot_channels || !tr->plot_channels->data)
            continue;
        PlotChn *plotChannel = PLOT_CHN(tr->plot_channels->data);
        if (plotChannel && plotChannel->name)
            ui->cmbChannels->addItem(plotChannel->name);
    }
}

/**
 * @brief Plot::stopCapture
 */
void Plot::stopCapture(){

    button_state=false;

    ui->sample_count_widget->setEnabled(true);
    ui->tree_channel_list->setEnabled(true);
    ui->btn_capture->setStyleSheet("background-color:red; color:white");
    ui->btnfftcapture->setStyleSheet("background-color:red; color:white");


    priv->stop_redraw = TRUE;

    if((ui->cmb_plot_type->currentIndex()==2 && !seekStart) || ui->cmb_plot_type->currentIndex()!=2)
    {
        dispose_parameters_from_plot();
        deassert_used_channels();
    }

    start(false);

    timerChart->stop();
    timerChartSweep->stop();
    if(Mode == SMART_TIME_DOMAIN) sendTimeDataToCardTimer.stop();
}

double Plot::getDeltaTime()
{
    return dltValue;
}


/**
 * @brief Plot::capture_setup
 * @return
 */
int Plot::capture_setup(void)
{
    int i, j;
    unsigned int min_timeout = 1000;
    unsigned int timeout;
    double freq;

    if (!ctx) {
        qCritical() << "Capture setup failed: IIO context is null";
        return -1;
    }

    // Get device count from context
    int num_devices = iio_context_get_devices_count(ctx);

    for (i = 0; i < num_devices; i++) {
        iio_device *dev = iio_context_get_device(ctx, i);
        if (!dev)
            continue;

        extra_dev_info *dev_info =(extra_dev_info *) iio_device_get_data(dev);
        if (!dev_info) {
            qWarning() << "Capture setup: missing device metadata at index" << i;
            continue;
        }

        int nb_channels = iio_device_get_channels_count(dev);
        int sample_size, sample_count = max_sample_count_from_plots(dev_info);

        /* We capture a double amount o data. Then we look for a trigger
                   condition in a half of this interval. This way, no matter where
                   the trigger sample is we have enough samples after the triggered
                   one to display on the plot. */
        if (dev_info->channel_trigger_enabled)
            sample_count *= 2;

        for (j = 0; j < nb_channels; j++) {
            struct iio_channel *ch = iio_device_get_channel(dev, j);
            if (!ch)
                continue;

            struct extra_info *info =(extra_info *) iio_channel_get_data(ch);
            if (!info) {
                qWarning() << "Capture setup: missing channel metadata for"
                           << (iio_channel_get_id(ch) ? iio_channel_get_id(ch) : "<unknown>");
                continue;
            }

            // Enable or disable channel by channel info
            if (info->shadow_of_enabled > 0)
                iio_channel_enable(ch);
            else
                iio_channel_disable(ch);
        }

        sample_size = iio_device_get_sample_size(dev);
        if (sample_size == 0 || sample_count == 0)
            continue;

        for (j = 0; j < nb_channels; j++) {

            iio_channel *ch = iio_device_get_channel(dev, j);
            if (!ch)
                continue;

            extra_info *info =(extra_info *)(iio_channel_get_data(ch));

            // Channel data_ref is a C++ QVector allocated with new.
            // Keep the same object alive across multiple Plot instances/capture restarts
            // so existing transforms never retain a dangling pointer.
            if (!info) {
                qWarning() << "Capture setup: missing channel metadata for"
                           << (iio_channel_get_id(ch) ? iio_channel_get_id(ch) : "<unknown>");
                continue;
            }

            if (!info->data_ref)
                info->data_ref = new QVector<double>();

            info->data_ref->resize(sample_count);
            info->data_ref->fill(0.0);
            info->sampleCount = sample_count;

            qInfo()<<"Address of p"<<j<<": \n"<< info->data_ref;
        }

        // If device buffer created before in cart then destory it
        if (dev_info->buffer)
            iio_buffer_destroy(dev_info->buffer);

        dev_info->buffer = NULL;
        dev_info->sample_count = sample_count;

        iio_device_set_data(dev, dev_info);

        freq = read_sampling_frequency(dev);

        if (freq > 0) {
            /* 2 x capture time + 1s */
            timeout = sample_count * 1000 / freq;
            if (dev_info->channel_trigger_enabled)
                timeout *= 2;
            timeout += 1000;
            if (timeout > min_timeout)
                min_timeout = timeout;
        }
    }

    if (ctx)
        iio_context_set_timeout(ctx, min_timeout);

    return 0;
}

/**
 * @brief Plot::start
 * @param start_event
 */
void Plot::start(gboolean start_event)
{

    if (start_event) {

        num_capturing_plots++;

        /* Stop the capture process to allow settings to be updated */
        stop_capture = TRUE;

        //G_TRYLOCK(buffer_full);

        /* Make sure the capture process in the Spectrum Analyzer plugin
                 * is not running */
        //        if (spect_analyzer_plugin)
        //            spect_analyzer_plugin->handle_external_request(spect_analyzer_plugin, "Stop");

        /* Start the capture process */
        capture_setup();
        capture_start();

        //restart_all_running_plots();
    } else {
        //G_TRYLOCK(buffer_full);
        //G_UNLOCK(buffer_full);

        //        ui->timeChart->clearGraphs();
        //                ui->fftChart->clearGraphs();
        //        ui->fftChart->clearPlottables();

        stop_capture = TRUE;

        num_capturing_plots--;
        if (num_capturing_plots == 0) {

            stop_capture = TRUE;
            close_active_buffers();
        }
    }
}

/**
 * @brief Plot::capture_process
 */
void Plot::capture_process()
{
    unsigned int i;
    //    qInfo() << "capture";

    if (!ctx) {
        qCritical() << "Capture process stopped: IIO context is null";
        return;
    }

    GetBaseFreq();
    num_devices = iio_context_get_devices_count(ctx);

    for (i = 0; i < num_devices; i++) {

        struct iio_device *dev = iio_context_get_device(ctx, i);
        if (!dev)
            continue;

        struct extra_dev_info *dev_info =(struct extra_dev_info *) iio_device_get_data(dev);
        if (!dev_info) {
            qWarning() << "Capture process: missing device metadata at index" << i;
            continue;
        }

        unsigned int i, sample_size = iio_device_get_sample_size(dev);
        unsigned int nb_channels = iio_device_get_channels_count(dev);
        ssize_t sample_count = dev_info->sample_count;
        struct iio_channel *chn;
        off_t offset = 0;

        if (dev_info->input_device == false)
            continue;

        if (sample_size == 0)
            continue;

        if (dev_info->buffer == NULL || device_is_oneshot(dev)) {

            dev_info->buffer_size = sample_count;

            dev_info->buffer = iio_device_create_buffer(dev,sample_count, false);

            if (!dev_info->buffer) {
                fprintf(stderr, "Error: Unable to create buffer: %s\n", strerror(errno));
                goto capture_stop_check;
            }

        }

        /* Reset the data offset for all channels */
        for (i = 0; i < nb_channels; i++) {
            struct iio_channel *ch = iio_device_get_channel(dev, i);
            if (!ch)
                continue;
            struct extra_info *info =(extra_info *) iio_channel_get_data(ch);
            if (!info || !info->data_ref)
                continue;
            info->offset = 0;
        }

        while (!stop_capture) {
            ssize_t ret = iio_buffer_refill(dev_info->buffer);
            if (ret < 0) {
                qInfo()<<stderr, "Error while reading data: %s\n", strerror(-ret);
                if (ret == -EPIPE) {
                    restart_capture = true;
                }
                stop_sampling();
                goto capture_stop_check;
            }

            ret /= iio_buffer_step(dev_info->buffer);
            if (ret >= sample_count) {

                iio_buffer_foreach_sample(
                            dev_info->buffer, demux_sample, NULL);

                if(detectorEnabled && !globals::hopping)
                {
                    //                                                            qInfo() << *(*priv->transform_list)[ui->cmbChannels->currentIndex()]->y_axis;
                    //                    qInfo()<< '\n';

                    detector->setTunerFrequency(baseFreq);
                    //                    qInfo() << "************************************************************************************";

                    if(priv->transform_list->size() != 0)
                    {
                        detector->AddToQueue(*(*priv->transform_list)[ui->cmbChannels->currentIndex()]->y_axis);

                    }

                }

                if (dev_info->buffer_size >= sample_count *2) {
                    qInfo()<<"Decreasing buffer size\n";
                    iio_buffer_destroy(dev_info->buffer);
                    dev_info->buffer_size /= 2;
                    dev_info->buffer = iio_device_create_buffer(dev,
                                                                dev_info->buffer_size, false);
                }
                break;
            }

            qInfo()<<"Increasing buffer size\n";
            iio_buffer_destroy(dev_info->buffer);
            dev_info->buffer_size *= 2;
            dev_info->buffer = iio_device_create_buffer(dev,
                                                        dev_info->buffer_size, false);
            if (!dev_info->buffer) {
                qCritical() << "Capture process: failed to recreate IIO buffer for"
                            << (iio_device_get_name(dev) ? iio_device_get_name(dev) : iio_device_get_id(dev));
                goto capture_stop_check;
            }
        }

        if (dev_info->channel_trigger_enabled) {
            chn = iio_device_get_channel(dev, dev_info->channel_trigger);
            if (!iio_channel_is_enabled(chn))
                dev_info->channel_trigger_enabled = false;
        }

        /* We find the sample that meets the trigger condition, then we grab samples
                   before and after it so that the trigger sample gets to be displayed in the
                   middle of the plot. When that's not possible, we display the sample as the
                   first sample of the plot. */
        if (dev_info->channel_trigger_enabled) {
            struct extra_info *info =(extra_info *) iio_channel_get_data(chn);
            offset = get_trigger_offset(chn, dev_info->trigger_falling_edge,
                                        dev_info->trigger_value);
            const off_t quatter_of_capture_interval = info->offset / 4;
            if (offset / (off_t)sizeof(gfloat) < quatter_of_capture_interval) {
                offset = 0;
            } else if (offset) {
                offset -= quatter_of_capture_interval * sizeof(gfloat);
                for (i = 0; i < nb_channels; i++) {
                    chn = iio_device_get_channel(dev, i);
                    if (iio_channel_is_enabled(chn))
                        apply_trigger_offset(chn, offset);
                }
            }
        }

        if (dev_info->channels_data_copy) {

            for (i = 0; i < nb_channels; i++) {
                struct iio_channel *ch = iio_device_get_channel(dev, i);
                struct extra_info *info =(extra_info *) iio_channel_get_data(ch);
                memcpy(dev_info->channels_data_copy[i], info->data_ref,
                       sample_count * sizeof(gfloat));
            }

            dev_info->channels_data_copy = NULL;
            //G_UNLOCK(buffer_full);
        }

        if (device_is_oneshot(dev)) {
            iio_buffer_destroy(dev_info->buffer);
            dev_info->buffer = NULL;
        }

        if (!dev_info->channel_trigger_enabled || offset)
            update_plot(dev_info->buffer);
    }

    stop_capture=!stop_capture;

capture_stop_check:
    return;
}

/**
 * @brief Plot::capture_start
 */
void Plot::capture_start()
{
    if (!priv->redraw_function) {
        priv->stop_redraw = FALSE;

    } else {
        priv->stop_redraw = FALSE;
        priv->redraw_function = 1;
        timerChart->start();
        timerChartSweep->start();
    }
}

/**
 * @brief Plot::capture_process_ended
 */
void Plot::capture_process_ended()
{
    if (restart_capture == true) {
        fprintf(stderr, "Sample acquisition stopped\n");
        restart_capture = false;

        /* Wait 100 msec then restart acquisition */
        g_usleep(G_USEC_PER_SEC * 0.1);

        capture_setup();
        fprintf(stderr, "Restarting acquisition\n");
        capture_start();
    }
}

#pragma endregion }

#pragma region Ungrouped {

iio_buffer * Plot::osc_plot_get_buffer()
{
    struct extra_dev_info *dev_info;

    dev_info =(extra_dev_info *) iio_device_get_data(priv->current_device);
    return dev_info->buffer;
}

double Plot::osc_plot_get_sample_count () {

    int count;

    if (ui->cmb_plot_type->currentIndex ()== FFT_PLOT ||
            ui->cmb_plot_type->currentIndex ()== FFT_SEEK_PLOT ||
            ui->cmb_plot_type->currentIndex () == SPECTRUM_PLOT)
        count = ui->cmb_fft_size->currentText ().toInt();
    else
        count = ui->sample_count_widget->value ();

    //    xItems=QVector<float>(count);
    //    yItems=QVector<float>(count);

    return count;
}

int Plot::plot_get_sample_count_of_device(const char *device)
{
    iio_device *iio_dev;
    extra_dev_info *dev_info;
    gdouble freq;
    int count = -1;

    if (!device)
        return count;

    if (!priv)
        return count;

    if (!ctx)
        return count;


    switch (ui->cmb_sample_count_units->currentIndex ()) {
    case 0:
        count = (int)osc_plot_get_sample_count();
        break;
    case 1:
        iio_dev = iio_context_find_device(ctx, device);
        if (!iio_dev)
            break;

        dev_info =(extra_dev_info *) iio_device_get_data(iio_dev);
        if (!dev_info)
            break;

        freq = dev_info->adc_freq * prefix2scale(dev_info->adc_scale);

        count = (int)round(ui->sample_count_widget->text ().toDouble () *
                           freq) / pow(10.0, 6);

        break;
    }

    return count;
}

/**
 * @brief Plot::plot_channels_update
 */
void Plot::plot_channels_update()
{
    GSList *node;
    PlotChn *ch;
    PlotMathChn *mch;
    int num_samples;

    g_return_if_fail(this);

    for (node = priv->ch_settings_list;
         node; node = g_slist_next(node)) {
        ch =(PlotChn *) node->data;
        if (ch->type != PLOT_MATH_CHANNEL)
            continue;

        mch = (PlotMathChn *)ch;
        num_samples = plot_get_sample_count_of_device(
                    mch->iio_device_name);
        if (num_samples < 0)
            num_samples = osc_plot_get_sample_count();
        mch->data_ref =(float*) realloc(mch->data_ref,
                                        sizeof(gfloat) * num_samples);

    }
}

void Plot::collect_parameters_from_plot()
{
    plot_params *prms;
    GSList *list;
    unsigned int i;

    for (i = 0; i < iio_context_get_devices_count(ctx); i++) {
        struct iio_device *dev = iio_context_get_device(ctx, i);
        extra_dev_info *info =(extra_dev_info *) iio_device_get_data(dev);
        const char *dev_name = iio_device_get_name(dev) ?: iio_device_get_id(dev);

        if (info->input_device == false)
            continue;

        prms =(plot_params *)malloc(sizeof(struct plot_params));
        prms->plot_id = priv->object_id;
        prms->sample_count = plot_get_sample_count_of_device(dev_name);
        list = info->plots_sample_counts;
        list = g_slist_prepend(list, prms);
        info->plots_sample_counts = list;
    }

}

void Plot::remove_transform_from_list(Transform *tr)
{
    QList<Transform*> *list = priv->transform_list;

    if (tr->has_the_marker)
        priv->tr_with_marker = NULL;

    transform_remove_own_markers(tr);
    if (tr->type_id == FREQ_SPECTRUM_TRANSFORM) {
        free(FREQ_SPECTRUM_SETTINGS(tr)->ffts_alg_data);
        free(FREQ_SPECTRUM_SETTINGS(tr)->maxXaxis);
        free(FREQ_SPECTRUM_SETTINGS(tr)->maxYaxis);
    }

    DataTypes dataTypes;
    dataTypes.TrList_remove_transform(list, tr);
    dataTypes.Transform_destroy(tr);

    if (list->size() == 0) {
        priv->active_transform_type = NO_TRANSFORM_TYPE;
    }
}

void Plot::remove_all_transforms()
{
    while (priv->transform_list->size())
        remove_transform_from_list(priv->transform_list->at(0));
}

QVector<double> * Plot::plot_channels_get_nth_data_ref(GSList *list, guint n)
{
    GSList *nth_node;
    QVector<double> *data = NULL;

    if (!list) {
        fprintf(stderr, "Invalid list argument.");
        if (!data)
            fprintf(stderr, "Could not find data reference in %s\n",
                    __func__);
        return data;
    }

    nth_node = g_slist_nth(list, n);
    if (!nth_node || !nth_node->data) {
        fprintf(stderr, "Element at index %d does not exist.", n);
        if (!data)
            fprintf(stderr, "Could not find data reference in %s\n",
                    __func__);
        return data;
    }

    PlotChn *plot_ch=(PlotChn *)(nth_node->data);
    data = plot_ch->get_data_ref(plot_ch);

    if (!data)
        fprintf(stderr, "Could not find data reference in %s\n",
                __func__);

    return data;
}

void Plot::do_fft(Transform *tr)
{
    if(globals::hopping) return;

    struct _fft_settings *settings =(_fft_settings *) tr->settings;
    struct _fft_alg_data *fft = &settings->fft_alg_data;
    struct marker_type *markers = settings->markers;
    enum marker_types marker_type = MARKER_OFF;

    QVector<double> *in_data = settings->real_source;

    QVector<double> *in_data_c;
    QVector<double> *out_data = tr->y_axis;
    QVector<double> *X = tr->x_axis;
    int fft_size = settings->fft_size;
    int i, j, k;
    int cnt;
    gfloat mag;
    double avg, pwr_offset;
    int maxX[MAX_MARKERS + 1];
    gfloat maxY[MAX_MARKERS + 1];
    gfloat plugin_fft_corr;

    if (settings->marker_type)
        marker_type = *((enum marker_types *)settings->marker_type);

    if ((fft->cached_fft_size == -1) || (fft->cached_fft_size != fft_size) ||
            (fft->cached_num_active_channels != fft->num_active_channels)) {

        if (fft->cached_fft_size != -1) {
            fftw_destroy_plan(fft->plan_forward);
            fftw_free(fft->win);
            fftw_free(fft->out);
            if (fft->in != NULL)
                fftw_free(fft->in);
            if (fft->in_c != NULL)
                fftw_free(fft->in_c);
            fft->in_c = NULL;
            fft->in = NULL;
        }

        fft->win =(double*) fftw_malloc(sizeof(double) * fft_size);
        if (fft->num_active_channels == 2) {
            fft->m = fft_size;
            fft->in_c =(double (*)[2])fftw_malloc(sizeof(fftw_complex) * fft_size);
            fft->in = NULL;
            fft->out = (double (*)[2])fftw_malloc(sizeof(fftw_complex) * (fft->m + 1));
            fft->plan_forward = fftw_plan_dft_1d(fft_size, fft->in_c, fft->out, FFTW_FORWARD, FFTW_ESTIMATE);
        } else {
            fft->m = fft_size / 2;
            fft->out =(double (*)[2]) fftw_malloc(sizeof(fftw_complex) * (fft->m + 1));
            fft->in_c = NULL;
            fft->in = (double *)fftw_malloc(sizeof(double) * fft_size);
            fft->plan_forward = fftw_plan_dft_r2c_1d(fft_size, fft->in, fft->out, FFTW_ESTIMATE);
        }

        for (i = 0; i < fft_size; i ++)
            fft->win[i] = window_function(win.toLocal8Bit().data(), i, fft_size);
        //            fft->win[i] = window_function(settings->fft_win, i, fft_size);


        fft->cached_fft_size = fft_size;
        fft->cached_num_active_channels = fft->num_active_channels;
    }

    if (fft->num_active_channels == 2) {
        in_data_c = settings->imag_source;


        for (cnt = 0, i = 0; cnt < fft_size; cnt++) {
            /* normalization and scaling see fft_corr */

            fft->in_c[cnt][0]=(*in_data)[i] * fft->win[cnt];
            fft->in_c[cnt][1]=(*in_data_c)[i] * fft->win[cnt];
            //            fft->in_c[cnt]=(*in_data)[i] * fft->win[cnt] + I * (*in_data_c)[i] * fft->win[cnt];
            i++;
        }
    } else {
        for (cnt = 0, i = 0; i < fft_size; i++) {
            /* normalization and scaling see fft_corr */
            fft->in[cnt] = (*in_data)[i] * fft->win[cnt];
            cnt++;
        }
    }

    struct iio_device *iio_dev = transform_get_device_parent(tr);
    struct extra_dev_info *dev_info =(extra_dev_info *) iio_device_get_data(iio_dev);

    plugin_fft_corr = dev_info->plugin_fft_corr;

    fft->plan_forward = fftw_plan_dft_1d(fft_size, fft->in_c, fft->out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(fft->plan_forward);
    avg = (double)settings->fft_avg;
    if (avg && avg != 128 )
        avg = 1.0f / avg;

    //pwr_offset = settings->fft_pwr_off + window_function_offset(settings->fft_win);

    //    pwr_offset = settings->fft_pwr_off + window_function_offset(win);
    pwr_offset = settings->fft_pwr_off + window_function_offset(settings->fft_win);


    for (j = 0; j <= MAX_MARKERS; j++) {
        maxX[j] = 0;
        maxY[j] = -200.0f;
    }

    int seekIndex=0;

    int seekSection=0;

    if(settings->is_seek)
    {
        if(tr->seek_x_axis->length()>=(int)(fftSize*sections))
        {
            seekSection=0;

            // Move to current seek section
            if(seekStartFreq!=baseFreq)
            {
                while((baseFreq-seekSection*seekStep)>seekStartFreq)
                    seekSection++;

                //switch(seekSection)
                //{
                // case 0:case 1:case 2:
                // seekSection+=10;
                // break;
                // default:
                // seekSection-=3;
                // break;
            }

        }
    }

    //int fftStartValid=fftSize*0.2;
    //int fftEndValid=fftSize*0.8;

    for (i = 0; i < fft->m; ++i) {

        if (fft->num_active_channels == 2) {
            if (i < (fft->m / 2))
                j = i + (fft->m / 2);
            else
                j = i - (fft->m / 2);
        } else {
            j = i;
        }

        if (fft->out[j][0] == 0 && fft->out[j][1] == 0)
        {
            fft->out[j][0]=FLT_MIN;
            fft->out[j][1]=FLT_MIN;
            //(fft->out)[j] = FLT_MIN + I * FLT_MIN;
        }

        mag = 10 * log10((fft->out[j][0] * fft->out[j][0] +
                fft->out[j][1] * fft->out[j][1]) / ((unsigned long long)fft->m * fft->m)) +
                fft->fft_corr + pwr_offset + plugin_fft_corr;

        /* it's better for performance to have separate loops,
                     * rather than do these tests inside the loop, but it makes
                     * the code harder to understand... Oh well...
                     ***/

        if ((*out_data)[i] == FLT_MAX) {
            /* Don't average the first iteration */
            (*out_data)[i] = mag;
        } else if (!avg) {
            /* keep peaks */
            if ((*out_data)[i] <= mag)
                (*out_data)[i] = mag;

        } else if (avg == 128) {
            /* keep min */
            if ((*out_data)[i] >= mag)
                (*out_data)[i] = mag;
        } else {
            /* do an average */
            (*out_data)[i] = ((1 - avg) * (*out_data)[i]) + (avg * mag);
        }

        if (!settings->markers || i < 2)
            continue;
        if (MAX_MARKERS && (marker_type == MARKER_PEAK ||
                            marker_type == MARKER_ONE_TONE ||
                            marker_type == MARKER_IMAGE)) {
            if (i <= 2) {
                maxX[0] = 0;
                maxY[0] = (*out_data)[0];
            } else {
                for (j = 0; j <= MAX_MARKERS && markers[j].active; j++) {
                    if  (((*out_data)[i - 1] > maxY[j]) &&
                         ((!(((*out_data)[i - 2] > (*out_data)[i - 1]) &&
                             ((*out_data)[i - 1] > (*out_data)[i]))) &&
                          (!(((*out_data)[i - 2] < (*out_data)[i - 1]) &&
                             ((*out_data)[i - 1] < (*out_data)[i]))))) {
                        if (marker_type == MARKER_PEAK) {
                            for (k = MAX_MARKERS; k > j; k--) {
                                maxY[k] = maxY[k - 1];
                                maxX[k] = maxX[k - 1];
                            }
                        }
                        maxY[j] = (*out_data)[i - 1];
                        maxX[j] = i - 1;
                        break;
                    }
                }
            }
        }

        if(settings->is_seek)
        {
            // Fill seek fft items
            seekIndex=i+(fftSize*(seekSection));
            if(seekIndex<tr->seek_y_axis->length())
            {
                if(i<3413)
                {
                    if(i<760) (*tr->seek_y_axis)[seekIndex] = (*tr->y_axis)[i]+0.0088*i-32 - 0.0105*i + 7+5;

                    else (*tr->seek_y_axis)[seekIndex]=(*tr->y_axis)[i]+0.0088*i-34+5;
                }

                else if( i>13068)
                {
                    if(i<=15748) (*tr->seek_y_axis)[seekIndex]=(*tr->y_axis)[i]-0.0088*(i-13067)-7+8;

                    else (*tr->seek_y_axis)[seekIndex]=(*tr->y_axis)[i]-0.0088*(i-13067)-7  + 0.0105*(i-15748)+10;
                }

                else (*tr->seek_y_axis)[seekIndex]=(*tr->y_axis)[i];
            }
        }
        else{

            // Fill seek fft items
            //            seekIndex=i+(fftSize*(seekSection));
            //            if(seekIndex<tr->y_axis->length())
            //            {
            //            qInfo() <<"----->" << (*tr->y_axis)[i];

            //dorafshan
            if(i<3413)
            {
                if(i<760 and (*tr->y_axis)[i]<(-55)) (*tr->y_axis)[i]=(*tr->y_axis)[i]+0.0088*i-32 - 0.0105*i + 7+5;
                else if ((*tr->y_axis)[i]<(-60))
                {
                    (*tr->y_axis)[i]=(*tr->y_axis)[i]+0.0088*i-34+5;}
            }

            else if( i>13068)
            {
                if(i<=15748 and (*tr->y_axis)[i]<(-60)) (*tr->y_axis)[i]=(*tr->y_axis)[i]-0.0088*(i-13067)-7+5;
                else if((*tr->y_axis)[i]<(-55)) (*tr->y_axis)[i]=(*tr->y_axis)[i]-0.0088*(i-13067)-7  + 0.0105*(i-15748)+5;
            }

            else (*tr->y_axis)[i]=(*tr->y_axis)[i];
            //            }
        }
    }

    getFFTMax(tr,settings->is_seek);
    getFFTMin(tr,settings->is_seek);

    //    fftMax+=(abs(fftMax)+abs(fftMin))/2;
    //    fftMin-=(abs(fftMax)+abs(fftMin))/8;

    //    if (!settings->markers)
    //        return;

    //    int m = fft->m;

    //    if ((marker_type == MARKER_ONE_TONE || marker_type == MARKER_IMAGE) &&
    //        ((fft->num_active_channels == 1 && maxX[0] == 0) ||
    //         (fft->num_active_channels == 2 && maxX[0] == m/2))) {
    //        unsigned int max_tmp;

    //        max_tmp = maxX[1];
    //        maxX[1] = maxX[0];
    //        maxX[0] = max_tmp;
    //    }

    //    if (MAX_MARKERS && marker_type != MARKER_OFF) {
    //        for (j = 0; j <= MAX_MARKERS && markers[j].active; j++) {
    //            if (marker_type == MARKER_PEAK) {
    //                markers[j].x = (gfloat)(*X)[maxX[j]];
    //                markers[j].y = (gfloat)(*out_data)[maxX[j]];
    //                markers[j].bin = maxX[j];
    //            } else if (marker_type == MARKER_FIXED) {
    //                markers[j].x = (gfloat)(*X)[markers[j].bin];
    //                markers[j].y = (gfloat)(*out_data)[markers[j].bin];
    //            } else if (marker_type == MARKER_ONE_TONE) {
    //                /* assume peak is the tone */
    //                if (j == 0) {
    //                    markers[j].bin = maxX[j];
    //                    i = 1;
    //                } else if (j == 1) {
    //                    /* keep DC */
    //                    if (tr->type_id == COMPLEX_FFT_TRANSFORM)
    //                        markers[j].bin = m / 2;
    //                    else
    //                        markers[j].bin = 0;
    //                } else {
    //                    /* where should the spurs be? */
    //                    i++;
    //                    if (tr->type_id == COMPLEX_FFT_TRANSFORM) {
    //                        markers[j].bin = markers[0].bin * i;
    //                        if (i % 2 == 0)
    //                            markers[j].bin += m / 2;
    //                        markers[j].bin %= m;
    //                    } else {
    //                        markers[j].bin = (markers[0].bin * i) % (2 * m);
    //                        /* Mirror the even Nyquist zones */
    //                        if (markers[j].bin > m)
    //                            markers[j].bin = 2 * m - markers[j].bin;
    //                    }
    //                }
    //                /* make sure we don't need to nudge things one way or the other */
    //                k = markers[j].bin;
    //                while (out_data[k] < out_data[k + 1]) {
    //                    k++;
    //                }

    //                while (markers[j].bin != 0 &&
    //                       out_data[markers[j].bin] < out_data[markers[j].bin - 1]) {
    //                    markers[j].bin--;
    //                }

    //                if (out_data[k] > out_data[markers[j].bin])
    //                    markers[j].bin = k;

    //                markers[j].x = (gfloat)(*X)[markers[j].bin];
    //                markers[j].y = (gfloat)(*out_data)[markers[j].bin];
    //            } else if (marker_type == MARKER_IMAGE) {
    //                /* keep DC, fundamental, and image
    //                                     * num_active_channels always needs to be 2 for images */
    //                if (j == 0) {
    //                    /* Fundamental */
    //                    markers[j].bin = maxX[j];
    //                } else if (j == 1) {
    //                    /* DC */
    //                    markers[j].bin = m / 2;
    //                } else if (j == 2) {
    //                    /* Image */
    //                    markers[j].bin = m / 2 - (markers[0].bin - m/2);
    //                } else
    //                    continue;
    //                markers[j].x = (gfloat)(*X)[markers[j].bin];
    //                markers[j].y = (gfloat)(*out_data)[markers[j].bin];

    //            }
    //            if (fft->num_active_channels == 2) {
    //                fft->in_c[cnt][0]=(*in_data)[i] * fft->win[cnt];
    //                fft->in_c[cnt][1]=(*in_data_c)[i] * fft->win[cnt];

    //                markers[j].vector[0]=(*in_data)[markers[j].bin];
    //                markers[j].vector[1]=(*settings->imag_source)[markers[j].bin];
    //                //                                markers[j].vector = I * settings->imag_source[markers[j].bin] +
    //                //                                    in_data[markers[j].bin];
    //            } else {
    //                markers[j].vector[0]=0;
    //                markers[j].vector[1]=0;
    //                //                                markers[j].vector = 0 + I * 0;
    //            }
    //        }
    //        if (settings->markers_copy && *settings->markers_copy) {
    //            memcpy(*settings->markers_copy, settings->markers,
    //                   sizeof(struct marker_type) * MAX_MARKERS);
    //            *settings->markers_copy = NULL;
    //            g_mutex_unlock(settings->marker_lock);
    //        }
    //    }

}

bool Plot::time_transform_function(Transform *tr, gboolean init_transform)
{
    struct _time_settings *settings =static_cast<_time_settings *>(tr->settings);
    unsigned axis_length = settings->num_samples;
    QVector<double> *in_data;
    unsigned int i;

    DataTypes datatype;
    if (init_transform) {

        /* Set the sources of the transfrom */
        settings->data_source = plot_channels_get_nth_data_ref(tr->plot_channels, 0);

        /* Initialize axis */
        datatype.Transform_resize_x_axis(tr, axis_length);
        for (i = 0; i < axis_length; i++) {
            if (settings->max_x_axis && settings->max_x_axis != 0)
                (*tr->x_axis)[i]=(gfloat)(i * settings->max_x_axis)/axis_length;
            else
                (*tr->x_axis)[i]=i;
        }
        tr->y_axis_size = axis_length;

        if (settings->apply_inverse_funct ||
                settings->apply_multiply_funct ||
                settings->apply_add_funct) {
            datatype.Transform_resize_y_axis(tr, tr->y_axis_size);
        } else {
            tr->y_axis = settings->data_source;
        }

        return true;
    }

    if (tr->plot_channels_type == PLOT_MATH_CHANNEL) {
        PlotMathChn *m =(PlotMathChn *) tr->plot_channels->data;
        m->math_expression(m->iio_channels_data,
                           m->data_ref, settings->num_samples);
    } else if (tr->plot_channels_type == PLOT_IIO_CHANNEL) {
        if (!settings->apply_inverse_funct &&
                !settings->apply_multiply_funct &&
                !settings->apply_add_funct)
            return true;

        in_data = plot_channels_get_nth_data_ref(tr->plot_channels, 0);
        if (!in_data)
            return false;

        for (i = 0; i < tr->y_axis_size; i++) {
            if (settings->apply_inverse_funct) {
                if (in_data[i].data() != 0)
                    (*tr->y_axis)[i]= 1 / *in_data[i].data();
                else
                    (*tr->y_axis)[i]= 65535;
            } else {
                (*tr->y_axis)[i]= *in_data[i].data();
            }
            if (settings->apply_multiply_funct)
                (*tr->y_axis)[i] *= settings->multiply_value;
            if (settings->apply_add_funct)
                (*tr->y_axis)[i] += settings->add_value;
        }
    }

    return true;

}

bool Plot::cross_correlation_transform_function(Transform *tr, gboolean init_transform)
{
    //    struct _cross_correlation_settings *settings =(_cross_correlation_settings *) tr->settings;
    //    unsigned axis_length = settings->num_samples;
    //    gfloat *i_0, *q_0;
    //    gfloat *i_1, *q_1;
    //    unsigned int i;

    //    DataTypes datatype;

    //    if (init_transform) {
    //        /* Set the sources of the transfrom */
    //        settings->i0_source = plot_channels_get_nth_data_ref(tr->plot_channels, 0);
    //        settings->q0_source = plot_channels_get_nth_data_ref(tr->plot_channels, 1);
    //        settings->i1_source = plot_channels_get_nth_data_ref(tr->plot_channels, 2);
    //        settings->q1_source = plot_channels_get_nth_data_ref(tr->plot_channels, 3);

    //        /* Initialize axis */
    //        if (settings->signal_a) {
    //            fftw_free(settings->signal_a);
    //            settings->signal_a = NULL;
    //        }
    //        if (settings->signal_b) {
    //            fftw_free(settings->signal_b);
    //            settings->signal_b = NULL;
    //        }
    //        if (settings->xcorr_data) {
    //            fftw_free(settings->xcorr_data);
    //            settings->xcorr_data = NULL;
    //        }
    //        settings->signal_a = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * axis_length);
    //        settings->signal_b = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * axis_length);
    //        settings->xcorr_data = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * axis_length * 2);
    //        //settings->xcorr_data[0] = FLT_MAX;

    //        datatype.Transform_resize_x_axis(tr, 2 * axis_length);
    //        datatype.Transform_resize_y_axis(tr, 2 * axis_length);
    //        for (i = 0; i < 2 * axis_length - 1; i++) {
    //            tr->x_axis[i] = ((i - (gfloat)axis_length + 1) * settings->max_x_axis)/(gfloat)axis_length;
    //            tr->y_axis[i] = 0;
    //        }
    //        tr->y_axis_size = 2 * axis_length - 1;

    //        return true;
    //    }

    //    GSList *node;

    //    if (tr->plot_channels_type == PLOT_MATH_CHANNEL)
    //        for (node = tr->plot_channels; node; node = g_slist_next(node)) {
    //            PlotMathChn *m =(PlotMathChn *) node->data;
    //            m->math_expression(m->iio_channels_data,
    //                               m->data_ref, settings->num_samples);
    //        }

    //    i_0 = settings->i0_source;
    //    q_0 = settings->q0_source;
    //    i_1 = settings->i1_source;
    //    q_1 = settings->q1_source;

    ////    for (i = 0; i < axis_length; i++) {
    ////        settings->signal_a[i] =q_0[i] + I * i_0[i];
    ////        settings->signal_b[i] = q_1[i] + I * i_1[i];
    ////    }

    //    if (settings->revert_xcorr)
    //        xcorr(settings->signal_b, settings->signal_a, settings->xcorr_data, axis_length, (double)settings->avg);
    //    else
    //        xcorr(settings->signal_a, settings->signal_b, settings->xcorr_data, axis_length, (double)settings->avg);

    //    gfloat *out_data = tr->y_axis;
    //    gfloat *X = tr->x_axis;
    //    struct marker_type *markers = settings->markers;
    //    enum marker_types marker_type = MARKER_OFF;
    //    unsigned int maxX[MAX_MARKERS + 1];
    //    gfloat maxY[MAX_MARKERS + 1];
    //    int j, k;

    //    if (settings->marker_type)
    //        marker_type = *((enum marker_types *)settings->marker_type);

    //    for (j = 0; j <= MAX_MARKERS; j++) {
    //        maxX[j] = 0;
    //        maxY[j] = -200.0f;
    //    }

    //    /* find the peaks */
    //    for (i = 0; i < 2 * axis_length - 1; i++) {
    //        //tr->y_axis[i] =  2 * creal(settings->xcorr_data[i]) / (gfloat)axis_length;
    //        if (!settings->markers)
    //            continue;

    //        if (MAX_MARKERS && marker_type == MARKER_PEAK) {
    //            if (i <= 2) {
    //                maxX[0] = 0;
    //                maxY[0] = out_data[0];
    //            } else {
    //                for (j = 0; j <= MAX_MARKERS && markers[j].active; j++) {
    //                    if  ((fabs(out_data[i - 1]) > maxY[j]) &&
    //                         ((!((out_data[i - 2] > out_data[i - 1]) &&
    //                             (out_data[i - 1] > out_data[i]))) &&
    //                          (!((out_data[i - 2] < out_data[i - 1]) &&
    //                             (out_data[i - 1] < out_data[i]))))) {
    //                        if (marker_type == MARKER_PEAK) {
    //                            for (k = MAX_MARKERS; k > j; k--) {
    //                                maxY[k] = maxY[k - 1];
    //                                maxX[k] = maxX[k - 1];
    //                            }
    //                        }
    //                        maxY[j] = fabs(out_data[i - 1]);
    //                        maxX[j] = i - 1;
    //                        break;
    //                    }
    //                }
    //            }
    //        }
    //    }

    //    if (!settings->markers)
    //        return true;

    //    /* now we know where the peaks are, we estimate the actual peaks,
    //     * by quadratic interpolation of existing spectral peaks, which is explained:
    //     * https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
    //     * written by Julius Orion Smith III.
    //     */
    //    if (MAX_MARKERS && marker_type != MARKER_OFF) {
    //        for (j = 0; j <= MAX_MARKERS && markers[j].active; j++)
    //            if (marker_type == MARKER_PEAK) {
    //                /* If we don't have the alpha or the gamma peaks, we can't continue */
    //                if (maxX[j] < 1 || maxX[j] > 2 * axis_length - 1) {
    //                    markers[j].x = 0;
    //                    markers[j].y = 0;
    //                    continue;
    //                }
    //                /* sync'ed with the pictures in the url above:
    //                 * alpha = (gfloat)out_data[maxX[j] - 1];
    //                 * gamma = (gfloat)out_data[maxX[j] + 1];
    //                 * beta  = (gfloat)out_data[maxX[j]];
    //                 */
    //                markers[j].x = (gfloat)((out_data[maxX[j] - 1] - out_data[maxX[j] + 1]) /
    //                        (2 * (out_data[maxX[j] - 1] - 2 * out_data[maxX[j]] +
    //                        out_data[maxX[j] + 1])));
    //                markers[j].y = (gfloat)(out_data[maxX[j]] - (out_data[maxX[j] - 1] - out_data[maxX[j] + 1]) *
    //                        markers[j].x / 4);
    //                markers[j].x += (gfloat)X[maxX[j]];
    //                markers[j].bin = maxX[j];
    //            }
    //        if (settings->markers_copy && *settings->markers_copy) {
    //            memcpy(*settings->markers_copy, settings->markers,
    //                   sizeof(struct marker_type) * MAX_MARKERS);
    //            *settings->markers_copy = NULL;
    //            g_mutex_unlock(settings->marker_lock);
    //        }
    //    }

    return true;
}

bool Plot::freq_spectrum_transform_function(Transform *tr, gboolean init_transform)
{
    struct iio_channel *chn;
    struct _freq_spectrum_settings *settings =static_cast<_freq_spectrum_settings *>(tr->settings);
    unsigned i, j, k, axis_length, fft_size, bits_used;
    int ret;
    double sampling_freq;
    bool complete_transform = false;

    DataTypes dataType;
    if (init_transform) {
        fft_size = settings->fft_size;
        chn = PLOT_IIO_CHN(tr->plot_channels->data)->iio_chn;
        ret = iio_channel_attr_read_double(chn, "sampling_frequency",
                                           &sampling_freq);
        if (ret < 0)
            return false;
        sampling_freq /= 1000000; /* Hz to MHz*/

        bits_used = iio_channel_get_data_format(chn)->bits;

        if (!bits_used)
            return false;

        /* Compute FFT normalization and scaling offset */
        settings->fft_corr = 20 * log10(2.0 / (1ULL << (bits_used - 1)));

        settings->fft_lower_clipping_limit = (fft_size / 2) - (settings->filter_bandwidth * fft_size) / (2 * sampling_freq);
        settings->fft_upper_clipping_limit = (fft_size / 2) + (settings->filter_bandwidth * fft_size) / (2 * sampling_freq);

        settings->real_source = plot_channels_get_nth_data_ref(tr->plot_channels, 0);
        settings->imag_source = plot_channels_get_nth_data_ref(tr->plot_channels, 1);

        axis_length = (settings->fft_upper_clipping_limit - settings->fft_lower_clipping_limit) * settings->fft_count;
        dataType.Transform_resize_x_axis(tr, axis_length);
        dataType.Transform_resize_y_axis(tr, axis_length);

        for (i = 0, k = 0; i < settings->fft_count; i++) {
            for (j = 0; j < fft_size; j++) {
                if (j >= settings->fft_lower_clipping_limit && j < settings->fft_upper_clipping_limit) {
                    (*tr->x_axis)[k]= (j * sampling_freq / settings->fft_size - sampling_freq / 2) + settings->freq_sweep_start + (settings->filter_bandwidth) * i;
                    (*tr->y_axis)[k]= FLT_MAX;
                    k++;
                }
            }
        }

        for (i = 0; i <= MAX_MARKERS; i++) {
            settings->maxXaxis[i] = 0;
            settings->maxYaxis[i] = -200.0f;
        }

        return true;
    }

    do_fft_for_spectrum(tr);
    settings->fft_index++;

    if (settings->fft_index == settings->fft_count) {
        settings->fft_index = 0;
        complete_transform = true;

        if (MAX_MARKERS && *settings->marker_type != MARKER_OFF) {
            for (j = 0; j <= MAX_MARKERS && settings->markers[j].active; j++)
                if (*settings->marker_type == MARKER_PEAK) {
                    settings->markers[j].x = (gfloat)*tr->x_axis[settings->maxXaxis[j]].data();
                    settings->markers[j].y = (gfloat)*tr->y_axis[settings->maxXaxis[j]].data();
                    settings->markers[j].bin = settings->maxXaxis[j];
                }
            if (*settings->markers_copy) {
                memcpy(*settings->markers_copy, settings->markers,
                       sizeof(struct marker_type) * MAX_MARKERS);
                *settings->markers_copy = NULL;
                g_mutex_unlock(settings->marker_lock);
            }
        }

        for (i = 0; i <= MAX_MARKERS; i++) {
            settings->maxXaxis[i] = 0;
            settings->maxYaxis[i] = -200.0f;
        }
    }

    return complete_transform;
}


bool Plot::fft_transform_function(Transform *tr, gboolean init_transform)
{
    struct iio_device *dev;
    struct extra_dev_info *dev_info;
    struct _fft_settings *settings =(_fft_settings *) tr->settings;
    unsigned num_samples;
    int axis_length;
    unsigned int bits_used;
    double corr;
    double step;
    int i;

    DataTypes dataType;
    if (init_transform) {
        /* Set the sources of the transfrom */
        settings->real_source = plot_channels_get_nth_data_ref(tr->plot_channels, 0);

        if (g_slist_length(tr->plot_channels) > 1)
            settings->imag_source = plot_channels_get_nth_data_ref(tr->plot_channels, 1);

        /* Initialize axis */
        dev = transform_get_device_parent(tr);
        if (!dev)
            return false;
        dev_info =(extra_dev_info *) iio_device_get_data(dev);
        num_samples = dev_info->sample_count;
        if (dev_info->channel_trigger_enabled)
            num_samples /= 2;

        PlotChn *chn = static_cast<PlotChn *>(tr->plot_channels->data);
        struct iio_channel *iio_chn = NULL;

        bits_used = 0;
        if (chn->type == PLOT_IIO_CHANNEL) {
            iio_chn = PLOT_IIO_CHN(chn)->iio_chn;
        } else if (PLOT_CHN(chn)->type == PLOT_MATH_CHANNEL) {
            if (PLOT_MATH_CHN(chn)->iio_channels) {
                iio_chn = (struct iio_channel *)
                        PLOT_MATH_CHN(chn)->iio_channels->data;
            }
        }
        if (iio_chn)
            bits_used = iio_channel_get_data_format(iio_chn)->bits;

        if (!bits_used)
            return false;
        axis_length = settings->fft_size * settings->fft_alg_data.num_active_channels / 2;
        dataType.Transform_resize_x_axis(tr, axis_length);
        dataType.Transform_resize_y_axis(tr, axis_length);

        if(settings->is_seek)
        {
            int sections=(settings->seek_max-settings->seek_min)/settings->seek_step+1;

            double step=seekStartFreq-(settings->seek_step/2);
            double freqStep=settings->seek_step/(fftSize);

            dataType.Transform_resize_seek_x_axis(tr, sections*fftSize);
            dataType.Transform_resize_seek_y_axis(tr, sections*fftSize);

            for(int i=0;i<(int)(sections*fftSize);i++)
            {
                (*tr->seek_x_axis)[i]= step+i*freqStep;
                (*tr->seek_y_axis)[i]= -100;//FLT_MAX;
            }
        }

        if (settings->fft_alg_data.num_active_channels == 2)
            corr = dev_info->adc_freq / 2.0;
        else
            corr = 0;
        step=dev_info->adc_freq / num_samples;

        for (i = 0; i < axis_length; i++) {
            (*tr->x_axis)[i]= baseFreq - 245.759999 + i*0.029999 ;
            (*tr->y_axis)[i]= FLT_MAX;
        }

        /* Compute FFT normalization and scaling offset */
        settings->fft_alg_data.fft_corr = 20 * log10(2.0 / (1ULL << (bits_used - 1)));

        /* Make sure that previous positions of markers are not out of bonds */
        if (settings->markers)
            for (i = 0; i <= MAX_MARKERS; i++)
                if (settings->markers[i].bin >= axis_length)
                    settings->markers[i].bin = 0;

        return false;
    }

    GSList *node;

    if (tr->plot_channels_type == PLOT_MATH_CHANNEL)
        for (node = tr->plot_channels; node; node = g_slist_next(node)) {
            PlotMathChn *m =(PlotMathChn *) node->data;
            m->math_expression(m->iio_channels_data,
                               m->data_ref, settings->fft_size);
        }


    do_fft(tr);

    return true;
}

bool Plot::constellation_transform_function(Transform *tr, gboolean init_transform)
{
    struct _constellation_settings *settings =(_constellation_settings *) tr->settings;
    unsigned axis_length = settings->num_samples;

    if (init_transform) {
        /* Set the sources of the transfrom */
        settings->x_source = plot_channels_get_nth_data_ref(tr->plot_channels, 0);
        settings->y_source = plot_channels_get_nth_data_ref(tr->plot_channels, 1);

        /* Initialize axis */
        tr->x_axis_size = axis_length;
        tr->y_axis_size = axis_length;
        tr->x_axis = settings->x_source;
        tr->y_axis = settings->y_source;

        return true;
    }

    GSList *node;

    if (tr->plot_channels_type == PLOT_MATH_CHANNEL)
        for (node = tr->plot_channels; node; node = g_slist_next(node)) {
            PlotMathChn *m =(PlotMathChn *) node->data;
            m->math_expression(m->iio_channels_data,
                               m->data_ref, settings->num_samples);
        }

    return true;
}

int Plot::plot_get_sample_count_for_transform(Transform *transform)
{
    struct iio_device *iio_dev = transform_get_device_parent(transform);

    if (!iio_dev)
        iio_dev = priv->current_device;

    return plot_get_sample_count_of_device(
                iio_device_get_name(iio_dev) ?:
                                              iio_device_get_id(iio_dev));
}

void Plot::update_transform_settings(Transform *transform)
{
    unsigned i;
    int plot_type;

    plot_type = ui->cmb_plot_type->currentIndex();

    if (plot_type == FFT_PLOT || plot_type == FFT_SEEK_PLOT) {

        FFT_SETTINGS(transform)->fft_size = ui->cmb_fft_size->currentText().toInt ();
        // Own a stable copy. toLocal8Bit().data() dies with the temporary and
        // window_function_offset() later reads freed heap under ASan.
        g_free(FFT_SETTINGS(transform)->fft_win);
        const QByteArray fftWin = ui->cmb_fft_win->currentText().toLocal8Bit();
        FFT_SETTINGS(transform)->fft_win = g_strdup(fftWin.constData());
        win=ui->cmb_fft_win->currentText();
        FFT_SETTINGS(transform)->fft_avg = ui->txt_fft_avg->value();
        FFT_SETTINGS(transform)->fft_pwr_off =ui->txt_pwr_offset->value();
        FFT_SETTINGS(transform)->fft_alg_data.cached_fft_size = -1;
        FFT_SETTINGS(transform)->fft_alg_data.cached_num_active_channels = -1;
        FFT_SETTINGS(transform)->fft_alg_data.num_active_channels = g_slist_length(transform->plot_channels);
        FFT_SETTINGS(transform)->markers = NULL;
        FFT_SETTINGS(transform)->markers_copy = NULL;
        FFT_SETTINGS(transform)->marker_lock = NULL;
        FFT_SETTINGS(transform)->marker_type = NULL;

        if(plot_type == FFT_SEEK_PLOT)
        {
            FFT_SETTINGS(transform)->is_seek = true;
            FFT_SETTINGS(transform)->seek_min = ui->txt_start_freq->value();
            FFT_SETTINGS(transform)->seek_max = ui->txt_stop_freq->value();
            FFT_SETTINGS(transform)->seek_step = ui->txt_freq_step->value();
        }
    } else if (plot_type == TIME_PLOT) {

        int dev_samples = plot_get_sample_count_for_transform(transform);

        if (dev_samples < 0)
            return;

        TIME_SETTINGS(transform)->num_samples = dev_samples;
        if (PLOT_CHN(transform->plot_channels->data)->type == PLOT_IIO_CHANNEL) {
            PlotIioChn *set;

            set = (PlotIioChn *)transform->plot_channels->data;

            TIME_SETTINGS(transform)->apply_inverse_funct = set->apply_inverse_funct;
            TIME_SETTINGS(transform)->apply_multiply_funct = set->apply_multiply_funct;
            TIME_SETTINGS(transform)->apply_add_funct = set->apply_add_funct;
            TIME_SETTINGS(transform)->multiply_value = set->multiply_value;
            TIME_SETTINGS(transform)->add_value = set->add_value;
            TIME_SETTINGS(transform)->max_x_axis = ui->sample_count_widget->value();
        }
    } else if (plot_type == XY_PLOT){
        CONSTELLATION_SETTINGS(transform)->num_samples = ui->sample_count_widget->value();
    } else if (plot_type == XCORR_PLOT){
        int dev_samples = plot_get_sample_count_for_transform(transform);
        if (dev_samples < 0)
            return;

        XCORR_SETTINGS(transform)->num_samples = dev_samples;
        XCORR_SETTINGS(transform)->avg = ui->txt_fft_avg->value ();
        XCORR_SETTINGS(transform)->revert_xcorr = 0;
        XCORR_SETTINGS(transform)->signal_a = NULL;
        XCORR_SETTINGS(transform)->signal_b = NULL;
        XCORR_SETTINGS(transform)->xcorr_data = NULL;
        XCORR_SETTINGS(transform)->markers = NULL;
        XCORR_SETTINGS(transform)->markers_copy = NULL;
        XCORR_SETTINGS(transform)->marker_lock = NULL;
        XCORR_SETTINGS(transform)->marker_type = NULL;
        XCORR_SETTINGS(transform)->max_x_axis = ui->sample_count_widget->value ();
    } else if (plot_type == SPECTRUM_PLOT) {
        FREQ_SPECTRUM_SETTINGS(transform)->ffts_alg_data =(_fft_alg_data *) calloc(sizeof(struct _fft_alg_data), priv->fft_count);
        FREQ_SPECTRUM_SETTINGS(transform)->fft_count = priv->fft_count;
        FREQ_SPECTRUM_SETTINGS(transform)->freq_sweep_start = priv->start_freq + priv->filter_bw / 2;
        FREQ_SPECTRUM_SETTINGS(transform)->filter_bandwidth = priv->filter_bw;
        FREQ_SPECTRUM_SETTINGS(transform)->fft_size = ui->cmb_fft_size->currentText ().toInt ();
        FREQ_SPECTRUM_SETTINGS(transform)->fft_avg = ui->txt_fft_avg->value ();
        FREQ_SPECTRUM_SETTINGS(transform)->fft_pwr_off = ui->txt_pwr_offset->value ();
        FREQ_SPECTRUM_SETTINGS(transform)->maxXaxis =(unsigned int *) malloc(sizeof(unsigned int) * (MAX_MARKERS + 1));
        FREQ_SPECTRUM_SETTINGS(transform)->maxYaxis =(gfloat *) malloc(sizeof(unsigned int) * (MAX_MARKERS + 1));
        for (i = 0; i < priv->fft_count; i++) {
            FREQ_SPECTRUM_SETTINGS(transform)->ffts_alg_data[i].cached_fft_size = -1;
            FREQ_SPECTRUM_SETTINGS(transform)->ffts_alg_data[i].cached_num_active_channels = -1;
            FREQ_SPECTRUM_SETTINGS(transform)->ffts_alg_data[i].num_active_channels = g_slist_length(transform->plot_channels);
        }
    }
}

Transform* Plot::add_transform_to_list( int tr_type, GSList *channels)
{
    Transform *transform;
    struct _time_settings *time_settings;
    struct _fft_settings *fft_settings;
    struct _constellation_settings *constellation_settings;
    struct _cross_correlation_settings *xcross_settings;
    struct _freq_spectrum_settings *freq_spectrum_settings;
    GSList *node;

    DataTypes dataType;
    transform = dataType.Transform_new(tr_type);
    seekPeriodFailed=seekPeriod;
    transform->graph_color = &color_graph[priv->transform_list->size()];
    transform->plot_channels = g_slist_copy(channels);
    transform->plot_channels_type = PLOT_CHN(channels->data)->type;
    //    transform->ChannelName = PLOT_CHN(channels->data)->name;

    /* Enable iio channels used by the transform */
    for (node = channels; node; node = g_slist_next(node)) {
        PlotChn *plot_ch=(PlotChn *)node->data;

        if (plot_ch)
            plot_ch->assert_used_iio_channels(plot_ch, true);
    }

    switch (tr_type) {
    case TIME_TRANSFORM:
        dataType.Transform_attach_function(transform, time_transform_function);
        time_settings = (struct _time_settings *)calloc(sizeof(struct _time_settings), 1);
        dataType.Transform_attach_settings(transform, time_settings);
        transform->graph_color = &PLOT_CHN(channels->data)->graph_color;
        break;
    case FFT_TRANSFORM:
        dataType.Transform_attach_function(transform, fft_transform_function);
        fft_settings = (struct _fft_settings *)calloc(sizeof(struct _fft_settings), 1);
        dataType.Transform_attach_settings(transform, fft_settings);
        break;
    case CONSTELLATION_TRANSFORM:
        dataType.Transform_attach_function(transform, constellation_transform_function);
        constellation_settings = (struct _constellation_settings *)calloc(sizeof(struct _constellation_settings), 1);
        dataType.Transform_attach_settings(transform, constellation_settings);
        break;
    case COMPLEX_FFT_TRANSFORM:
        dataType.Transform_attach_function(transform, fft_transform_function);
        fft_settings = (struct _fft_settings *)calloc(sizeof(struct _fft_settings), 1);
        dataType.Transform_attach_settings(transform, fft_settings);
        break;
    case CROSS_CORRELATION_TRANSFORM:
        //                dataType.Transform_attach_function(transform, cross_correlation_transform_function);
        xcross_settings = (struct _cross_correlation_settings *)calloc(sizeof(struct _cross_correlation_settings), 1);
        dataType.Transform_attach_settings(transform, xcross_settings);
        break;
    case FREQ_SPECTRUM_TRANSFORM:
        dataType.Transform_attach_function(transform, freq_spectrum_transform_function);
        freq_spectrum_settings = (struct _freq_spectrum_settings *)calloc(sizeof(struct _freq_spectrum_settings), 1);
        dataType.Transform_attach_settings(transform, freq_spectrum_settings);
        break;
    default:
        fprintf(stderr, "Invalid transform\n");
        return NULL;
    }

    dataType.TrList_add_transform(priv->transform_list, transform);
    update_transform_settings(transform);

    priv->active_transform_type = tr_type;
    priv->current_device = transform_get_device_parent(transform);

    return transform;
}

void Plot::channels_transform_assignment(PlotChn * chn,ch_tr_params *user_data)
{
    struct ch_tr_params *prm =(ch_tr_params *) user_data;
    PlotChn *settings;
    Transform *transform = NULL;
    gboolean enabled;
    int num_added_chs;

    settings=chn;

    prm->ch_settings = g_slist_prepend(prm->ch_settings, settings);
    num_added_chs = g_slist_length(prm->ch_settings);

    switch (ui->cmb_plot_type->currentIndex ()) {
    case TIME_PLOT:
        transform = add_transform_to_list(TIME_TRANSFORM, prm->ch_settings);
        break;
    case FFT_PLOT : case FFT_SEEK_PLOT:
        if (prm->enabled_channels == 1) {
            transform = add_transform_to_list( FFT_TRANSFORM, prm->ch_settings);
        } else if ((prm->enabled_channels == 2 || prm->enabled_channels == 4) && num_added_chs == 2) {
            if (plugin_installed("FMComms6")) {
                transform = add_transform_to_list( COMPLEX_FFT_TRANSFORM, prm->ch_settings);
            } else {
                prm->ch_settings = g_slist_reverse(prm->ch_settings);
                transform = add_transform_to_list( COMPLEX_FFT_TRANSFORM, prm->ch_settings);
            }
        }
        break;
    case XY_PLOT:
        if (prm->enabled_channels == 2 && num_added_chs == 2) {
            prm->ch_settings = g_slist_reverse(prm->ch_settings);
            transform = add_transform_to_list(CONSTELLATION_TRANSFORM, prm->ch_settings);
        }
        break;
    case XCORR_PLOT:
        if (prm->enabled_channels == 4 && num_added_chs == 4) {
            prm->ch_settings = g_slist_reverse(prm->ch_settings);
            transform = add_transform_to_list(CROSS_CORRELATION_TRANSFORM, prm->ch_settings);
        }
        break;
    case SPECTRUM_PLOT:
        if (prm->enabled_channels == 2 && num_added_chs == 2) {
            prm->ch_settings = g_slist_reverse(prm->ch_settings);
            transform = add_transform_to_list(FREQ_SPECTRUM_TRANSFORM, prm->ch_settings);
        }
        break;
    default:
        break;
    }
    if (transform && prm->ch_settings) {
        g_slist_free(prm->ch_settings);
        prm->ch_settings = NULL;
    }
}

void Plot::devices_transform_assignment()
{
    struct ch_tr_params prm;

    prm.enabled_channels = enabled_channels_count();
    prm.ch_settings = NULL;

    QTreeWidgetItemIterator it(ui->tree_channel_list);

    QString name;
    QString parent;

    while(*it)
    {
        if((*it)->parent ())
        {
            name=(*it)->text (0);
            parent=(*it)->parent()->text (0);

            for(int i=0;i<ChannelItemDataList.size ();i++)
            {
                if(ChannelItemDataList[i]->name==name&&
                        ChannelItemDataList[i]->parent==parent &&
                        ChannelItemDataList[i]->isChecked)
                {
                    channels_transform_assignment(ChannelItemDataList[i]->chn , &prm);
                }
            }
        }
        ++it;
    }

}

void Plot::markers_init()
{
    //    GtkDatabox *databox = GTK_DATABOX(plot->priv->databox);
    struct marker_type *markers = priv->markers;
    const char *empty_text = " ";
    char buf[10];
    int i;

    /* Clear marker information text box */
    //        if (priv->tbuf)
    //            priv->tbuf->setPlainText (empty_text);

    priv->markers_copy = NULL;

    /* Don't go any further with the init when in TIME or XY domains*/
    if (priv->active_transform_type == TIME_TRANSFORM ||
            priv->active_transform_type == CONSTELLATION_TRANSFORM)
        return;

    /* Ensure that Marker Image is applied only to Complex FFT Transforms */
    if (priv->active_transform_type == FFT_TRANSFORM && priv->marker_type == MARKER_IMAGE)
        priv->marker_type = MARKER_OFF;

    //	for (i = 0; i <= MAX_MARKERS; i++) {
    //		markers[i].x = 0.0f;
    //		markers[i].y = 0.0f;
    //		if (markers[i].graph)
    //			g_object_unref(markers[i].graph);
    //		markers[i].graph = gtk_databox_markers_new(1, &markers[i].x, &markers[i].y, &color_marker,
    //			10, GTK_DATABOX_MARKERS_TRIANGLE);
    //		gtk_databox_graph_add(databox, markers[i].graph);
    //		gtk_databox_graph_set_hide(markers[i].graph, true);
    //		sprintf(buf, "?%i", i);
    //		gtk_databox_markers_set_label(GTK_DATABOX_MARKERS(markers[i].graph),
    //			0, GTK_DATABOX_MARKERS_TEXT_N, buf, FALSE);
    //		if (priv->marker_type == MARKER_OFF)
    //			gtk_databox_graph_set_hide(markers[i].graph, TRUE);
    //		else
    //			gtk_databox_graph_set_hide(markers[i].graph, !markers[i].active);
    //	}
    //	if (priv->marker_type != MARKER_OFF)
    //		set_marker_labels(plot, NULL, priv->marker_type);

}

void Plot::device_rx_info_update()
{
    //    GtkTextIter iter;
    char text[256];
    unsigned int i, num_devices = 0;

    priv->devices_buf->setPlainText ("");
    //    gtk_text_buffer_get_iter_at_line(priv->devices_buf, &iter, 1);

    if (ctx)
        num_devices = iio_context_get_devices_count(ctx);

    //    OSCMain osc;

    ui->txt_devices->setPlainText("");
    for (i = 0; i < num_devices; i++) {
        iio_device *dev =iio_context_get_device(ctx, i);
        const char *name = iio_device_get_name(dev) ?: iio_device_get_id(dev);
        extra_dev_info *dev_info =( extra_dev_info *) iio_device_get_data(dev);

        double freq, percent, seconds;
        char freq_prefix, sec_prefix;

        if (dev_info == 0x00)
            continue;

        if (dev_info->input_device == false)
            continue;

        freq = dev_info->adc_freq * prefix2scale(dev_info->adc_scale);
        freq = freq / ui->cmb_fft_size->currentText ().toInt ();

        seconds = 1 / freq;
        percent = seconds * priv->fps * 100.0;
        if (freq > 1e6) {
            freq = freq / 1e6;
            freq_prefix = 'M';
        } else if (freq > 1e3) {
            freq = freq / 1e3;
            freq_prefix = 'k';
        } else
            freq_prefix = ' ';
        if (seconds < 1e-6) {
            seconds = seconds * 1e9;
            sec_prefix = 'n';
        } else if (seconds < 1e-3) {
            seconds = seconds * 1e6;
            sec_prefix = 'u';
        } else {
            seconds = seconds * 1e3;
            sec_prefix = 'm';
        }

        snprintf(text, sizeof(text), "%s:\n\tSampleRate: %3.2f %cSPS\n"
                                     "\tHz/Bin: %3.2f %cHz\n"
                                     "\tSweep: %3.2f %cs (%2.2f%%)\n"
                                     "\tFPS: %2.2f\n",
                 name, dev_info->adc_freq, dev_info->adc_scale,
                 freq, freq_prefix, seconds, sec_prefix, percent, priv->fps);

        //        qInfo()<<"device_rx_info_update -> "<<text;

        ui->txt_devices->setPlainText(ui->txt_devices->toPlainText()+"\n"+text);

    }
}

void Plot::update_grid(gfloat left, gfloat right)
{
    if (priv->active_transform_type == FFT_TRANSFORM ||
            priv->active_transform_type == COMPLEX_FFT_TRANSFORM) {
        gfloat spacing;

        spacing = ceil((right - left) / 130) * 10;
        if (spacing < 10)
            spacing = 10;
        fill_axis(priv->gridx, left, spacing, 14);
        fill_axis(priv->gridy, 10, -10, 25);
    }
}

void Plot::osc_plot_update_rx_lbl(bool initial_update)
{
    QList<Transform*> *tr_list = priv->transform_list;
    struct extra_dev_info *dev_info = NULL;
    char buf[20];
    double corr;
    int i;

    device_rx_info_update();

    //	/* Skip rescaling graphs, updating labels and others if the redrawing is currently halted. */
    if (priv->redraw_function <= 0 && !initial_update)
        return;

    if (is_frequency_transform()) {
        gfloat top, bottom, left, right;
        gfloat padding;

        DataTypes datatype;
        //		/* In FFT mode we need to scale the x-axis according to the selected sampling frequency */
        for (i = 0; i < tr_list->size(); i++) {
            if(!initial_update)
                datatype.Transform_setup(tr_list->at(i));
            //gtk_databox_graph_set_hide(tr_list->transforms[i]->graph, TRUE);
        }

        dev_info =( extra_dev_info *) iio_device_get_data(transform_get_device_parent(tr_list->at(i - 1)));
        sprintf(buf, "%cHz", dev_info->adc_scale);
        //		gtk_label_set_text(GTK_LABEL(priv->hor_scale), buf);

        if (priv->active_transform_type == COMPLEX_FFT_TRANSFORM)
            corr = dev_info->adc_freq / 2.0;
        else
            corr = 0;

        //		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->enable_auto_scale)) && !initial_update)
        //			return;

        if (priv->profile_loaded_scale)
            return;

        update_grid(-corr, dev_info->adc_freq / 2.0);

        padding = (dev_info->adc_freq / 2.0 + corr) * 0.05;

        //		gtk_databox_get_total_limits(GTK_DATABOX(priv->databox), &left, &right,
        //				&top, &bottom);
        //		gtk_databox_set_total_limits(GTK_DATABOX(priv->databox),
        //				-corr - padding, dev_info->adc_freq / 2.0 + padding,
        //				top, bottom);
        //	} else {
        //            switch (gtk_combo_box_get_active(GTK_COMBO_BOX(priv->hor_units))) {
        //            case 0:
        //                //ui->txt_hor_scale, "Samples");
        //                break;
        //            case 1:
        //                //gtk_label_set_text(GTK_LABEL(priv->hor_scale), "µs");
        //                break;
        //            }
    }
}

bool Plot:: is_frequency_transform()
{
    return priv->active_transform_type == FFT_TRANSFORM ||
            priv->active_transform_type == COMPLEX_FFT_TRANSFORM ||
            priv->active_transform_type == FREQ_SPECTRUM_TRANSFORM;
}

void Plot::transform_add_plot_markers( Transform *transform)
{
    transform->has_the_marker = true;
    priv->tr_with_marker = transform;
    if (priv->active_transform_type == FFT_TRANSFORM ||
            priv->active_transform_type == COMPLEX_FFT_TRANSFORM) {
        FFT_SETTINGS(transform)->markers = priv->markers;
        FFT_SETTINGS(transform)->markers_copy = &priv->markers_copy;
        FFT_SETTINGS(transform)->marker_type = &priv->marker_type;
        FFT_SETTINGS(transform)->marker_lock = &priv->g_marker_copy_lock;
    } else if (priv->active_transform_type == CROSS_CORRELATION_TRANSFORM) {
        XCORR_SETTINGS(transform)->markers = priv->markers;
        XCORR_SETTINGS(transform)->markers_copy = &priv->markers_copy;
        XCORR_SETTINGS(transform)->marker_type = &priv->marker_type;
        XCORR_SETTINGS(transform)->marker_lock = &priv->g_marker_copy_lock;
    } else if (priv->active_transform_type == FREQ_SPECTRUM_TRANSFORM) {
        FREQ_SPECTRUM_SETTINGS(transform)->markers = priv->markers;
        FREQ_SPECTRUM_SETTINGS(transform)->markers_copy = &priv->markers_copy;
        FREQ_SPECTRUM_SETTINGS(transform)->marker_type = &priv->marker_type;
        FREQ_SPECTRUM_SETTINGS(transform)->marker_lock = &priv->g_marker_copy_lock;
    }
}

void Plot::transform_add_own_markers(Transform *transform)
{
    struct marker_type *markers;
    int i;

    markers =(marker_type *) calloc(sizeof(struct marker_type), MAX_MARKERS + 2);
    if (!markers) {
        fprintf(stderr,
                "Error: could not alloc memory for markers in %s\n",
                __func__);
        return;
    }

    for (i = 0; i < MAX_MARKERS; i++)
        markers[i].active = (i <= 4);

    if (transform->type_id == FFT_TRANSFORM ||
            transform->type_id == COMPLEX_FFT_TRANSFORM) {
        FFT_SETTINGS(transform)->markers = markers;
        FFT_SETTINGS(transform)->marker_type = FFT_SETTINGS(
                    priv->tr_with_marker)->marker_type;
    } else if (transform->type_id == CROSS_CORRELATION_TRANSFORM) {
        XCORR_SETTINGS(transform)->markers = markers;
        XCORR_SETTINGS(transform)->marker_type = XCORR_SETTINGS(
                    priv->tr_with_marker)->marker_type;
    }
}

void Plot::plot_setup()
{
    QList<Transform*> *tr_list = priv->transform_list;
    Transform *transform;
    QVector<double> *transform_x_axis;
    QVector<double> *transform_y_axis;
    unsigned int max_x_axis = 0;
    int i;

    markers_init();

    DataTypes dataType;
    for (i = 0; i < tr_list->size(); i++) {
        transform = tr_list->at(i);

        QCPGraph *graph=nullptr;

        dataType.Transform_setup(transform);
        transform_x_axis = dataType.Transform_get_x_axis_ref(transform);
        transform_y_axis = dataType.Transform_get_y_axis_ref(transform);

        QString plot_type_str = ui->cmb_graph_type->currentText ();

        if (plot_type_str=="Lines") {
            graph=ui->timeChart->addGraph();
            graph->setPen(QPen(*transform->graph_color));
        }

        if (is_frequency_transform()){
            QCPGraph *fftgraph = ui->fftChart->addGraph();
            if (fftgraph && transform->graph_color)
                fftgraph->setPen(QPen(*transform->graph_color));
            transform->fftgraph = fftgraph;
        }

        transform->graph = graph;

        if (transform->x_axis_size > max_x_axis)
            max_x_axis = transform->x_axis_size;

        if (is_frequency_transform() ||
                priv->active_transform_type == CROSS_CORRELATION_TRANSFORM) {
            if (i == 0)
                transform_add_plot_markers(transform);
            else
                transform_add_own_markers(transform);
        }

        //		gtk_databox_graph_set_hide(graph, TRUE);
        //		gtk_databox_graph_add(GTK_DATABOX(priv->databox), graph);
    }
    if (!priv->profile_loaded_scale) {
        //		if (priv->active_transform_type == TIME_TRANSFORM &&
        //			!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->enable_auto_scale)))
        //			gtk_databox_set_total_limits(GTK_DATABOX(priv->databox), 0.0, max_x_axis,
        //				(int)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->y_axis_max))),
        //				(int)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->y_axis_min))));
        //		else if (priv->active_transform_type == CONSTELLATION_TRANSFORM &&
        //			!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->enable_auto_scale)))
        //			gtk_databox_set_total_limits(GTK_DATABOX(priv->databox), -1000.0, 1000.0, 1000, -1000);
        //		else if (priv->active_transform_type == FREQ_SPECTRUM_TRANSFORM) {
        //			double end_freq = priv->start_freq + priv->filter_bw * priv->fft_count;
        //			double width = end_freq - priv->start_freq;
        //			gtk_databox_set_total_limits(GTK_DATABOX(priv->databox),
        //				priv->start_freq - 0.05 * width, end_freq + 0.05 * width,
        //				0.0, -100.0);
        //		}
    }

    //    osc_plot_update_rx_lbl(INITIAL_UPDATE);

    bool show_phase_info = false;

    if (priv->active_transform_type == COMPLEX_FFT_TRANSFORM &&
            priv->transform_list->size() == 2) {
        show_phase_info = true;
    }

    //	notebook_info_set_page_visibility(GTK_NOTEBOOK(
    //		gtk_builder_get_object(priv->builder, "notebook_info")),
    //		2, show_phase_info);

}

void Plot::update_plot(struct iio_buffer *buf)
{

    if (osc_plot_get_buffer() == buf) {
        osc_plot_data_update();
    }
}

void Plot::fps_counter()
{
    struct timeval now, diff;

    priv->frame_counter++;
    if (gettimeofday(&now, NULL) == -1) {
        printf("err with gettimeofdate()\n");
        return;
    }
    if (!priv->fps) {
        priv->last_update.tv_sec = now.tv_sec;
        priv->last_update.tv_usec = now.tv_usec;
        priv->fps = -1.0;
        priv->frame_counter = 0;
        return;
    }

    timersub(&now, &priv->last_update, &diff);

    if (diff.tv_sec >= 5 || priv->fps == -1.0) {
        double tmp =  priv->frame_counter / (diff.tv_sec + diff.tv_usec / 1000000.0);
        priv->fps = tmp;
        priv->frame_counter = 0;
        priv->last_update.tv_sec = now.tv_sec;
        priv->last_update.tv_usec = now.tv_usec;

        device_rx_info_update();
    }
}

iio_device * Plot::transform_get_device_parent(Transform *transform)
{
    struct iio_device *iio_dev = NULL;
    PlotChn *plot_ch;

    if (!transform || !transform->plot_channels)
        return NULL;

    plot_ch=(PlotChn *)transform->plot_channels->data;

    if (plot_ch)
        iio_dev = plot_ch->get_iio_parent(plot_ch);

    return iio_dev;
}

void Plot::draw_marker_values(Transform *tr)
{

    return;
    struct iio_device *iio_dev;
    struct extra_dev_info *dev_info;
    struct marker_type *markers;
    char text[256];
    int markers_scale;
    double lo_freq;
    int m;

    if (tr->type_id == CROSS_CORRELATION_TRANSFORM)
        markers = XCORR_SETTINGS(tr)->markers;
    else if (tr->type_id == FREQ_SPECTRUM_TRANSFORM)
        markers = FREQ_SPECTRUM_SETTINGS(tr)->markers;
    else if(tr->type_id == FFT_TRANSFORM)
        markers = FFT_SETTINGS(tr)->markers;
    else if(tr->type_id == COMPLEX_FFT_TRANSFORM)
        markers = FFT_SETTINGS(tr)->markers;
    else
        return;

    if (priv->tbuf == NULL) {
        priv->tbuf = NULL;
        //        ui->txt_markers->setPlainText (priv->tbuf->toPlainText ());
    }

    iio_dev = transform_get_device_parent(tr);

    if (!iio_dev) {
        fprintf(stderr,
                "Error: Could not find iio device parent for the given transform.%s\n",
                __func__);
        return;
    }

    dev_info =static_cast<extra_dev_info *>(iio_device_get_data(iio_dev));

    /* Get the LO frequency stored by a iio channel which is used by
         * this transform. All channels should have the same lo freq. */
    lo_freq = 0.0;
    if (tr->plot_channels && g_slist_length(tr->plot_channels)) {
        PlotChn *p = PLOT_CHN(tr->plot_channels->data);
        if (p->type == PLOT_IIO_CHANNEL) {
            struct iio_channel *ch;
            struct extra_info *ch_info;
            ch = PLOT_IIO_CHN(p)->iio_chn;
            if (ch) {
                ch_info =(extra_info *) iio_channel_get_data(ch);
                if (ch_info)
                    lo_freq = ch_info->lo_freq;
            }
        }
    }

    markers_scale = prefix2scale(dev_info->adc_scale);

    if (MAX_MARKERS && priv->marker_type != MARKER_OFF) {
        for (m = 0; m <= MAX_MARKERS && markers[m].active; m++) {
            if (tr->type_id == FFT_TRANSFORM || tr->type_id == COMPLEX_FFT_TRANSFORM) {
                sprintf(text, "%s: %2.2f dBFS @ %2.3f %cHz%c",
                        markers[m].label, markers[m].y,
                        lo_freq / markers_scale + markers[m].x,
                        dev_info->adc_scale,
                        m != MAX_MARKERS ? '\n' : '\0');
            } else if (tr->type_id == CROSS_CORRELATION_TRANSFORM) {
                sprintf(text, "M%i: %1.6f @ %2.3f%c", m, markers[m].y, markers[m].x,
                        m != MAX_MARKERS ? '\n' : '\0');
            } else if (tr->type_id == FREQ_SPECTRUM_TRANSFORM) {
                sprintf(text, "M%i: %1.6f @ %2.3f%c", m, markers[m].y, markers[m].x,
                        m != MAX_MARKERS ? '\n' : '\0');
            }

            if (m == 0) {
                //                qInfo()<<"text : "<<text;
                //priv->tbuf->setPlainText (text);
                //gtk_text_buffer_get_iter_at_line(priv->tbuf, &iter, 1);
            } else {
                //                qInfo()<<"text : "<<text;
                //priv->tbuf->setPlainText (text);
            }

        }
    } else {
        //        priv->tbuf->setPlainText("No markers active");
    }
}

void Plot::markers_phase_diff_show()
{
    static float avg[MAX_MARKERS] = {NAN};

    char text[256];
    int m;
    struct marker_type *trA_markers;
    struct marker_type *trB_markers;
    float angle_diff, lead_lag;
    float filter, angle;

    priv->phase_buf->setPlainText ("");
    //gtk_text_buffer_get_iter_at_line(priv->phase_buf, &iter, 1);

    if (priv->active_transform_type == COMPLEX_FFT_TRANSFORM &&
            priv->transform_list->size() == 2) {

        trA_markers = FFT_SETTINGS(
                    priv->transform_list->at(0))->markers;
        trB_markers = FFT_SETTINGS(
                    priv->transform_list->at(1))->markers;

        filter =  ui->txt_fft_avg->value ();
        if (!filter)
            filter = 1;
        filter = 1.0 / filter;

        if (MAX_MARKERS && priv->marker_type != MARKER_OFF) {
            for (m = 0; m < MAX_MARKERS &&
                 trA_markers[m].active; m++) {

                /* find out the quadrant
                                 * since carg() returns something from [-pi, +pi], use that.
                                 * this handles reflex angles up to [-2*pi, +2*pi]
                                 */
                lead_lag = (cargf(trA_markers[m].vector[0]) - cargf(trB_markers[m].vector[0])) * 180 / M_PI;

                /* [-2*pi, +2*pi] is kind of silly
                                 * move things to [-pi, +pi]
                                 */
                if (lead_lag > 180)
                    lead_lag = lead_lag - 360;
                if (lead_lag < -180)
                    lead_lag = 360 + lead_lag;

                //				if (isnan(avg[m]))
                //					avg[m] = lead_lag;

                /* Cosine law, answers are [0, +pi] */
                if (cabsf(trA_markers[m].vector[0]) == 0.0 || cabsf(trB_markers[m].vector[0]) == 0.0) {
                    /* divide by 0 is nan */
                    angle_diff = lead_lag;
                } else {
                    angle_diff =  acosf((crealf(trA_markers[m].vector[0]) * crealf(trB_markers[m].vector[0]) +
                            cimagf(trA_markers[m].vector[1]) * cimagf(trB_markers[m].vector[1])) /
                            (cabsf(trA_markers[m].vector[0]) * cabsf(trB_markers[m].vector[0]))) * 180 / M_PI;
                }

                /* put back into the correct quadrant */
                if (lead_lag < 0)
                    angle_diff *= -1.0;

                if (lead_lag > 180.0)
                    angle_diff = 360.0 - angle_diff;

                avg[m] = ((1 - filter) * avg[m]) + (filter * angle_diff);

                angle = avg[m];
                if (angle > 180.0)
                    angle -= 360.0;
                if (angle < -180.0)
                    angle += 360.0;

                trA_markers[m].angle = angle;
                trB_markers[m].angle = angle;

                snprintf(text, sizeof(text),
                         "%s: %02.3f° @ %2.3f %cHz %c",
                         trA_markers[m].label,
                         angle,
                         /* lo_freq / markers_scale */ trA_markers[m].x,
                         /*dev_info->adc_scale */ 'M',
                         m != (MAX_MARKERS - 1) ? '\n' : '\0');

                priv->phase_buf->setPlainText (text);
            }
        } else {
            priv->phase_buf->setPlainText ("No markers active");
        }
    }
}

bool Plot::call_all_transform_functions()
{
    QList<Transform*> *tr_list = priv->transform_list;
    Transform *tr;
    bool valid = true;
    bool tr_valid;
    int i = 0;

    if (priv->redraw_function <= 0)
        return false;

    DataTypes dataType;
    for (; i < tr_list->size(); i++) {
        tr = tr_list->at(i);
        tr_valid = dataType.Transform_update_output(tr);
        //		if (tr_valid)
        //			gtk_databox_graph_set_hide(tr->graph, FALSE);
        valid &= tr_valid;
    }

    return valid;
}

void Plot::osc_plot_data_update ()
{
    if (call_all_transform_functions())
        priv->redraw = TRUE;

    if (priv->single_shot_mode) {
        priv->single_shot_mode = false;
        ui->btn_capture->setChecked(false);
    }
}

gboolean Plot::plot_redraw()
{
    QList<Transform*> *tr_list = priv->transform_list;
    Transform *tr;
    bool show_diff_phase = false;
    int i;

    if (priv->redraw) {

        fps_counter();

        for (i = 0; i < tr_list->size(); i++) {

            tr = tr_list->at(i);

            if (!tr->has_the_marker) {
                show_diff_phase = true;
                draw_marker_values( tr);
            }
        }

        if (show_diff_phase)
            markers_phase_diff_show();

    }

    if (priv->stop_redraw == TRUE)
        priv->redraw_function = 0;

    priv->redraw = FALSE;
    return !priv->stop_redraw;
}

void Plot::dispose_parameters_from_plot()
{
    plot_params *prms;
    GSList *node;
    GSList *del_link = NULL;
    unsigned int i;

    for (i = 0; i < iio_context_get_devices_count(ctx); i++) {
        iio_device *dev = iio_context_get_device(ctx, i);
        extra_dev_info *info =(extra_dev_info *) iio_device_get_data(dev);
        GSList *list = info->plots_sample_counts;

        if (info->input_device == false)
            continue;

        for (node = list; node; node = g_slist_next(node)) {
            prms =(plot_params *) node->data;
            if (prms->plot_id == priv->object_id) {
                del_link = node;
                break;
            }
        }
        if (del_link) {
            list = g_slist_delete_link(list, del_link);
            info->plots_sample_counts = list;
        }
    }
}

void Plot::deassert_used_channels()
{
    Transform *tr;
    GSList *node;
    int i;

    for (i = 0; i < priv->transform_list->size(); i++) {
        tr = priv->transform_list->at(i);
        /* Disable iio channels used by the transform */
        for (node = tr->plot_channels; node; node = g_slist_next(node)) {
            PlotChn *plot_ch;

            plot_ch =(PlotChn *) node->data;
            if (plot_ch)
                plot_ch->assert_used_iio_channels(plot_ch, false);
        }
    }
}

gboolean Plot::check_valid_setup()
{
    if (!check_valid_setup_of_all_devices())
        goto capture_button_err;

    //    if (ui->btn_capture->isChecked ())
    //        g_object_set(ui->btn_capture, "stock-id", "gtk-stop", NULL);
    //    else
    //        g_object_set(ui->btn_capture, "stock-id", "gtk-media-play", NULL);
    ui->btn_capture->setToolTip ("Capture / Stop");

    //    g_object_set(ui->btn_ss, "stock-id", "gtk-media-next", NULL);
    //    ui->btn_ss->setToolTip ("Single Shot Capture");

    //    if (!priv->capture_button_hid) {
    //        //		priv->capture_button_hid = g_signal_connect(priv->capture_button, "toggled",
    //        //			G_CALLBACK(capture_button_clicked_cb), plot);

    //        priv->deactivate_capture_btn_flag = 0;
    //    }

    return true;

capture_button_err:
    //	g_object_set(priv->capture_button, "stock-id", "gtk-dialog-warning", NULL);
    //	g_object_set(priv->ss_button, "stock-id", "gtk-dialog-warning", NULL);
    if (priv->capture_button_hid) {
        //		g_signal_handler_disconnect(priv->capture_button, priv->capture_button_hid);
        priv->deactivate_capture_btn_flag = 1;
    }
    priv->capture_button_hid = 0;

    return false;
}

void Plot::single_shot_clicked_cb(gpointer data)
{
    priv->single_shot_mode = true;
    //	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(priv->capture_button), true);
}

void Plot::osc_plot_set_visible (Plot *plot, bool visible)
{
    plot->setVisible (visible);
}

Plot * Plot::osc_plot_new()
{
    create_plot();

    ShowControls(0);

    //saeid raziani ==> these lines are for showing frequency domain plot. by default when program is ran, time domain is shown
    if(Mode == FRQ_DOMAIN)
    {
        on_cmb_plot_type_currentIndexChanged(1);
        ui->cmb_plot_type->setCurrentIndex(1);
    }


    return this;
}

Plot * Plot::osc_plot_new_with_pref(OscPlotPreferences *pref)
{
    priv->preferences = pref;

    create_plot();

    return this;
}

// Initialize and show saveAs dialog
void Plot::saveas_channels_list_fill()
{
    //	GtkWidget *ch_window;
    //	GtkWidget *vbox;
    unsigned int num_devices = 0;
    unsigned int i;

    if (ctx)
        num_devices = iio_context_get_devices_count(ctx);

    //    ch_window = priv->viewport_saveas_channels;
    //    vbox = gtk_vbox_new(FALSE, 10);
    //    gtk_container_add(GTK_CONTAINER(ch_window), vbox);
    //    priv->device_combobox = gtk_combo_box_text_new();
    //    gtk_box_pack_start(GTK_BOX(vbox), priv->device_combobox, FALSE, TRUE, 0);
    //    priv->saveas_channels_list = gtk_vbox_new(FALSE, 0);
    //    gtk_box_pack_end(GTK_BOX(vbox), priv->saveas_channels_list, FALSE, TRUE, 0);

    for (i = 0; i < num_devices; i++) {
        struct iio_device *dev = iio_context_get_device(ctx, i);
        const char *name = iio_device_get_name(dev) ?:
                                                     iio_device_get_id(dev);
        extra_dev_info *dev_info =(extra_dev_info * )iio_device_get_data(dev);

        //        if (dev_info->input_device == false)
        //            continue;

        //        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(priv->device_combobox), name);
    }

    //    if (num_devices == 0)
    //        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(priv->device_combobox),
    //            "No Devices Available");
    //    g_signal_connect(priv->device_combobox, "changed",
    //        G_CALLBACK(saveas_device_changed_cb), (gpointer)plot);
    //    gtk_combo_box_set_active(GTK_COMBO_BOX(priv->device_combobox), 0);
    //    gtk_widget_set_size_request(priv->viewport_saveas_channels, -1, 150);
    //    gtk_widget_show_all(vbox);

}

/**
 * @brief if chart horizontal range changed then change plot range horizontally and vertically
 * @param type : 1=> time domain chart , 2=> frequency domain chart
 */
void Plot::reScale_plot(int type){

    if(type==1)
    {
        //        if(abs(rangeXTimeChart.lower - rangeXTimeChart.upper)<1 )
        //            return;

        //        for (int i = 0; i < ui->timeChart->graphCount(); ++i)
        //        {
        //            ui->timeChart->xAxis->setRange(rangeXTimeChart.lower, rangeXTimeChart.upper);
        //            ui->timeChart->yAxis->setRange(rangeYTimeChart.lower, rangeYTimeChart.upper);
        //        }
    }
    else
    {


        if ((*priv->transform_list).size() <= 0) return;

        //==========================================================
        //--------------------[ saeid raziani ]---------------------
        //==========================================================

        //this section find the maximum frequency between the area that user selects

        QCPItemText * txtLbl = new QCPItemText(ui->fftChart);
        txtLbl->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
        txtLbl->position->setType(QCPItemPosition::ptAxisRectRatio);
        txtLbl->position->setCoords(0.02, 0);
        txtLbl->setFont(QFont(font().family(), 12));
        txtLbl->setColor(Qt::red);
        txtLbl->setText("");

        int minIndexNum{};
        int maxIndexNum{};

        int axisMax{};

        double lowVal = rangeXFFTChart.lower;
        double upVal  = rangeXFFTChart.upper;
        int maxNumber {-1000};


        for (int i = 0; i < (*priv->transform_list)[0]->x_axis->size(); i++)
        {
            if(minIndexNum != 0 and maxIndexNum != 0) break; // it means we found min and max index

            double val = (*(*priv->transform_list)[0]->x_axis)[i];

            if (qAbs(val- lowVal) <= 0.1)
            {
                minIndexNum =  i;
            }

            if (qAbs(val- upVal) <= 0.1)
            {
                maxIndexNum =  i;
            }
        }

        for(int i{minIndexNum} ; i < maxIndexNum; i++)
        {
            double val = (*(*priv->transform_list)[0]->y_axis)[i];
            if(val > maxNumber)
            {
                maxNumber = val;
                axisMax = i;
            }
        }




        QString pVal ="P: " +
                QString::number((*(*priv->transform_list)[0]->y_axis)[axisMax])+" dBFS"+
                "    "+QString::number((*(*priv->transform_list)[0]->x_axis)[axisMax])+" MHz";

        txtLbl->setText (txtLbl->text()+"\n "+pVal);



        maximumFrq = (*(*priv->transform_list)[0]->x_axis)[axisMax];//this is maximum frq



        int mul =- 3;

        QCPItemText *textLabel = new QCPItemText(ui->fftChart);
        textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);


        if(priv->transform_list->size() != 0) textLabel->position->setCoords((*(*priv->transform_list)[0]->x_axis)[axisMax]+mul, (*(*priv->transform_list)[0]->y_axis)[axisMax]+mul);

        textLabel->setText("p: "+  QString::number((*(*priv->transform_list)[0]->y_axis)[axisMax]));
        textLabel->setFont(QFont(font().family(), 15));
        textLabel->setPen(QPen(Qt::red));
        textLabel->setColor(Qt::red);

        emit ampValueSignal((*(*priv->transform_list)[0]->y_axis)[axisMax]);
        //start timer
        if(!selectedSmartNoiseTimer.isActive())
        {
            selectedSmartNoiseTimer.start();
        }

        selectingNoiseIsActive = true;

        ui->fftChart->xAxis->setRange(rangeXFFTChart.lower, rangeXFFTChart.upper);
        ui->fftChart->yAxis->setRange(rangeYFFTChart.lower, rangeYFFTChart.upper);
    }
}



QCPItemText * Plot::createTextOnPlot()
{
    QCPItemText * txtLbl = new QCPItemText(ui->fftChart);
    txtLbl->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    txtLbl->position->setType(QCPItemPosition::ptAxisRectRatio);
    txtLbl->position->setCoords(0.02, 0);
    txtLbl->setFont(QFont(font().family(), 12));
    txtLbl->setColor(Qt::yellow);
    txtLbl->setText("");
    return txtLbl;
}
void Plot::count_changed_cb()
{
    extra_dev_info *dev_info;
    gdouble freq = 0;

    //    if (priv->current_device) {
    //        dev_info =( extra_dev_info *) iio_device_get_data(priv->current_device);
    //        freq = dev_info->adc_freq * prefix2scale(dev_info->adc_scale);
    //    }

    switch(ui->cmb_sample_count_units->currentIndex ()) {
    case HOR_SCALE_SAMPLES:
        priv->sample_count = (int)ui->sample_count_widget->value ();
        break;
    case HOR_SCALE_TIME:
        priv->sample_count = (int)round((ui->sample_count_widget->value () *
                                         freq) / pow(10.0, 6));
        break;
    }

}

#pragma endregion }

#pragma region Channels TreeView {

/**
 * @brief Get device Channels Count
 * @param name
 * @return
 */
int Plot::num_of_channels_of_device(const char *name)
{

    QTreeWidgetItemIterator it(ui->tree_channel_list);

    int channelCount=0;
    while(*it)
    {
        if((*it)->parent())
            if((*it)->parent()->text (0)==name)
            {
                channelCount++;
            }
        ++it;
    }

    return channelCount;

}

/**
 * @brief Plot::enabled_channels_of_device
 * @param name
 * @param enabled_mask
 * @return
 */
int Plot::enabled_channels_of_device(QString name, unsigned *enabled_mask)
{
    int num_enabled = 0;

    if (enabled_mask)
        *enabled_mask = 0;

    for(int i=0;i<ChannelItemDataList.size ();i++)
    {
        if(ChannelItemDataList[i]->parent==name)
        {
            if(ChannelItemDataList[i]->isChecked)
                num_enabled++;
        }
    }

    return num_enabled;
}

/**
 * @brief Plot::enabled_channels_count
 * @return
 */
int Plot::enabled_channels_count()
{
    gboolean next_iter;
    QString dev_name;
    int count = 0;

    QTreeWidgetItemIterator it(ui->tree_channel_list);

    while(*it)
    {
        if(!(*it)->parent ())
        {

            dev_name=(*it)->text(0);

            count += enabled_channels_of_device(dev_name, NULL);
        }
        ++it;
    }

    return count;
}

/**;
 * @brief Plot::check_valid_setup_of_device
 * @param name
 * @return
 */
gboolean Plot::check_valid_setup_of_device(const char *name)
{

    int plot_type;
    int num_enabled;
    struct iio_device *dev;
    unsigned int nb_channels = num_of_channels_of_device(name);
    unsigned enabled_channels_mask;

    gboolean device_enabled=enabled_channels_count()>0;

    plot_type = ui->cmb_plot_type->currentIndex ();

    if (!device_enabled && plot_type != TIME_PLOT)
        return true;

    num_enabled = enabled_channels_of_device(name, &enabled_channels_mask);

    /* Basic validation rules */
    if (plot_type == FFT_PLOT) {
        if (num_enabled != 4 && num_enabled != 2 && num_enabled != 1) {
            ui->btn_capture->setToolTip ("FFT needs 4 or 2 channels");
            return false;
        }
    } else if (plot_type == XY_PLOT) {
        if (num_enabled != 2) {
            ui->btn_capture->setToolTip ("Constellation requires only 2 channels");
            return false;
        }
    } else if (plot_type == TIME_PLOT) {
        //            if (enabled_channels_count() == 0) {
        //                ui->btn_capture->setToolTip ("Time Domain needs at least one channel");
        //                return false;
        //            } else if (dev && !dma_valid_selection(name, enabled_channels_mask | global_enabled_channels_mask(dev), nb_channels)) {
        //                ui->btn_capture->setToolTip ("Channel selection not supported");
        //                return false;
        //            }
    } else if (plot_type == XCORR_PLOT) {
        if (enabled_channels_count() != 4) {
            ui->btn_capture->setToolTip ("Correlation requires 4 channels");
            return false;
        }
    }

    /* No additional checking is needed for non iio devices */
    //        if (!dev)
    //            return TRUE;

    char warning_text[100];

    /* Check if devices that need a trigger have one and it's configured */
    const struct iio_device *trigger;
    int ret;

    //    ret = osc_iio_device_get_trigger(dev, &trigger);
    //    if (ret == 0 && trigger == NULL && num_enabled > 0) {
    //        snprintf(warning_text, sizeof(warning_text),
    //                 "Device %s needs an impulse generator", name);
    //        ui->btn_capture->setToolTip (warning_text);
    //        return false;
    //    }

    /* Additional validation rules provided by the plugin of the device */
    if (num_enabled > 1 || plot_type == TIME_PLOT)
        return true;

    bool valid_comb;
    const char *ch_names[2];

    //        plugin_setup_validation_fct = find_setup_check_fct_by_devname(name);

    //    if (plugin_setup_validation_fct) {
    //        foreach_channel_iter_of_device(GTK_TREE_VIEW(priv->channel_list_view),
    //                                       name, *set_may_be_enabled_bit, NULL);
    //        valid_comb = (*plugin_setup_validation_fct)(dev, ch_names);
    //        if (!valid_comb) {
    //            snprintf(warning_text, sizeof(warning_text),
    //                     "Combination between %s and %s is invalid", ch_names[0], ch_names[1]);
    //            ui->btn_capture->setToolTip (warning_text);
    //            return false;
    //        }
    //    }

    //    if (num_enabled && plot_type == FFT_PLOT && !ui->btn_capture->isChecked ()) {
    //        GtkListStore *liststore;
    //        int i, j, k = 0, m = 0;
    //        char buf[256];

    //        j = comboboxtext_get_active_text_as_int(GTK_COMBO_BOX_TEXT(priv->fft_size_widget));
    //        liststore = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(priv->fft_size_widget)));
    //        gtk_list_store_clear(liststore);

    //        i = 4194304;
    //        /* make sure we don't exceed DMA, 2^22 bytes (not samples) */
    //        while (i >= 64) {
    //            if (i * num_enabled * 2 <= 4194304) {
    //                sprintf(buf, "%i", i);
    //                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(priv->fft_size_widget), buf);

    //                if (i == j)
    //                    m = k;
    //                k++;
    //            }
    //            i = i / 2;
    //        }
    //        gtk_combo_box_set_active(GTK_COMBO_BOX(priv->fft_size_widget), m);

    //    }

    return true;
}

/**
 * @brief Plot::check_valid_setup_of_all_devices
 * @return
 */
gboolean Plot::check_valid_setup_of_all_devices()
{
    gboolean valid=false;

    QTreeWidgetItemIterator it(ui->tree_channel_list);

    // For all treeview items
    while(*it)
    {
        // If treeview item has parent
        if(!(*it)->parent())
        {
            // toLatin1() is a temporary; keep it alive across the call.
            const QByteArray devName = (*it)->text(0).toLatin1();
            valid = check_valid_setup_of_device(devName.constData());
            if(valid)
                return true;
        }

        ++it;
    }

    return valid;
}

/**
 * @brief Plot::treeview_expand_update
 */
void Plot::treeview_expand_update()
{
    //	GtkTreeIter dev_iter;
    //	GtkTreeView *tree = GTK_TREE_VIEW(plot->priv->channel_list_view);
    //	GtkTreeModel *model;
    //	gboolean next_dev_iter;
    //	gboolean expanded;

    //	model = gtk_tree_view_get_model(tree);
    //	next_dev_iter = gtk_tree_model_get_iter_first(model, &dev_iter);
    //	while (next_dev_iter) {
    //		gtk_tree_model_get(model, &dev_iter, EXPANDED, &expanded, -1);
    //		expand_iter(plot, &dev_iter, expanded);
    //		next_dev_iter = gtk_tree_model_iter_next(model, &dev_iter);
    //	}
}

/**
 * @brief Plot::channel_toggled
 * @param pathStr
 * @param plot
 */
void Plot::channel_toggled( gchar* pathStr, gpointer plot)
{
    gboolean active;

    check_valid_setup();
}

/**
 * @brief Plot::onItemClicked
 * @param item
 * @param column
 */


void Plot::onItemClicked()
{
    //    QString itemName=item->text (0);
    //    QString itemparentName=item->parent()->text (0);

    //saeid raziani ==>
    try {


        QStringList itemName;
        QString itemparentName;

        if(Mode == TIME_DOMAIN)
        {
            itemparentName = "axi-adrv9009-rx-hpc";
            QStringList a = { "voltage0_i", "voltage0_q", "voltage1_i", "voltage1_q", "", ""};
            itemName = a;
        }

        else if(Mode == FRQ_DOMAIN)
        {
            itemparentName = "axi-adrv9009-rx-obs-hpc";
            QStringList a = { "", "","","","voltage0_i","voltage0_q"};
            itemName = a;
        }

        else if(Mode == SMART_TIME_DOMAIN)
        {
            itemparentName = "axi-adrv9009-rx-obs-hpc";
            QStringList a = { "", "","","","voltage0_i","voltage0_q"};
            itemName = a;
        }
        //    else if(Mode == FRQ_SEEK){
        //        itemparentName = "axi-adrv9009-rx-obs-hpc";
        //        QStringList a = { "", "","","","voltage0_i","voltage0_q"};
        //        itemName = a;
        //    }

        //    QString itemName=item->text (0);
        //    QString itemparentName=item->parent()->text (0);

        /*
         * ChannelItemDataList contains every visible IIO channel and can be
         * larger than the fixed list of channels used by these two plots.
         * The old code indexed itemName with ChannelItemDataList's index,
         * which aborts as soon as the board exposes more than six channels.
         * Match by parent/name instead so the plot configuration is independent
         * of the number and ordering of channels reported by the board.
         */
        /*
         * This function is called repeatedly during startup and mode changes.
         * Set the requested state instead of toggling it so repeated calls are
         * safe and do not corrupt shadow_of_enabled.
         */
        int j=0;
        for(int i=0; i<ChannelItemDataList.size(); ++i)
        {
            ChannelItemData *channelData = ChannelItemDataList[i];
            if(!channelData)
                continue;

            const bool wasChecked = channelData->isChecked;
            const bool shouldBeChecked =
                    (channelData->parent == itemparentName &&
                     !channelData->name.isEmpty() &&
                     itemName.contains(channelData->name));
            channelData->isChecked = shouldBeChecked;

            const QByteArray parentName = channelData->parent.toLocal8Bit();
            const QByteArray channelName = channelData->name.toLocal8Bit();

            if(!ctx)
            {
                qWarning() << "Plot: cannot configure channels because IIO context is null";
                break;
            }

            iio_device *iio_dev = iio_context_find_device(ctx, parentName.constData());
            if(!iio_dev)
            {
                qWarning() << "Plot: IIO device not found:" << channelData->parent;
                continue;
            }

            iio_channel *iio_chn = iio_device_find_channel(iio_dev, channelName.constData(), false);
            if(!iio_chn)
            {
                qWarning() << "Plot: IIO channel not found:" << channelData->name
                           << "on device" << channelData->parent;
                continue;
            }

            struct extra_info *ch_info =
                    static_cast<struct extra_info *>(iio_channel_get_data(iio_chn));
            if(!ch_info)
            {
                qWarning() << "Plot: channel extra_info is null:" << channelData->name
                           << "on device" << channelData->parent;
                continue;
            }

            if(wasChecked != shouldBeChecked)
                ch_info->shadow_of_enabled += shouldBeChecked ? 1 : -1;

            if(ch_info->shadow_of_enabled < 0)
                ch_info->shadow_of_enabled = 0;

            if(shouldBeChecked)
            {
                ch_info->listIndex=j++;
                ch_info->plotType=ui->cmb_plot_type->currentIndex();
            }

            iio_channel_set_data(iio_chn,ch_info);
        }

        check_valid_setup();
    }

    catch (...)
    {
        QMessageBox t;
        t.setText("somehing went wrong");
        t.exec();

    }


}



//void Plot::onItemClicked(QTreeWidgetItem *item, int column) {

//    // If item is in root of tree
//    if(!(item->parent()))
//    {
//        // If current plot type isn't time domain
//        if(ui->cmb_plot_type->currentIndex()!=0)
//        {
//            // Check the item
//            item->setCheckState(0,Qt::CheckState::Checked);

//            // Change Rf Bandwith label
//            if(item->text(0).toLower().contains("obs"))
//                ui->lbl_bandwidth->setText(QString::number(RFBandwidthOBS)+" MHz");
//            else
//                ui->lbl_bandwidth->setText(QString::number(RFBandwidth)+" MHz");

//            // If Item has already checked
//            if(item->checkState(0)==Qt::CheckState::Checked)
//            {

//                QTreeWidgetItemIterator it(ui->tree_channel_list);

//                // For all treeview items
//                while(*it)
//                {
//                    // If treeview item has parent
//                    if((*it)->parent())
//                    {
//                        // If treeview item's parent is not equal selected item
//                        if((*it)->parent()->text(0)!=item->text(0))
//                        {
//                            // uncheck the treeview item
//                            (*it)->setCheckState (0,Qt::Unchecked);
//                            // Disable the treeview item
//                            (*it)->setDisabled(true);
//                        }
//                        // Otherwise enable the treeview item
//                        else
//                            (*it)->setDisabled(false);
//                    }
//                    // Otherwise if treeview item isn't equal selected item
//                    else if((*it)->text(0)!=item->text(0))
//                    {
//                        (*it)->setCheckState (0,Qt::CheckState::Unchecked);
//                    }
//                    // Otherwise
//                    else
//                        (*it)->setCheckState (0,Qt::CheckState::Checked);

//                    ++it;

//                }
//            }
//        }

//        return;
//    }
//    // else if item parent didn't checked and current plot type isn't time domain
//    else if(item->parent()->checkState(0)==Qt::CheckState::Unchecked && ui->cmb_plot_type->currentIndex()!=0)
//    {
//        // Check parent of selected Item
//        item->parent()->setCheckState(0,Qt::CheckState::Checked);

//        // Change Rf Bandwith label
//        if(item->parent()->text(0).toLower().contains("obs"))
//            ui->lbl_bandwidth->setText(QString::number(RFBandwidthOBS)+" MHz");
//        else
//            ui->lbl_bandwidth->setText(QString::number(RFBandwidth)+" MHz");

//    }

//    QString itemName=item->text (0);
//    QString itemparentName=item->parent()->text (0);

//    for(int i=0;i<ChannelItemDataList.size ();i++)
//    {
//        if(ChannelItemDataList[i]->name==itemName &&
//                ChannelItemDataList[i]->parent==itemparentName)
//        {

//            ChannelItemDataList[i]->isChecked=!(ChannelItemDataList[i]->isChecked);
//            item->setCheckState(0,ChannelItemDataList[i]->isChecked?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
//        }
//    }

//    int j=0;
//    for(int i=0;i<ChannelItemDataList.size ();i++)
//    {
//        iio_device * iio_dev =(iio_device * ) iio_context_find_device(ctx, ChannelItemDataList[i]->parent.toLocal8Bit().data());
//        iio_channel * iio_chn =(iio_channel * ) iio_device_find_channel(iio_dev, ChannelItemDataList[i]->name.toLocal8Bit().data(), false);
//        struct extra_info *ch_info =(struct extra_info *) iio_channel_get_data(iio_chn);

//        ch_info->shadow_of_enabled+=ChannelItemDataList[i]->isChecked?1:-1;

//        if(ch_info->shadow_of_enabled)   {
//            ch_info->listIndex=j++;
//            ch_info->plotType=ui->cmb_plot_type->currentIndex();
//        }

//        iio_channel_set_data(iio_chn,ch_info);
//    }

//    check_valid_setup();

//}


/**
 * Add channels to devices
 */
void Plot::plot_channels_add_channel(PlotChn *pchn)
{

    QTreeWidgetItemIterator it(ui->tree_channel_list);

    while(*it)
    {

        if((*it)->text(0)==pchn->parent_name)
        {

            QTreeWidgetItem* item=new  QTreeWidgetItem();

            // Set channel text and checkbox
            item->setText (0,pchn->name);
            item->setFlags (item->flags ()|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
            item->setCheckState (0,Qt::Unchecked);

            // Set channel icon
            QPixmap icon(30,30);
            icon.fill (pchn->graph_color);
            item->setIcon (0,QIcon(icon));

            //---------------------------------

            struct iio_device *iio_dev = NULL;
            struct iio_channel *iio_chn = NULL;

            if (ctx && (iio_dev = iio_context_find_device(ctx, pchn->parent_name)))
                iio_chn = iio_device_find_channel(iio_dev, pchn->name, false);

            bool sensitive = true;
            bool active = false;

            /* Do not dereference a channel that is absent on this board image. */
            struct extra_info *ch_info = NULL;
            if(iio_chn)
                ch_info = (struct extra_info *) iio_channel_get_data(iio_chn);
            else
                qWarning() << "Plot: channel is not present in current IIO context:"
                           << pchn->parent_name << pchn->name;

            if (ch_info) {
                active = (ch_info->constraints & CONSTR_CHN_INITIAL_ENABLED);
                sensitive = !(ch_info->constraints & CONSTR_CHN_UNTOGGLEABLE);
            }

            // Collect Channel data
            ChannelItemData* itemData=new ChannelItemData();
            itemData->chn=pchn;
            itemData->name=pchn->name;
            itemData->parent=pchn->parent_name;
            itemData->isChecked=false;

            ChannelItemDataList.append(itemData);

            //---------------------------

            // Add channel to device item
            (*it)->addChild (item);

        }

        ++it;

    }
}

/**
 * Add The Devices to tree view
 */
void Plot::plot_channels_add_device(const char *dev_name)
{
    priv->deviceList.append (dev_name);

    QTreeWidgetItem *item=new QTreeWidgetItem();

    item->setText (0,dev_name);

    if(ui->cmb_plot_type->currentIndex()>0)
        item->setCheckState(0,Qt::Unchecked);

    ui->tree_channel_list->addTopLevelItem(item);
}

/**
 * @brief Plot::plot_channel_add_to_plot
 * @param settings
 */
void Plot::plot_channel_add_to_plot(PlotChn *settings)
{
    GSList *list = priv->ch_settings_list;
    int index = priv->nb_plot_channels;

    g_return_if_fail(settings);

    /* Set a default color */
    settings->graph_color=color_graph[index % NUM_GRAPH_COLORS];

    /* Add the settings to an internal list */
    list = g_slist_prepend(list, settings);
    priv->ch_settings_list = list;
    priv->nb_plot_channels++;
}

/**
 * Initialize The Devices Tree view
 */
void Plot::device_list_treeview_init()
{
    unsigned int i, j;

    QList<const char *> devices;

    priv->nb_input_devices = 0;

    if(ChannelItemDataList.size()>0)
    {
        ui->tree_channel_list->clear();
        ChannelItemDataList.clear();
        disconnect(ui->tree_channel_list,&QTreeWidget::itemClicked,this,&Plot::onItemClicked);
    }

    if (!ctx)
        goto math_channels;

    for (i = 0; i < iio_context_get_devices_count(ctx); i++) {

        iio_device *dev =iio_context_get_device(ctx, i);

        extra_dev_info *dev_info = ( extra_dev_info *) iio_device_get_data(dev);

        const char *dev_name = iio_device_get_name(dev) ?:
                                                         iio_device_get_id(dev);

        if (!dev_info)
            continue;

        if (dev_info->input_device == false)
            continue;

        if (!priv->current_device)
            priv->current_device = dev;

        iio_utils iioUtils;
        GArray *channels = iioUtils.get_iio_channels_naturally_sorted(dev);

        for (j = 0; j < channels->len; ++j) {
            iio_channel *ch = g_array_index(channels,
                                            struct iio_channel *, j);
            if (!show_channel(ch))
                continue;

            if(devices.indexOf (dev_name) == -1)
            {
                devices.append (dev_name);
                plot_channels_add_device(dev_name);
                priv->nb_input_devices++;
            }

            const char *chn_name = iio_channel_get_name(ch) ?:
                                                             iio_channel_get_id(ch);
            PlotIioChn *pic;

            pic = plot_iio_channel_new();

            if (!pic) {
                fprintf(stderr, "Could not create an iio plot"
                                "channel with name %s in function %s\n",
                        chn_name, __func__);
                break;
            }

            plot_channel_add_to_plot(PLOT_CHN(pic));

            pic->iio_chn = ch;
            pic->base.type = PLOT_IIO_CHANNEL;
            pic->base.name = g_strdup(chn_name);
            pic->base.parent_name = g_strdup(dev_name);

            plot_channels_add_channel(PLOT_CHN(pic));
        }

        g_array_free(channels, FALSE);

    }

    connect(ui->tree_channel_list,&QTreeWidget::itemClicked,this,&Plot::onItemClicked);

math_channels:
#ifdef linux
    plot_channels_add_device(MATH_CHANNELS_DEVICE);
    priv->nb_input_devices++;
#endif

    treeview_expand_update();

}

#pragma endregion }

#pragma region Side Panel {

void Plot::on_cmb_plot_type_currentIndexChanged(int index)
{
    count_changed_cb();
    ShowControls(index);
    device_list_treeview_init();
}

void Plot::on_cmb_graph_type_currentIndexChanged(int index)
{

}

void Plot::on_txtSelectedFreq_valueChanged(double value)
{
    // Mark the entered frequency on the spectrum diagram so the display
    // updates with the frequency selection.
    if (!selectedFreqLine) {
        selectedFreqLine = new QCPItemLine(ui->fftChart);
        selectedFreqLine->setPen(QPen(QColor(255, 255, 0), 1, Qt::DashLine));
    }
    selectedFreqLine->start->setCoords(value, ui->fftChart->yAxis->range().lower);
    selectedFreqLine->end->setCoords(value, ui->fftChart->yAxis->range().upper);
    selectedFreqLine->setVisible(true);
    ui->fftChart->replot();
}

void Plot::ShowControls(int index){

    switch(index){

    case 0:
        ui->sample_count_widget->setVisible (true);
        ui->cmb_sample_count_units->setVisible (true);
        ui->cmb_sample_count_units->setEnabled (true);
        ui->lblGraphType->setVisible (true);
        ui->cmb_graph_type->setVisible (true);

        ui->cmb_fft_size->setVisible (false);
        ui->cmb_fft_win->setVisible (false);
        ui->txt_fft_avg->setVisible (false);
        ui->lbl_fft_size->setVisible (false);
        ui->lbl_fft_win->setVisible (false);
        ui->lbl_fft_avg->setVisible (false);

        ui->lbl_pwr_offset->setVisible (false);
        ui->txt_pwr_offset->setVisible (false);

        ui->fftSettings->setVisible(false);
        ui->fftChart->setVisible(false);
        ui->fftTable->setVisible(false);

        ui->frmStartFreq->setVisible(false);
        ui->frmStopFreq->setVisible(false);
        ui->frmFreqStep->setVisible(false);
        ui->frmFreqDelay->setVisible(false);
        ui->frmBandwidth->setVisible(false);

        break;
    case 1:
        ui->sample_count_widget->setVisible (false);
        ui->cmb_sample_count_units->setVisible (false);
        ui->cmb_sample_count_units->setEnabled (false);
        ui->lblGraphType->setVisible (false);
        ui->cmb_graph_type->setVisible (false);
        ui->cmb_fft_size->setVisible (true);
        ui->cmb_fft_win->setVisible (true);
        ui->txt_fft_avg->setVisible (true);
        ui->lbl_fft_size->setVisible (true);
        ui->lbl_fft_win->setVisible  (true);
        ui->lbl_fft_avg->setVisible  (true);
        ui->lbl_pwr_offset->setVisible (true);
        ui->txt_pwr_offset->setVisible (true);
        ui->fftSettings->setVisible(true);
        ui->fftChart->setVisible(true);
        //saeid raziani
        //        ui->fftTable->setVisible(true);
        ui->fftTable->setVisible(false);
        // end
        ui->frmStartFreq->setVisible(false);
        ui->frmStopFreq->setVisible(false);
        ui->frmFreqStep->setVisible(false);
        ui->frmFreqDelay->setVisible(false);
        ui->frmBandwidth->setVisible(false);
        break;

    case 2:
        ui->sample_count_widget->setVisible (false);
        ui->cmb_sample_count_units->setVisible (false);
        ui->cmb_sample_count_units->setEnabled (false);
        ui->lblGraphType->setVisible (false);
        ui->cmb_graph_type->setVisible (false);
        ui->cmb_fft_size->setVisible (true);
        ui->cmb_fft_win->setVisible (true);
        ui->txt_fft_avg->setVisible (true);
        ui->lbl_fft_size->setVisible (true);
        ui->lbl_fft_win->setVisible  (true);
        ui->lbl_fft_avg->setVisible  (true);
        ui->lbl_pwr_offset->setVisible (true);
        ui->txt_pwr_offset->setVisible (true);
        ui->fftSettings->setVisible(true);
        ui->fftChart->setVisible(true);
        //saeid raziani
        //        ui->fftTable->setVisible(true);
        ui->fftTable->setVisible(false);
        // end        ui->frmStartFreq->setVisible(true);
        ui->frmStopFreq->setVisible( true);
        ui->frmFreqStep->setVisible( true);
        ui->frmFreqDelay->setVisible(true);
        ui->frmBandwidth->setVisible(true);
        break;


    case 3:
        ui->sample_count_widget->setVisible (true);
        ui->cmb_sample_count_units->setVisible (true);
        ui->cmb_sample_count_units->setEnabled (true);
        ui->lblGraphType->setVisible (true);
        ui->cmb_graph_type->setVisible (true);
        ui->cmb_fft_size->setVisible (false);
        ui->cmb_fft_win->setVisible (false);
        ui->txt_fft_avg->setVisible (false);
        ui->lbl_fft_size->setVisible(false);
        ui->lbl_fft_win->setVisible (false);
        ui->lbl_fft_avg->setVisible (false);
        ui->lbl_pwr_offset->setVisible (false);
        ui->txt_pwr_offset->setVisible (false);
        ui->frmStartFreq->setVisible(false);
        ui->frmStopFreq->setVisible(false);
        ui->frmFreqStep->setVisible(false);
        ui->frmFreqDelay->setVisible(false);
        ui->frmBandwidth->setVisible(false);
        break;

    case 4:
        ui->sample_count_widget->setVisible (true);
        ui->cmb_sample_count_units->setVisible (true);
        ui->cmb_sample_count_units->setEnabled (true);
        ui->lblGraphType->setVisible (true);
        ui->cmb_graph_type->setVisible (true);
        ui->cmb_fft_size->setVisible (false);
        ui->cmb_fft_win->setVisible (false);
        ui->txt_fft_avg->setVisible (true);
        ui->lbl_fft_size->setVisible (false);
        ui->lbl_fft_win->setVisible (false);
        ui->lbl_fft_avg->setVisible (true);
        ui->lbl_pwr_offset->setVisible (false);
        ui->txt_pwr_offset->setVisible (false);
        ui->frmStartFreq->setVisible(false);
        ui->frmStopFreq->setVisible(false);
        ui->frmFreqStep->setVisible(false);
        ui->frmFreqDelay->setVisible(false);
        ui->frmBandwidth->setVisible(false);
        break;
    }

}

#pragma endregion }

#pragma region Time Domain Top Panel {

void Plot::on_btn_capture_clicked()
{
    //    qInfo()<<"dddddddddddddddddddddd";



    //    ui->fftChart->removeGraph(0);
    //    ui->fftChart->clearGraphs();
    //    ui->fftChart->removeGraph(1);


    timerRemoveUnupdatedFrq->stop();
    timerRemoveUnupdatedSeek->stop();

    //    if(Mode == SMART_TIME_DOMAIN)
    //    {
    //        int i{};
    //        std::cout<<i;
    //    }

    if (!check_valid_setup())
    {
        return;
    }



    if (!button_state)
    {

        timerRemoveUnupdatedFrq->start();
        timerRemoveUnupdatedSeek->stop();
        //sleep(1);
        ui->btn_capture->setStyleSheet("background-color:#186a3b;color:white");
        ui->btnfftcapture->setStyleSheet("background-color:#186a3b;color:white");
        isCaptureOn = true;

        if(ui->cmb_plot_type->currentIndex()==2)
        {
            timerRemoveUnupdatedSeek->start();
            timerRemoveUnupdatedFrq->stop();

            iio_device *dev= iio_context_find_device(ctx, PHY_DEVICE);

            iio_channel *ch1 = iio_device_find_channel(dev, "altvoltage0", true);

            ChangeFrequencyHopping(ch1,ui->txt_start_freq->value());

            //startCapture();

            hoppingFuture=QtConcurrent::run([=]{

                //hopping=true;

                double max=ui->txt_stop_freq->value();
                seekStartFreq=ui->txt_start_freq->value();
                //double next=seekStartFreq;
                seekStep=ui->txt_freq_step->value();

                sections=(max-seekStartFreq)/seekStep+1;

                //currentSections=-1;

                int delay=(int)ui->txt_freq_delay->value();

                emit HoppingSignal(true,seekStartFreq,max,seekStep,delay);

            });

            if(isExciterOn) startCapture();
        }

        else
        {
            if(isExciterOn) startCapture();
        }


    }
    else
    {
        isCaptureOn = false;
        if(ui->cmb_plot_type->currentIndex()==2)
        {
            emit HoppingSignal(false);
        }

        //hopping=false;
        //            seekStart=false;
        //            timerSeek->stop();
        //}
        stopCapture();

        if(Mode == FRQ_DOMAIN)
        {
            for (int i = 0; i < ui->fftChart->graphCount(); ++i)
            {
                ui->fftChart->graph(i)->data()->clear();
            }
            //        ui->timeChart->clearGraphs();
            //              ui->fftChart->clearGraphs();
            //        ui->fftChart->clearPlottables();
        }
    }

    if(Mode == TIME_DOMAIN)
    {
        ui->btn_fill_table_data->clicked();

    }

}

void Plot::exciterModeOn()
{
    isCaptureOn = true;

    //    button_state = false;

    //    if (!check_valid_setup()) return;

    iio_device *dev= iio_context_find_device(ctx, PHY_DEVICE);

    iio_channel *ch1 = iio_device_find_channel(dev, "altvoltage0", true);

    ChangeFrequencyHopping(ch1,ui->txt_start_freq->value());

    //startCapture();

    hoppingFuture=QtConcurrent::run([=]{

        //hopping=true;

        double max=ui->txt_stop_freq->value();
        seekStartFreq=ui->txt_start_freq->value();
        //double next=seekStartFreq;
        seekStep=ui->txt_freq_step->value();

        sections=(max-seekStartFreq)/seekStep+1;

        //currentSections=-1;

        int delay=(int)ui->txt_freq_delay->value();

        emit HoppingSignal(true,seekStartFreq,max,seekStep,delay);

    });
}

void Plot::on_btn_select_clicked()
{
    rangeYTimeChart.lower=ui->txt_y_axis_min->value();
    rangeYTimeChart.upper=ui->txt_y_axis_max->value();
    selectTimeChart=false;
}

void Plot::on_btn_new_plot_clicked()
{
    //    Plot *plot=new Plot(mApp,"newPlot");
    //    plot->osc_plot_new();
    //    plot->show();

    OSCMain osc;
    osc.new_plot_cb();
}

void Plot::on_btn_zoom_in_clicked()
{
    if(rangeXTimeChart.lower==rangeXTimeChart.upper)
    {
        rangeYTimeChart.lower=ui->txt_y_axis_min->value();
        rangeYTimeChart.upper=ui->txt_y_axis_max->value();
    }
    rangeXTimeChart.lower+=5;
    rangeXTimeChart.upper+=5;
    selectTimeChart=true;
}

void Plot::on_btn_zoom_out_clicked()
{
    if(rangeXTimeChart.lower==rangeXTimeChart.upper)
    {
        rangeYTimeChart.lower=ui->txt_y_axis_min->value();
        rangeYTimeChart.upper=ui->txt_y_axis_max->value();
    }
    rangeXTimeChart.lower-=5;
    rangeXTimeChart.upper-=5;
    selectTimeChart=true;
}

void Plot::on_chk_auto_scale_stateChanged(int arg1)
{

    if(ui->chk_auto_scale->isChecked())
    {
        ui->txt_y_axis_min->setVisible(false);
        ui->txt_y_axis_max->setVisible(false);
        ui->lbl_y_max->setVisible(false);
        ui->lbl_y_min->setVisible(false);
    }
    else
    {
        ui->txt_y_axis_min->setVisible(true);
        ui->txt_y_axis_max->setVisible(true);
        ui->lbl_y_max->setVisible(true);
        ui->lbl_y_min->setVisible(true);
    }
}

//void Plot::on_btn_fullscreen_clicked()
//{
//    if(this->isFullScreen())
//        this->showNormal();
//    else
//        this->showFullScreen();
//}

#pragma endregion }

#pragma region Frequency Domain Top Panel  {

void Plot::on_btn_select_fft_clicked()
{

    if(isBandRejectOn) return;

    selectingNoiseIsActive = false;
    selectedSmartNoiseTimer.stop();
    selectedSmartNoiseStatusSignal(false);

    rangeYFFTChart.lower=ui->txt_y_axis_min_fft->value();
    rangeYFFTChart.upper=ui->txt_y_axis_max_fft->value();

    selectFFTChart=false;

    if(!button_state || freqChanged)
    {
        ui->fftChart->graph(0)->rescaleAxes();
        reScale_plot(2);
        ui->fftChart->replot();
    }
}

void Plot::on_btn_zoom_in_fft_clicked()
{
    //    if(rangeXFFTChart.lower==rangeXFFTChart.upper)
    //    {
    //        rangeYFFTChart.lower=ui->txt_y_axis_min_fft->value();
    //        rangeYFFTChart.upper=ui->txt_y_axis_max_fft->value();
    //    }
    //    rangeXFFTChart.lower+=5;
    //    rangeXFFTChart.upper+=5;
    //    selectFFTChart=true;
}

void Plot::on_btn_zoom_out_fft_clicked()
{
    if(rangeXFFTChart.lower==rangeXFFTChart.upper)
    {
        rangeYFFTChart.lower=ui->txt_y_axis_min_fft->value();
        rangeYFFTChart.upper=ui->txt_y_axis_max_fft->value();
    }
    rangeXFFTChart.lower-=5;
    rangeXFFTChart.upper-=5;
    selectFFTChart=true;
}

void Plot::on_chk_auto_scale_fft_stateChanged(int arg1)
{

    if(ui->chk_auto_scale_fft->isChecked())
    {
        ui->txt_y_axis_min_fft->setVisible(false);
        ui->txt_y_axis_max_fft->setVisible(false);
        ui->lbl_y_max_fft->setVisible(false);
        ui->lbl_y_min_fft->setVisible(false);
    }
    else
    {
        ui->txt_y_axis_min_fft->setVisible(true);
        ui->txt_y_axis_max_fft->setVisible(true);
        ui->lbl_y_max_fft->setVisible(true);
        ui->lbl_y_min_fft->setVisible(true);
    }
}

#pragma endregion }

void Plot::on_btn_capture_triggered(QAction *arg1)
{

}

void Plot::on_btnfftcapture_clicked()
{
    on_btn_capture_clicked();
}
//saeid raziani
//update frq value of frq btn
void Plot::frqValueChangedSlot(QString val)
{
    QString t = "single frq\n" + QString("f=") + val;
    ui->btn_plot_frq->setText(t);
    double p = abs(DC_6_UPTO_8_12-(val.toDouble()));
    ui->txtSelectedFreq->setValue(p);
}



void Plot::on_btn_plot_frq_clicked()
{

}

void Plot::on_btnClear_clicked()
{
    ui->timeDataTable->clearContents();
    ui->timeDataTable->setRowCount(0);
    duplicate = false;
}


void Plot::on_btnHidden_clicked()
{

}

void Plot::on_btnHidden_clicked(bool checked)
{
    ui->wdgWaterFall->setVisible(!ui->wdgWaterFall->isVisible());
}


void Plot::on_timeDataTable_cellClicked(int row, int column)
{
    for(int i{}; i < ui->timeDataTable->rowCount(); i++)
    {
        for(int j{}; j < ui->timeDataTable->columnCount(); j++)
        {
            //            ui->timeDataTable->item(i,j)->setBackground(QBrush());
        }
    }

    //    if(row == selectedRow)
    //    {
    //        selectedRow = -2;
    //        return;
    //    }

    //    for(int i{}; i < ui->timeDataTable->columnCount(); i++)
    //    {
    //        ui->timeDataTable->item(row,i)->setBackgroundColor(Qt::red);
    //        selectedRow = row;
    //    }

    //    if (column != 12) return;
}

void Plot::on_timeDataTable_itemClicked(QTableWidgetItem *item)
{
    // item->setBackground(Qt::green);
}



void Plot::on_btnHideShow_clicked(bool checked)
{
    //    qInfo() << Mode;
    ui->btnHideShow->setText(isSettingShow==false ?"->" :"<-");
    emit settingHideShowSignal(isSettingShow);
    isSettingShow = ! isSettingShow;

}

void Plot::saveToCSVInThread(QTableWidget *tableWidget, const QString &fileName) {
    QThread* thread = new QThread;

    QObject::connect(thread, &QThread::started, [=]() {


        QFile file(fileName);
        bool isExist = file.exists();

        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream stream(&file);

            if(!isExist)
            {
                for (int col = 0; col < tableWidget->columnCount(); ++col)
                {
                    stream << tableWidget->horizontalHeaderItem(col)->text();
                    //                    if (col < tableWidget->columnCount() - 1)
                    stream << ",";
                }

                stream << "time";
                stream << "\n";
            }


            for (int row = 0; row < tableWidget->rowCount(); ++row)
            {
                for (int col = 0; col < tableWidget->columnCount(); ++col)
                {
                    QTableWidgetItem *item = tableWidget->item(row, col);
                    if (item)
                    {
                        stream << item->text();
                    }
                    //                    if (col < tableWidget->columnCount() - 1)
                    stream << ",";
                }
                stream << QDateTime::currentDateTime().toString("yyyy-MM-dd");
                stream << "\n";
            }
            file.close();
        }

        thread->quit();
        ui->btnSaveTableData->setText("Save");
    });

    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
void Plot::on_btnSaveTableData_clicked()
{
    //    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv)");

    //    if (!fileName.isEmpty()) {
    ui->btnSaveTableData->setText("saving...");
    if(userName.isEmpty()) userName = "Unknown";
    saveToCSVInThread(ui->timeDataTable, "/home/seraj3/Log/"+userName+"_LOG.csv");
    //    }

}

void Plot::on_chk_maxHold_clicked(bool checked)
{

    QCPGraph * t = ui->fftChart->graph(1);

    if(!t)
    {
        ui->fftChart->addGraph();
        ui->fftChart->graph(1)->setPen(QPen(Qt::red));
    }


    ui->fftChart->graph(1)->setVisible(checked);

    if(!checked)
    {
        maxHold_X.clear();
        maxHold_Y.clear();
    }
}
