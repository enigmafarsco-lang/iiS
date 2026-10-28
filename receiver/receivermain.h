/**
 * @file receivermain.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class ReceiverMain
 * @brief High-level controller class for ReceiverMain.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ReceiverMain;
}

/**
 * @class ReceiverMain
 * @brief High-level controller class for ReceiverMain.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ReceiverMain : public QWidget
{
    Q_OBJECT

    int cnt{0};
    /**
     * @brief Executes the real time plot procedure for ReceiverMain.
     * @param customPlot Input parameter specifying customPlot (QCustomPlot).
     */
    void realTimePlot(QCustomPlot *customPlot);
    QCustomPlot * plt;

    /**
     * @brief Initializes and configures the setup realtime data demo for the ReceiverMain subsystem.
     * @param customPlot Input parameter specifying customPlot (QCustomPlot).
     */
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

    QGLchart *freqPlot;

    /**
     * @brief Executes the creating time plot procedure for ReceiverMain.
     * @param  Input parameter specifying  (QCustomPlot).
     */
    void creatingTimePlot     (QCustomPlot *);
    /**
     * @brief Executes the creating histogram plot procedure for ReceiverMain.
     * @param  Input parameter specifying  (QCustomPlot).
     * @param  Input parameter specifying  (QCPAxisRect).
     * @param QCPBars* Input parameter specifying QCPBars* (QCPBars*).
     * @param  Input parameter specifying  (QCPGraph).
     */
    void creatingHistogramPlot(QCustomPlot * , QCPAxisRect *, QCPBars*,  QCPGraph *);
    /**
     * @brief Initializes and configures the init g l plot for the ReceiverMain subsystem.
     * @param glChart Input parameter specifying glChart (QGLchart).
     * @param glChartType Input parameter specifying glChartType (ChartType).
     */
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
    /**
     * @brief Executes the select channel with frq procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @return Output value of type int.
     */
    int selectChannelWithFrq(double);
    QString whichNoiseModeIsActive{};
    double holdFrq{};
    int selectedRow{-2};
    bool frqChanged{false};
    double frqHold{};


    /**
     * @brief Executes the smart time domain noise procedure for ReceiverMain.
     */
    void smartTimeDomainNoise();
    /**
     * @brief Sets the auto amp value parameter and updates internal state.
     * @param ampVal Input parameter specifying ampVal (double).
     */
    void setAutoAmpValue(double ampVal);
    /**
     * @brief Executes the smart table noise procedure for ReceiverMain.
     */
    void smartTableNoise();
    /**
     * @brief Executes the smart selected noise procedure for ReceiverMain.
     */
    void smartSelectedNoise();
    /**
     * @brief Executes the show noise status procedure for ReceiverMain.
     * @param activeMode Input parameter specifying activeMode (QString).
     * @param msg Input parameter specifying msg (QString).
     */
    void showNoiseStatus(QString activeMode, QString msg);
    /**
     * @brief Executes the message for auth procedure for ReceiverMain.
     * @param msg Input parameter specifying msg (QString).
     */
    void messageForAuth(QString msg);
    /**
     * @brief Sets the prv frq parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setPrvFrq(double);
    /**
     * @brief Executes the hash password procedure for ReceiverMain.
     * @param pass Input parameter specifying pass (const QString).
     * @return Output value of type QString.
     */
    QString hashPassword(const QString &pass);
    /**
     * @brief Executes the verify user procedure for ReceiverMain.
     * @param QString Input parameter specifying QString (const).
     * @param  Input parameter specifying  (const QString).
     * @param  Input parameter specifying  (const QString).
     * @return Output value of type bool.
     */
    bool verifyUser(const QString, const QString &, const QString &);
    /**
     * @brief Executes the input is not empty procedure for ReceiverMain.
     * @param inputVal Input parameter specifying inputVal (QString).
     * @return Output value of type bool.
     */
    bool inputIsNotEmpty(QString inputVal);
    /**
     * @brief Executes the length checker procedure for ReceiverMain.
     * @param inputVal Input parameter specifying inputVal (QString).
     * @return Output value of type bool.
     */
    bool lengthChecker(QString inputVal);
    /**
     * @brief Executes the user input checker procedure for ReceiverMain.
     * @param val1 Input parameter specifying val1 (QString).
     * @param val2 Input parameter specifying val2 (QString).
     * @return Output value of type bool.
     */
    bool userInputChecker(QString val1, QString val2);
    /**
     * @brief Transmits write to file command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     * @param QString Input parameter specifying QString (QString).
     * @param QString Input parameter specifying QString (QString).
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type bool.
     */
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


    /**
     * @brief Executes the hidden ui elements procedure for ReceiverMain.
     * @param bool Input parameter specifying bool (bool).
     */
    void hiddenUiElements(bool);
    /**
     * @brief Executes the save calibr to file procedure for ReceiverMain.
     */
    void saveCalibrToFile();
    QString const calibFile{dataFilePath(QStringLiteral("Calibration.txt"))};
    /**
     * @brief Executes the check frq in colibr procedure for ReceiverMain.
     */
    void checkFrqInColibr();
    /**
     * @brief Executes the calib managment procedure for ReceiverMain.
     */
    void calibManagment();
    /**
     * @brief Executes the calib initial procedure for ReceiverMain.
     */
    void calibInitial();
    /**
     * @brief Initializes and configures the setup drfm control tab for the ReceiverMain subsystem.
     */
    void setupDrfmControlTab();
    /**
     * @brief Initializes and configures the setup bit control tab for the ReceiverMain subsystem.
     */
    void setupBitControlTab();
    /**
     * @brief Executes the ensure zero dac buffer file procedure for ReceiverMain.
     * @return Output value of type QString.
     */
    QString ensureZeroDacBufferFile();
    /**
     * @brief Executes the ensure drfm dac buffer running procedure for ReceiverMain.
     */
    void ensureDrfmDacBufferRunning();
public:
    /**
     * @brief Constructs a new ReceiverMain instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit ReceiverMain(QWidget *parent = nullptr);
    /**
     * @brief Initializes and configures the init for the ReceiverMain subsystem.
     */
    void init();
    // Called by MainWindow to place the exciter widget in the receiver tabs.
    void addExciterTab(QWidget *exciter);
    QString timeDomain{""};
    QString frqDomain{};
    bool isConnect{false};
    double minFrqLimit{};
    double maxFrqLimit{};
    int cntOutCalib{};

    /**
     * @brief Destructor for ReceiverMain, releasing allocated hardware resources, timers, and buffers.
     */
    ~ReceiverMain();

signals:
    /**
     * @brief Retrieves the current onnect signal value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isConnectSignal(bool);
    /**
     * @brief Executes the in channel signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void inChannelSignal(double);



private slots:
    /**
     * @brief Slot handler responding to on btn set clicked event.
     */
    void on_btnSet_clicked();

    /**
     * @brief Slot handler responding to on btn set band select clicked event.
     */
    void on_btnSetBandSelect_clicked();
    /**
     * @brief Executes the realtime data slot procedure for ReceiverMain.
     */
    void realtimeDataSlot();

    // Phase 5: Profile tab "Set" button (auto-connected by name).
    void on_btnProfileSet_clicked();
    void on_rdoProfile100_toggled(bool checked);
    void on_rdoProfile200_toggled(bool checked);
    void on_rdoProfile400_toggled(bool checked);
    void on_btnSave_clicked();
    /**
     * @brief Slot handler responding to on btn set default clicked event.
     */
    void on_btnSetDefault_clicked();
    /**
     * @brief Slot handler responding to on btn reset clicked event.
     */
    void on_btnReset_clicked();
    /**
     * @brief Slot handler responding to on btn default iio osc clicked event.
     */
    void on_btnDefaultIioOsc_clicked();
    /**
     * @brief Sets the tings ini path parameter and updates internal state.
     * @param fileName Input parameter specifying fileName (const QString).
     * @return Output value of type QString.
     */
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
    /**
     * @brief Executes the frq mode is selected procedure for ReceiverMain.
     */
    void frqModeIsSelected();
    /**
     * @brief Executes the default parameters procedure for ReceiverMain.
     */
    void defaultParameters();
    /**
     * @brief Updates and synchronizes the update serial number data stream and visual indicators.
     */
    void updateSerialNumber();
    /**
     * @brief Executes the board serial number procedure for ReceiverMain.
     * @return Output value of type QString.
     */
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

    void settingMinMaxBand();
    void frqAndSeekStopCapturing(bool);
    void reboutWithSSN();
    void initPlot();
    /**
     * @brief Executes the re scale plot procedure for ReceiverMain.
     * @param type Input parameter specifying type (int).
     * @param  Input parameter specifying  (QCustomPlot).
     * @param  Input parameter specifying  (QCPRange).
     * @param  Input parameter specifying  (QCPRange).
     */
    void reScale_plot(int type, QCustomPlot *,QCPRange *,QCPRange *);
    /**
     * @brief Executes the plotting values procedure for ReceiverMain.
     * @param  Input parameter specifying  (const double).
     * @param  Input parameter specifying  (QCustomPlot).
     * @param  Input parameter specifying  (QVector<double>).
     * @param  Input parameter specifying  (QVector<double>).
     * @param min Input parameter specifying min (double).
     * @param max Input parameter specifying max (double).
     * @param int Input parameter specifying int (int).
     * @return Output value of type int.
     */
    int plottingValues( const double *, QCustomPlot * , QVector<double> *, QVector<double> *, double min, double max,int);
    /**
     * @brief Executes the fill historam array procedure for ReceiverMain.
     * @param int Input parameter specifying int (int).
     * @param  Input parameter specifying  (QCustomPlot).
     * @param  Input parameter specifying  (double).
     * @param int Input parameter specifying int (int).
     */
    void fillHistoramArray(int, QCustomPlot *, double *, int );
    /**
     * @brief Executes the plot histogram procedure for ReceiverMain.
     * @param  Input parameter specifying  (QCustomPlot).
     * @param std::unordered_map<double Input parameter specifying std::unordered_map<double (std::unordered_map<double).
     * @param  Input parameter specifying  (double >).
     */
    void plotHistogram(QCustomPlot *,std::unordered_map<double, double > *);
    /**
     * @brief Executes the plot init procedure for ReceiverMain.
     */
    void plotInit();
    /**
     * @brief Executes the default settings procedure for ReceiverMain.
     */
    void defaultSettings();
    // Phase 6: one-shot ADRV9009 calibration after software start (all
    // calibration items except lol-ext and fhm), then never again.
    void smartNoise();

    double centerFrq{};
    double maximumFinder(double *, double *);
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
    void smartInit();
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

    /**
     * @brief Transmits send frq to calib device command/packet over the communication bus.
     */
    void sendFrqToCalibDevice();
    /**
     * @brief Transmits send power to calib device command/packet over the communication bus.
     */
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
    /**
     * @brief Slot handler responding to on btn set t h clicked event.
     */
    void on_btnSetTH_clicked();
    /**
     * @brief Slot handler responding to on btn set t h c w clicked event.
     */
    void on_btnSetTHCW_clicked();
    /**
     * @brief Slot handler responding to on tab widget current changed event.
     * @param index Input parameter specifying index (int).
     */
    void on_tabWidget_currentChanged(int index);
    /**
     * @brief Slot handler responding to on btn frq seek clicked event.
     */
    void on_btnFrqSeek_clicked();
    /**
     * @brief Slot handler responding to on label 43 link activated event.
     * @param link Input parameter specifying link (const QString).
     */
    void on_label_43_linkActivated(const QString &link);
    /**
     * @brief Slot handler responding to on cmb transmit activated event.
     * @param index Input parameter specifying index (int).
     */
    void on_cmbTransmit_activated(int index);

    /**
     * @brief Slot handler responding to on chb en  p a t t e r n clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chbEn_PATTERN_clicked(bool checked);

    //    void on_tabWidgetSetting_tabBarClicked(int index);

    void on_btnReboot_clicked(bool checked);

    void on_tableWidget_2_cellClicked(int row, int column);


    /**
     * @brief Slot handler responding to on push button clicked event.
     */
    void on_pushButton_clicked();

    /**
     * @brief Slot handler responding to on tbl c w cell clicked event.
     * @param row Input parameter specifying row (int).
     * @param column Input parameter specifying column (int).
     */
    void on_tblCW_cellClicked(int row, int column);


    /**
     * @brief Slot handler responding to on btn new user clicked event.
     */
    void on_btnNewUser_clicked();

    /**
     * @brief Slot handler responding to on tab widget tab bar clicked event.
     * @param index Input parameter specifying index (int).
     */
    void on_tabWidget_tabBarClicked(int index);

    /**
     * @brief Slot handler responding to on btn login clicked event.
     */
    void on_btnLogin_clicked();

    /**
     * @brief Slot handler responding to on btn logout clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnLogout_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn recovery clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnRecovery_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn new admin clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnNewAdmin_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn save limit clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnSaveLimit_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn show user clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnShowUser_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn calib clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnCalib_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn calib rff clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnCalibRff_clicked(bool checked);

//    void on_btnLimitation_clicked();
    void checkingCalibResponseInSpectromSlot(double, double);


    void on_btnStopCalib_clicked();

//    void on_chbCalib_stateChanged(int arg1);

public slots:
    void connection();
    void ValueSlot (Pulse);
    void frqValueChenged(QString);
    /**
     * @brief Transmits send spot file to cart command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendSpotFileToCart(QString);
    /**
     * @brief Retrieves the current plot data from thread value.
     */
    void getPlotDataFromThread();
    /**
     * @brief Executes the histo main plot slot procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void histoMainPlotSlot(double, double);
    /**
     * @brief Executes the stop smart noise slot procedure for ReceiverMain.
     */
    void stopSmartNoiseSlot();
    /**
     * @brief Executes the stop exciter slot procedure for ReceiverMain.
     */
    void stopExciterSlot();
    /**
     * @brief Executes the calculating cw table slot procedure for ReceiverMain.
     * @param QVector<double> Input parameter specifying QVector<double> (QVector<double>).
     * @param QVector<double> Input parameter specifying QVector<double> (QVector<double>).
     * @param QVector<QString> Input parameter specifying QVector<QString> (QVector<QString>).
     * @param int> Input parameter specifying int> (QVector<long long).
     */
    void calculatingCwTableSlot(QVector<double>,QVector<double>,QVector<QString>,QVector<long long int>);
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray);


    //    void sendLimitFrqSlot();
    //    void histoBarPlotSlot (double, double);
    //    void test(modesConst );


signals:
    void createPlotSignal1(QCustomSeries);
    void seekingStatusSignal(QString);
    /**
     * @brief Executes the frq domain is on procedure for ReceiverMain.
     */
    void frqDomainIsOn();
    /**
     * @brief Executes the start exciter signal procedure for ReceiverMain.
     */
    void startExciterSignal();
    /**
     * @brief Executes the frq plt signal procedure for ReceiverMain.
     */
    void frqPltSignal();

    // Phase 5: the Profile tab picked an ADRV9009 TX profile (100/200/400 MHz).
    // Routed to the exciter so spot noise files match the active profile.
    void profileBandwidthChanged(int bwMHz);

    // Phase 6: TX calibration (Calibrated mismatch power, dB) of the
    // Calibration tab changed - refresh every calibrated TX power display.
    void txCalibChangedSignal(double);


    void frqValues      (QCustomSeries a);
    void frqValueSignal      (double,double);
    /**
     * @brief Executes the p a peak value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void PAPeakValueSignal   (double,double);
    /**
     * @brief Executes the p a sum max value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void PASumMaxValueSignal (double,double);
    /**
     * @brief Executes the pri value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void priValueSignal      (double,double);
    /**
     * @brief Executes the pw one value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void pwOneValueSignal    (double,double);
    /**
     * @brief Executes the pw two value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void pwTwoValueSignal    (double,double);
    /**
     * @brief Executes the toa value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void toaValueSignal      (double,double);
    /**
     * @brief Executes the channel value signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void ChannelValueSignal  (double,double);
    /**
     * @brief Executes the smart noise is active signal procedure for ReceiverMain.
     */
    void smartNoiseIsActiveSignal  ();



    /**
     * @brief Executes the hi procedure for ReceiverMain.
     */
    void hi();
    /**
     * @brief Executes the cw frq signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void cwFrqSignal        (double);
    /**
     * @brief Executes the cw power signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void cwPowerSignal      (double);
    /**
     * @brief Executes the spot frq signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void spotFrqSignal      (double);
    /**
     * @brief Executes the spot power signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void spotPowerSignal    (double);
    /**
     * @brief Executes the sweep start frq signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void sweepStartFrqSignal(double);
    /**
     * @brief Executes the sweep stop frq signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void sweepStopFrqSignal (double);
    /**
     * @brief Executes the sweep step signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void sweepStepSignal    (double);
    /**
     * @brief Executes the sweep power signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void sweepPowerSignal   (double);
    /**
     * @brief Executes the sweep delay signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void sweepDelaySignal   (double);
    /**
     * @brief Executes the imp frq signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void impFrqSignal       (double);
    /**
     * @brief Executes the imp power signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void impPowerSignal     (double);
    /**
     * @brief Executes the imp pw signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void impPwSignal        (double);
    /**
     * @brief Executes the imp pri signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void impPriSignal       (double);
    /**
     * @brief Executes the wb power signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void wbPowerSignal      (double);


    /**
     * @brief Executes the cw on btn signal procedure for ReceiverMain.
     */
    void cwOnBtnSignal     ();
    /**
     * @brief Executes the cw off btn signal procedure for ReceiverMain.
     */
    void cwOffBtnSignal    ();
    /**
     * @brief Executes the spot on btn signal procedure for ReceiverMain.
     */
    void spotOnBtnSignal   ();
    /**
     * @brief Executes the spot off btn signal procedure for ReceiverMain.
     */
    void spotOffBtnSignal  ();
    /**
     * @brief Executes the sweep on btn signal procedure for ReceiverMain.
     */
    void sweepOnBtnSignal  ();
    /**
     * @brief Executes the sweep off btn signal procedure for ReceiverMain.
     */
    void sweepOffBtnSignal ();
    /**
     * @brief Executes the imp on btn signal procedure for ReceiverMain.
     */
    void impOnBtnSignal    ();
    /**
     * @brief Executes the imp off btn signal procedure for ReceiverMain.
     */
    void impOffBtnSignal   ();
    /**
     * @brief Executes the wb on btn signal procedure for ReceiverMain.
     */
    void wbOnBtnSignal     ();
    /**
     * @brief Executes the wb off btn signal procedure for ReceiverMain.
     */
    void wbOffBtnSignal    ();

    /**
     * @brief Executes the user logged in signal procedure for ReceiverMain.
     * @param double Input parameter specifying double (double).
     */
    void userLoggedInSignal(double);
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
    //    void cwTabletoPlotSignal(qvdouble,double,QString);

};

#endif // RECEIVERMAIN_H
