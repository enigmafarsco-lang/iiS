// ============================================================================
// File: receivermain.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for receivermain.h.
// ============================================================================

#ifndef RECEIVERMAIN_H
#define RECEIVERMAIN_H
#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <receiver/oscmain.h>
#include <receiver/datafiles.h>
#include <unordered_map>
#include "components/GLChart/QGLchart.h"
#include <QSystemTrayIcon>
#include "plotfunction.h"
#include <QRandomGenerator>
#include <QComboBox>
#include "calculateplotdata.h"
#include <QProcess>
#include <QTextStream>
//#include <QSs

namespace Ui {
class ReceiverMain;
}

class ReceiverMain : public QWidget
{
    Q_OBJECT

    int cnt{0};
    // Executes real time plot routine.
    void realTimePlot(QCustomPlot *customPlot);
    QCustomPlot * plt;

    // Initializes setup realtime data demo for ReceiverMain.
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

    QGLchart *freqPlot;

    // Executes creating time plot routine.
    void creatingTimePlot     (QCustomPlot *);
    // Executes creating histogram plot routine.
    void creatingHistogramPlot(QCustomPlot * , QCPAxisRect *, QCPBars*,  QCPGraph *);
    // Initializes init g l plot for ReceiverMain.
    void initGLPlot(QGLchart *glChart, ChartType glChartType);

    QVector2D  vec;

    QCustomSeries a;

    plotfunction * t1 ;
    plotfunction * t2 ;
    plotfunction * t3 ;
    plotfunction * t4 ;
    plotfunction * t5 ;
    plotfunction * t6 ;
    plotfunction * t7 ;
    plotfunction * t8 ;
    // Executes select channel with frq routine.
    int selectChannelWithFrq(double);
    QString whichNoiseModeIsActive{};
    double holdFrq{};
    int selectedRow{-2};
    bool frqChanged{false};
    double frqHold{};


    // Executes smart time domain noise routine.
    void smartTimeDomainNoise();
    // Sets auto amp value value and updates internal subsystem configuration.
    void setAutoAmpValue(double ampVal);
    // Executes smart table noise routine.
    void smartTableNoise();
    // Executes smart selected noise routine.
    void smartSelectedNoise();
    // Executes show noise status routine.
    void showNoiseStatus(QString activeMode, QString msg);
    // Executes message for auth routine.
    void messageForAuth(QString msg);
    // Sets prv frq value and updates internal subsystem configuration.
    void setPrvFrq(double);
    // Executes hash password routine.
    QString hashPassword(const QString &pass);
    // Executes verify user routine.
    bool verifyUser(const QString, const QString &, const QString &);
    // Executes input is not empty routine.
    bool inputIsNotEmpty(QString inputVal);
    // Executes length checker routine.
    bool lengthChecker(QString inputVal);
    // Executes user input checker routine.
    bool userInputChecker(QString val1, QString val2);
    // Transmits write to file command/packet over communication interface.
    bool writeToFile(QString,QString, QString,QString);
    QString const fileAuth{dataFilePath(QStringLiteral("upf"))};
    QString superUser{"superadmin"};
    QString passSuper{"sadefa@1402"};
    bool isUserLoggedin{false};
    QString userRole{};
    double avgFrq{};
    double avgPower{};
    int num{0};
    QTimer * cwTableTimer;
    int cntVecCalib{};


    // Executes hidden ui elements routine.
    void hiddenUiElements(bool);
    // Executes save calibr to file routine.
    void saveCalibrToFile();
    QString const calibFile{dataFilePath(QStringLiteral("Calibration.txt"))};
    // Executes check frq in colibr routine.
    void checkFrqInColibr();
    // Executes calib managment routine.
    void calibManagment();
    // Executes calib initial routine.
    void calibInitial();
    // Initializes setup drfm control tab for ReceiverMain.
    void setupDrfmControlTab();
    // Initializes setup bit control tab for ReceiverMain.
    void setupBitControlTab();
    // Executes ensure zero dac buffer file routine.
    QString ensureZeroDacBufferFile();
    // Executes ensure drfm dac buffer running routine.
    void ensureDrfmDacBufferRunning();
public:
    // Constructor: Initializes ReceiverMain subsystem components, default parameters, and state.
    explicit ReceiverMain(QWidget *parent = nullptr);
    // Initializes init for ReceiverMain.
    void init();
    // Called by MainWindow to place the exciter widget in the receiver tabs.
    void addExciterTab(QWidget *exciter);
    QString timeDomain{""};
    QString frqDomain{};
    bool isConnect{false};
    double minFrqLimit{};
    double maxFrqLimit{};
    int cntOutCalib{};

    // Destructor: Cleans up ReceiverMain resources, timers, and allocated memory.
    ~ReceiverMain();

signals:
    // Retrieves current onnect signal state.
    void isConnectSignal(bool);
    // Executes in channel signal routine.
    void inChannelSignal(double);



private slots:
    // Event/Signal handler for on btn set clicked.
    void on_btnSet_clicked();

    // Event/Signal handler for on btn set band select clicked.
    void on_btnSetBandSelect_clicked();
    // Executes realtime data slot routine.
    void realtimeDataSlot();

    // Phase 5: Profile tab "Set" button (auto-connected by name).
    void on_btnProfileSet_clicked();
    // Event/Signal handler for on rdo profile100 toggled.
    void on_rdoProfile100_toggled(bool checked);
    // Event/Signal handler for on rdo profile200 toggled.
    void on_rdoProfile200_toggled(bool checked);
    // Event/Signal handler for on rdo profile400 toggled.
    void on_rdoProfile400_toggled(bool checked);
    // Event/Signal handler for on btn save clicked.
    void on_btnSave_clicked();
    // Event/Signal handler for on btn set default clicked.
    void on_btnSetDefault_clicked();
    // Event/Signal handler for on btn reset clicked.
    void on_btnReset_clicked();
    // Event/Signal handler for on btn default iio osc clicked.
    void on_btnDefaultIioOsc_clicked();
    // Sets tings ini path value and updates internal subsystem configuration.
    QString settingsIniPath(const QString &fileName);

    //    QTimer * frqTimer;

public:
    Ui::ReceiverMain *ui;

    //for test


    QTimer mTimer;
    /////////////////////////////////////////






    // saeid raziani ==> here we define some ui pointer in order to using and showing them in receiver main.
    //--- Global --------------------------------------------------------
    QComboBox* ensmCmb;
    QDoubleSpinBox * frqSpn{};
    QComboBox * ensm_mode_available;
    //--- Hopping -------------------------------------------------------
    QCheckBox      * chk_trigger_mode;
    QCheckBox      * chk_exit_mode;
    QCheckBox      * chk_msc_sync;
    QCheckBox      * chk_enable;
    QDoubleSpinBox * txt_init_frequency;
    QSpinBox       * txt_gpio_pin;
    QSpinBox       * txt_min_freq;
    QSpinBox       * txt_max_freq;
    QPushButton    * save_settings;

    QDoubleSpinBox * nextFrqSpn;
    QDoubleSpinBox * minSpn;
    QDoubleSpinBox * maxSpn;
    QDoubleSpinBox * stepSpn;
    QDoubleSpinBox * delaySpn;
    QPushButton    * startBtn;
    QCheckBox      * modeChk;
    //--- TX1 -----------------------------------------------------------
    QCheckBox      * pinn_TX1_Chk;
    QCheckBox      * track_TX1_Chk;
    QCheckBox      * power_TX1_DownChk = nullptr;
    QCheckBox      * lo_TX1_Chk;
    QDoubleSpinBox * att_TX1_Spn = nullptr;
    QLabel         * rfBandlbl;
    QLabel         * sampleRatelbl;
    //--- TX1 -----------------------------------------------------------
    QCheckBox      * pinn_TX2_Chk;
    QCheckBox      * track_TX2_Chk;
    QCheckBox      * powerTX2DownChk;
    QCheckBox      * lo_TX2_Chk;
    QDoubleSpinBox * att_TX2_Spn;
    //--- OBSRX ---------------------------------------------------------
    QCheckBox      * power_OBSRX_Spn;
    QCheckBox      * track_OBSRX_Chk;
    QDoubleSpinBox * hardwareGain;
    QLabel         * rfBandOBSlbl;
    QLabel         * sampleRateOBSlbl;
    //--- THCW ----------------------------------------------------------
    QDoubleSpinBox * THCW1 = new QDoubleSpinBox;
    QSpinBox * THCW2 = new QSpinBox;
    QSpinBox * THCW3 = new QSpinBox;
    QPushButton * btnWrite;

    //===================================================================
    QCPRange *rangeXChart = new QCPRange[10];
    QCPRange *rangeYChart = new QCPRange[10];

    QVector<double> xFrq,     yFrq;
    QVector<double> xPW1,     yPW1;
    QVector<double> xPW2,     yPW2;
    QVector<double> xPAPEAK,  yPAPEAK;
    QVector<double> xPASUM,   yPASUM;
    QVector<double> xTOA,     yTOA;
    QVector<double> xCHANNEL, yCHANNEL;
    QVector<double> xPRI,     yPRI;

    Plot * timeDomainPlot ;
    Plot * smartTimeDomainPlot ;
    Plot * frqDomainPlot;
    // Executes frq mode is selected routine.
    void frqModeIsSelected();
    // Executes default parameters routine.
    void defaultParameters();
    // Updates and synchronizes update serial number data stream.
    void updateSerialNumber();
    // Executes board serial number routine.
    QString boardSerialNumber();
    // Apply a Talise profile to the board (the Profile tab Set button and
    // the startup assertion from the first-start IP form both use this).
    // Returns false when nothing was written (serial rule / missing file).
    bool applyProfileBw(int bw);
    // Compose the Talise profile from the section files
    // (files/filters/adrv9009/sections/general_<TX>.txt + orx_<ORx>.txt +
    // tx_<TX>.txt + "</profile>") and write it to the board.  The first-start
    // IP form provides both the TX and the ORx choice; the Profile tab Set
    // button uses the diagonal (ORx == TX).
    bool applyComposedProfile(int txBw, int orxBw);
    // Sync the software UI (Profile tab TX/ORx radios, exciter profile,
    // spectrum window) to the given profile without writing to the board.
    void syncProfileUi(int txBw, int orxBw);
    // Keep the Profile tab ORx choices consistent with the TX choice:
    // TX 100 -> ORx 100; TX 200 -> ORx 100/200; TX 400 -> ORx 200/400.
    void syncProfileOrxChoices();
    // Executes changing plot mode routine.
    void changingPlotMode(int);
    QTimer dataTimer;

    QString spotPath = "spot/spot10mhz.txt";
    bool isSending{};
    bool isSettingShow{};


    //----------
    QDoubleSpinBox * spnCWFrq;           ;
    QDoubleSpinBox * spnCWPower;         ;
    QDoubleSpinBox * spnSpotFrq          ;
    QDoubleSpinBox * spnSpotPower        ;
    QDoubleSpinBox * spnSweepStartFrq    ;
    QDoubleSpinBox * spnSweepStopFrq     ;
    QDoubleSpinBox * spnSweepStep        ;
    QDoubleSpinBox * spnSweepPower       ;
    QDoubleSpinBox * spnDelay            ;
    QDoubleSpinBox * spnImpulseFrq       ;
    QDoubleSpinBox * spnImpulsePower     ;
    QDoubleSpinBox * spnWBPower          ;
    //---------


    bool state{false};
    bool pwIsOn{};

    const QString fileAddress{resourceFilePath(QStringLiteral("filters/adrv9009/Tx_BW400_IR491p52_Rx_BW100_OR122p88_ORx_BW400_OR491p52_DC245p76.txt"))};
    // TX and ORx bandwidths the user chose in the first-start IP form;
    // asserted on the board once at first startup (the board itself boots
    // at profile 100 = TX100 + ORx100).
    int startupTxBw{200};
    int startupOrxBw{100};

    // Board serial number cache (SN001 gates the ORx 400 option in the
    // Profile tab); filled once the IIO context is up.
    QString boardSerialCache;

    //    QMap
    std::unordered_map<double, double> frqMap;
    std::unordered_map<double, double> pwMap;
    std::unordered_map<double, double> priMap;
    std::unordered_map<double, double> piSumMap;

    //=====================================================================

    // Sets ting min max band value and updates internal subsystem configuration.
    void settingMinMaxBand();
    // Executes frq and seek stop capturing routine.
    void frqAndSeekStopCapturing(bool);
    // Executes rebout with s s n routine.
    void reboutWithSSN();
    // Initializes init plot for ReceiverMain.
    void initPlot();
    // Executes re scale plot routine.
    void reScale_plot(int type, QCustomPlot *,QCPRange *,QCPRange *);
    // Executes plotting values routine.
    int plottingValues( const double *, QCustomPlot * , QVector<double> *, QVector<double> *, double min, double max,int);
    // Executes fill historam array routine.
    void fillHistoramArray(int, QCustomPlot *, double *, int );
    // Executes plot histogram routine.
    void plotHistogram(QCustomPlot *,std::unordered_map<double, double > *);
    // Executes plot init routine.
    void plotInit();
    // Executes default settings routine.
    void defaultSettings();
    // Phase 6: one-shot ADRV9009 calibration after software start (all
    // calibration items except lol-ext and fhm), then never again.
    void smartNoise();

    double centerFrq{};
    // Executes maximum finder routine.
    double maximumFinder(double *, double *);
    // Executes minimum finder routine.
    double minimumFinder(double *, double *);

    double frqMax{0};
    double PAPeakMax{0};
    double PASumMax{0};
    double PRIMax{0};
    double PRIMin{9999999999};
    double PW1Max{0};
    double PW2Max{0};
    double TOAMax{0};
    double ChannelMax{0};

    double xMax{};
    double xMin{9999999999};
    double yMax{};
    double yMin{};

    int frqCnt{};
    int PAPeakCnt{-1};
    int PASumCnt{};
    int PRICnt{};
    int PW1Cnt{};
    int PW2Cnt{-1};
    int TOACnt{-1};
    int ChannelCnt{-1};
    int clickCnt{};
    int frqValue{};

    OSCMain *oscMain = nullptr;
    int arrayOne[100];
    int arrayTwo[100];
    int arrayCount{};
    bool isExciterOn{false};
    bool receiverIsConnected{false};
    //    int frqVal{};

    int currentTabState;
    QTimer* timer;
    // Executes smart init routine.
    void smartInit();
    // Executes stop smart noise routine.
    void stopSmartNoise(QString);
    int tabPress{};
    QWidget * wigRecover;
    bool firstTime{true};
    QVector<long long int>    frqTimeVec;
    QVector<double>  frqValVec;
    QVector<double>  powerValVec;
    QVector<QString> modValVec;
    int counter{};

    bool isShow{false};
    bool isRffCalibOn{false};

    QVector<double> frqCalibVec   = QVector<double> (700,0);
    QVector<double> powerCalibVec = QVector<double> (700,0);
    QVector<double> frqCardVec    = QVector<double> (700,0);
    QVector<double> powerCardVec  = QVector<double> (700,0);

    // Transmits send frq to calib device command/packet over communication interface.
    void sendFrqToCalibDevice();
    // Transmits send power to calib device command/packet over communication interface.
    void sendPowerToCalibDevice();
    bool isCalibAllowedToSend{true};
    double newPowerCalib{};
    double newFrqCalib{};
    double calibVal{};
    int cntCalib{};
    QTimer * checkIfValuesSetOnSG;
    QTimer * calibPowerTimer;
    int holdIndexFrqCalib{};
    int holdIndexTblUser{-10};
    QString userNameInFile{};
//    int cntFrqCalib{};
//    int cntPowerCalib{};
    double frqCalibHold{};


    //calibration
    void startCalibSending();
    bool isCalibAllowed {false};

    QString modeCalib{};
    bool isFrqSetOnCalib{false};
    bool isPowerSetOnCalib{false};
    double powNw{};
    QVector<double> sgFrqInCalibFile;
    QVector<double> sgPowerInCalibFile;
    QVector<double> cardPowerInCalibFile;
    double minFrqVal{50000};


private slots:
    // Event/Signal handler for on btn set t h clicked.
    void on_btnSetTH_clicked();
    // Event/Signal handler for on btn set t h c w clicked.
    void on_btnSetTHCW_clicked();
    // Event/Signal handler for on tab widget current changed.
    void on_tabWidget_currentChanged(int index);
    // Event/Signal handler for on btn frq seek clicked.
    void on_btnFrqSeek_clicked();
    // Event/Signal handler for on label 43 link activated.
    void on_label_43_linkActivated(const QString &link);
    // Event/Signal handler for on cmb transmit activated.
    void on_cmbTransmit_activated(int index);

    // Event/Signal handler for on chb en  p a t t e r n clicked.
    void on_chbEn_PATTERN_clicked(bool checked);

    //    void on_tabWidgetSetting_tabBarClicked(int index);

    // Event/Signal handler for on btn reboot clicked.
    void on_btnReboot_clicked(bool checked);

    // Event/Signal handler for on table widget 2 cell clicked.
    void on_tableWidget_2_cellClicked(int row, int column);


    // Event/Signal handler for on push button clicked.
    void on_pushButton_clicked();

    // Event/Signal handler for on tbl c w cell clicked.
    void on_tblCW_cellClicked(int row, int column);


    // Event/Signal handler for on btn new user clicked.
    void on_btnNewUser_clicked();

    // Event/Signal handler for on tab widget tab bar clicked.
    void on_tabWidget_tabBarClicked(int index);

    // Event/Signal handler for on btn login clicked.
    void on_btnLogin_clicked();

    // Event/Signal handler for on btn logout clicked.
    void on_btnLogout_clicked(bool checked);

    // Event/Signal handler for on btn recovery clicked.
    void on_btnRecovery_clicked(bool checked);

    // Event/Signal handler for on btn new admin clicked.
    void on_btnNewAdmin_clicked(bool checked);

    // Event/Signal handler for on btn save limit clicked.
    void on_btnSaveLimit_clicked(bool checked);

    // Event/Signal handler for on btn show user clicked.
    void on_btnShowUser_clicked(bool checked);

    // Event/Signal handler for on btn calib clicked.
    void on_btnCalib_clicked(bool checked);

    // Event/Signal handler for on btn calib rff clicked.
    void on_btnCalibRff_clicked(bool checked);

//    void on_btnLimitation_clicked();
    void checkingCalibResponseInSpectromSlot(double, double);


    // Event/Signal handler for on btn stop calib clicked.
    void on_btnStopCalib_clicked();

//    void on_chbCalib_stateChanged(int arg1);

public slots:
    // Executes connection routine.
    void connection();
    // Executes value slot routine.
    void ValueSlot (Pulse);
    // Executes frq value chenged routine.
    void frqValueChenged(QString);
    // Transmits send spot file to cart command/packet over communication interface.
    void sendSpotFileToCart(QString);
    // Retrieves current plot data from thread state.
    void getPlotDataFromThread();
    // Executes histo main plot slot routine.
    void histoMainPlotSlot(double, double);
    // Executes stop smart noise slot routine.
    void stopSmartNoiseSlot();
    // Executes stop exciter slot routine.
    void stopExciterSlot();
    // Executes calculating cw table slot routine.
    void calculatingCwTableSlot(QVector<double>,QVector<double>,QVector<QString>,QVector<long long int>);
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray);


    //    void sendLimitFrqSlot();
    //    void histoBarPlotSlot (double, double);
    //    void test(modesConst );


signals:
    // Executes create plot signal1 routine.
    void createPlotSignal1(QCustomSeries);
    // Executes seeking status signal routine.
    void seekingStatusSignal(QString);
    // Executes frq domain is on routine.
    void frqDomainIsOn();
    // Executes start exciter signal routine.
    void startExciterSignal();
    // Executes frq plt signal routine.
    void frqPltSignal();

    // Phase 5: the Profile tab picked an ADRV9009 TX profile (100/200/400 MHz).
    // Routed to the exciter so spot noise files match the active profile.
    void profileBandwidthChanged(int bwMHz);

    // Phase 6: TX calibration (Calibrated mismatch power, dB) of the
    // Calibration tab changed - refresh every calibrated TX power display.
    void txCalibChangedSignal(double);


    // Executes frq values routine.
    void frqValues      (QCustomSeries a);
    // Executes frq value signal routine.
    void frqValueSignal      (double,double);
    // Executes p a peak value signal routine.
    void PAPeakValueSignal   (double,double);
    // Executes p a sum max value signal routine.
    void PASumMaxValueSignal (double,double);
    // Executes pri value signal routine.
    void priValueSignal      (double,double);
    // Executes pw one value signal routine.
    void pwOneValueSignal    (double,double);
    // Executes pw two value signal routine.
    void pwTwoValueSignal    (double,double);
    // Executes toa value signal routine.
    void toaValueSignal      (double,double);
    // Executes channel value signal routine.
    void ChannelValueSignal  (double,double);
    // Executes smart noise is active signal routine.
    void smartNoiseIsActiveSignal  ();



    // Executes hi routine.
    void hi();
    // Executes cw frq signal routine.
    void cwFrqSignal        (double);
    // Executes cw power signal routine.
    void cwPowerSignal      (double);
    // Executes spot frq signal routine.
    void spotFrqSignal      (double);
    // Executes spot power signal routine.
    void spotPowerSignal    (double);
    // Executes sweep start frq signal routine.
    void sweepStartFrqSignal(double);
    // Executes sweep stop frq signal routine.
    void sweepStopFrqSignal (double);
    // Executes sweep step signal routine.
    void sweepStepSignal    (double);
    // Executes sweep power signal routine.
    void sweepPowerSignal   (double);
    // Executes sweep delay signal routine.
    void sweepDelaySignal   (double);
    // Executes imp frq signal routine.
    void impFrqSignal       (double);
    // Executes imp power signal routine.
    void impPowerSignal     (double);
    // Executes imp pw signal routine.
    void impPwSignal        (double);
    // Executes imp pri signal routine.
    void impPriSignal       (double);
    // Executes wb power signal routine.
    void wbPowerSignal      (double);


    // Executes cw on btn signal routine.
    void cwOnBtnSignal     ();
    // Executes cw off btn signal routine.
    void cwOffBtnSignal    ();
    // Executes spot on btn signal routine.
    void spotOnBtnSignal   ();
    // Executes spot off btn signal routine.
    void spotOffBtnSignal  ();
    // Executes sweep on btn signal routine.
    void sweepOnBtnSignal  ();
    // Executes sweep off btn signal routine.
    void sweepOffBtnSignal ();
    // Executes imp on btn signal routine.
    void impOnBtnSignal    ();
    // Executes imp off btn signal routine.
    void impOffBtnSignal   ();
    // Executes wb on btn signal routine.
    void wbOnBtnSignal     ();
    // Executes wb off btn signal routine.
    void wbOffBtnSignal    ();

    // Executes user logged in signal routine.
    void userLoggedInSignal(double);
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
    //    void cwTabletoPlotSignal(qvdouble,double,QString);

};

#endif // RECEIVERMAIN_H
