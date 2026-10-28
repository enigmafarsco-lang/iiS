/**
 * @file receivermain.h
 * @brief Master Receiver Window, Real-Time Plotting, and DRFM Controller for eLynxSDR.
 *
 * ReceiverMain manages the primary operational interface:
 * 1. Hosts the high-performance spectrum analyzer and waterfall spectrogram (`oscmain`, `plot.cpp`).
 * 2. Manages Digital Radio Frequency Memory (DRFM) AXI register IO (`setupDrfmControlTab()`, `drfmregisterio.cpp`).
 * 3. Handles profile switching, calibration curves (Profile 100/200 vs 400), and RF switch matrices.
 * 4. Integrates the Exciter subsystem directly as an embedded tab for single-window operation.
 * 5. Controls multi-user authentication, roles, and administrative limits.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef RECEIVERMAIN_H
#define RECEIVERMAIN_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QTextStream>
#include <QTimer>
#include <unordered_map>

#include "receiver/oscmain.h"
#include "receiver/datafiles.h"
#include "components/GLChart/QGLchart.h"
#include "plotfunction.h"
#include "calculateplotdata.h"

namespace Ui {
class ReceiverMain;
}

/**
 * @class ReceiverMain
 * @brief Main GUI and DSP coordinator for SDR reception, spectrum plotting, and DRFM control.
 */
class ReceiverMain : public QWidget
{
    Q_OBJECT

    int cnt{0};
    void realTimePlot(QCustomPlot *customPlot);
    QCustomPlot * plt;

    void setupRealtimeDataDemo(QCustomPlot *customPlot);

    QGLchart *freqPlot;

    void creatingTimePlot(QCustomPlot *);
    void creatingHistogramPlot(QCustomPlot *, QCPAxisRect *, QCPBars*, QCPGraph *);
    void initGLPlot(QGLchart *glChart, ChartType glChartType);

    QVector2D vec;
    QCustomSeries a;

    plotfunction * t1;
    plotfunction * t2;
    plotfunction * t3;
    plotfunction * t4;
    plotfunction * t5;
    plotfunction * t6;
    plotfunction * t7;
    plotfunction * t8;

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
    bool writeToFile(QString, QString, QString, QString);

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

    void hiddenUiElements(bool);
    void saveCalibrToFile();
    QString const calibFile{dataFilePath(QStringLiteral("Calibration.txt"))};
    void checkFrqInColibr();
    void calibManagment();
    void calibInitial();

    /**
     * @brief Configures DRFM GUI controls and binds events to non-blocking AXI register updates.
     */
    void setupDrfmControlTab();

    /**
     * @brief Generates or verifies existence of 4-sample dummy zero DAC buffer file ('0 0 0 0').
     * @return Absolute file path to zero DAC buffer file.
     */
    QString ensureZeroDacBufferFile();

    /**
     * @brief Ensures DAC DMA buffer clock is running on DRFM startup without interrupting active exciter.
     */
    void ensureDrfmDacBufferRunning();

public:
    /**
     * @brief Constructs ReceiverMain widget.
     * @param parent Optional parent QWidget pointer.
     */
    explicit ReceiverMain(QWidget *parent = nullptr);

    /**
     * @brief Initializes sub-modules, plots, and hardware links.
     */
    void init();

    /**
     * @brief Embeds the Exciter UI into the tabbed receiver interface.
     * @param exciter Pointer to Exciter widget.
     */
    void addExciterTab(QWidget *exciter);

    /**
     * @brief Controls seeking and hopping status.
     * @param status True to stop capturing, false to resume.
     */
    void frqAndSeekStopCapturing(bool status);

    bool isConnect{false};                 ///< Transceiver hardware connection state.
    bool receiverIsConnected{true};        ///< Receiver operational status.
    oscmain * oscMain{nullptr};            ///< IIO Oscilloscope main controller.
    CalculatePlotData * frqDomainPlot;    ///< Real-time frequency domain plot data calculator.

    QDoubleSpinBox * frqSpn;               ///< RF Carrier Center Frequency spinbox.
    QDoubleSpinBox * att_TX1_Spn;          ///< TX1 digital attenuation control.
    QDoubleSpinBox * att_TX2_Spn;          ///< TX2 digital attenuation control.
    QCheckBox      * power_TX1_DownChk;    ///< TX1 powerdown / mute control.
    QCheckBox      * power_TX2_DownChk;    ///< TX2 powerdown / mute control.

    // Synchronized Exciter parameter spinboxes
    QDoubleSpinBox * spnCWFrq;
    QDoubleSpinBox * spnCWPower;
    QDoubleSpinBox * spnSpotFrq;
    QDoubleSpinBox * spnSpotPower;
    QDoubleSpinBox * spnImpulseFrq;
    QDoubleSpinBox * spnImpulsePower;
    QDoubleSpinBox * spnSweepPower;
    QDoubleSpinBox * spnWBPower;
    QSpinBox       * spnDelay;

    double minFrqLimit{};                  ///< Minimum RF tuning limit.
    double maxFrqLimit{};                  ///< Maximum RF tuning limit.

signals:
    /**
     * @brief Emitted when active profile bandwidth changes (100, 200, 400 MHz).
     * @param bwMhz Bandwidth in MHz.
     */
    void profileBandwidthChanged(int bwMhz);

    /**
     * @brief Emitted when TX calibration or attenuation changes.
     * @param rfFrqMhz Current RF frequency.
     * @param tx1AttenDb TX1 attenuation in dB.
     * @param tx2AttenDb TX2 attenuation in dB.
     */
    void txCalibChangedSignal(double rfFrqMhz, double tx1AttenDb, double tx2AttenDb);

    /**
     * @brief Emitted when user logs in with authenticated role.
     * @param isLoggedIn True if logged in.
     * @param role User permission role.
     */
    void userLoggedInSignal(bool isLoggedIn, QString role);

    /**
     * @brief Emitted to transmit command byte array to peripheral device.
     * @param cmd Byte array command.
     */
    void sendCommandToDeviceSignal(QByteArray cmd);

    /**
     * @brief Emitted when seeking status changes.
     * @param msg Status message string.
     */
    void seekingStatusSignal(QString msg);

    /**
     * @brief Emitted when smart noise tracking is active.
     * @param active True if active.
     */
    void smartNoiseIsActiveSignal(bool active);

    // Waveform synchronization signals
    void cwFrqSignal(double);
    void cwPowerSignal(double);
    void spotFrqSignal(double);
    void spotPowerSignal(double);
    void sweepStartFrqSignal(double);
    void sweepStopFrqSignal(double);
    void sweepStepSignal(double);
    void sweepPowerSignal(double);
    void sweepDelaySignal(int);
    void impFrqSignal(double);
    void impPowerSignal(double);
    void impPwSignal(double);
    void impPriSignal(double);
    void wbPowerSignal(double);
    void cwOnBtnSignal();
    void cwOffBtnSignal();
    void spotOnBtnSignal();
    void spotOffBtnSignal();
    void sweepOnBtnSignal();
    void sweepOffBtnSignal();
    void impOnBtnSignal();
    void impOffBtnSignal();
    void wbOnBtnSignal();
    void wbOffBtnSignal();

public slots:
    /**
     * @brief Handles response byte array from peripheral devices.
     * @param data Raw response payload.
     */
    void on_DeviceResponseSlot(QByteArray data);

    /**
     * @brief Handles RF frequency modifications from Exciter or UI.
     * @param val New RF carrier frequency in MHz.
     */
    void frqValueChenged(double val);

    /**
     * @brief Stops all active exciter transmissions.
     */
    void stopExciterSlot();

    /**
     * @brief Disables smart noise tracking mode.
     */
    void stopSmartNoiseSlot();
};

#endif // RECEIVERMAIN_H
