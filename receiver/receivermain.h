#ifndef RECEIVERMAIN_H
#define RECEIVERMAIN_H
#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <receiver/oscmain.h>
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
    void realTimePlot(QCustomPlot *customPlot);
    QCustomPlot * plt;

    void setupRealtimeDataDemo(QCustomPlot *customPlot);

    QGLchart *freqPlot;

    void creatingTimePlot     (QCustomPlot *);
    void creatingHistogramPlot(QCustomPlot * , QCPAxisRect *, QCPBars*,  QCPGraph *);
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
    int selectChannelWithFrq(double);
    QString whichNoiseModeIsActive{};
    double holdFrq{};
    int selectedRow{-2};
    bool frqChanged{false};
    double frqHold{};


    void smartTimeDomainNoise();
    void setAutoAmpValue(double ampVal);
    void smartTableNoise();
    void smartSelectedNoise();
    void showNoiseStatus(QString activeMode, QString msg);
    void messageForAuth(QString msg);
    void setPrvFrq(double);
    QString hashPassword(const QString &pass);
    bool verifyUser(const QString, const QString &, const QString &);
    bool inputIsNotEmpty(QString inputVal);
    bool lengthChecker(QString inputVal);
    bool userInputChecker(QString val1, QString val2);
    bool writeToFile(QString,QString, QString,QString);
    QString const fileAuth{"upf"};
    QString superUser{"superadmin"};
    QString passSuper{"sadefa@1402"};
    bool isUserLoggedin{false};
    QString userRole{};
    double avgFrq{};
    double avgPower{};
    int num{0};
    QTimer * cwTableTimer;
    int cntVecCalib{};


    void hiddenUiElements(bool);
    void saveCalibrToFile();
    QString const calibFile{"Calibration.txt"};
    void checkFrqInColibr();
    void calibManagment();
    void calibInitial();
    void setupDrfmControlTab();
public:
    explicit ReceiverMain(QWidget *parent = nullptr);
    void init();
    QString timeDomain{""};
    QString frqDomain{};
    bool isConnect{false};
    double minFrqLimit{};
    double maxFrqLimit{};
    int cntOutCalib{};

    ~ReceiverMain();

signals:
    void isConnectSignal(bool);
    void inChannelSignal(double);



private slots:
    void on_btnSet_clicked();

    void on_btnSetBandSelect_clicked();
    void realtimeDataSlot();

    //    QTimer * frqTimer;

public:
    Ui::ReceiverMain *ui;

    //for test


    QTimer mTimer;
    /////////////////////////////////////////






    // saeid raziani ==> here we define some ui pointer in order to using and showing them in receiver main.
    //--- Global --------------------------------------------------------
    QComboBox* ensmCmb;
    QDoubleSpinBox * frqSpn;
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
    void frqModeIsSelected();
    void defaultParameters();
    void updateSerialNumber();
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

    const QString fileAddress{"/home/seraj3/Downloads/iio-oscilloscope/filters/adrv9009/Tx_BW400_IR491p52_Rx_BW100_OR122p88_ORx_BW400_OR491p52_DC245p76.txt"};

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
    void reScale_plot(int type, QCustomPlot *,QCPRange *,QCPRange *);
    int plottingValues( const double *, QCustomPlot * , QVector<double> *, QVector<double> *, double min, double max,int);
    void fillHistoramArray(int, QCustomPlot *, double *, int );
    void plotHistogram(QCustomPlot *,std::unordered_map<double, double > *);
    void plotInit();
    void defaultSettings();
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

    void sendFrqToCalibDevice();
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
    void on_btnSetTH_clicked();
    void on_btnSetTHCW_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_btnFrqSeek_clicked();
    void on_label_43_linkActivated(const QString &link);
    void on_cmbTransmit_activated(int index);

    void on_chbEn_PATTERN_clicked(bool checked);

    //    void on_tabWidgetSetting_tabBarClicked(int index);

    void on_btnReboot_clicked(bool checked);

    void on_tableWidget_2_cellClicked(int row, int column);


    void on_pushButton_clicked();

    void on_tblCW_cellClicked(int row, int column);


    void on_btnNewUser_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_btnLogin_clicked();

    void on_btnLogout_clicked(bool checked);

    void on_btnRecovery_clicked(bool checked);

    void on_btnNewAdmin_clicked(bool checked);

    void on_btnSaveLimit_clicked(bool checked);

    void on_btnShowUser_clicked(bool checked);

    void on_btnCalib_clicked(bool checked);

    void on_btnCalibRff_clicked(bool checked);

//    void on_btnLimitation_clicked();
    void checkingCalibResponseInSpectromSlot(double, double);


    void on_btnStopCalib_clicked();

//    void on_chbCalib_stateChanged(int arg1);

public slots:
    void connection();
    void ValueSlot (Pulse);
    void frqValueChenged(QString);
    void sendSpotFileToCart(QString);
    void getPlotDataFromThread();
    void histoMainPlotSlot(double, double);
    void stopSmartNoiseSlot();
    void stopExciterSlot();
    void calculatingCwTableSlot(QVector<double>,QVector<double>,QVector<QString>,QVector<long long int>);
    void on_DeviceResponseSlot(QByteArray);


    //    void sendLimitFrqSlot();
    //    void histoBarPlotSlot (double, double);
    //    void test(modesConst );


signals:
    void createPlotSignal1(QCustomSeries);
    void seekingStatusSignal(QString);
    void frqDomainIsOn();
    void startExciterSignal();
    void frqPltSignal();


    void frqValues      (QCustomSeries a);
    void frqValueSignal      (double,double);
    void PAPeakValueSignal   (double,double);
    void PASumMaxValueSignal (double,double);
    void priValueSignal      (double,double);
    void pwOneValueSignal    (double,double);
    void pwTwoValueSignal    (double,double);
    void toaValueSignal      (double,double);
    void ChannelValueSignal  (double,double);
    void smartNoiseIsActiveSignal  ();



    void hi();
    void cwFrqSignal        (double);
    void cwPowerSignal      (double);
    void spotFrqSignal      (double);
    void spotPowerSignal    (double);
    void sweepStartFrqSignal(double);
    void sweepStopFrqSignal (double);
    void sweepStepSignal    (double);
    void sweepPowerSignal   (double);
    void sweepDelaySignal   (double);
    void impFrqSignal       (double);
    void impPowerSignal     (double);
    void impPwSignal        (double);
    void impPriSignal       (double);
    void wbPowerSignal      (double);


    void cwOnBtnSignal     ();
    void cwOffBtnSignal    ();
    void spotOnBtnSignal   ();
    void spotOffBtnSignal  ();
    void sweepOnBtnSignal  ();
    void sweepOffBtnSignal ();
    void impOnBtnSignal    ();
    void impOffBtnSignal   ();
    void wbOnBtnSignal     ();
    void wbOffBtnSignal    ();

    void userLoggedInSignal(double);
    void sendCommandToDeviceSignal(QByteArray);
    //    void cwTabletoPlotSignal(qvdouble,double,QString);

};

#endif // RECEIVERMAIN_H
