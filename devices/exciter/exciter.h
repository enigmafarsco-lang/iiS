/**
 * @file exciter.h
 * @brief Multi-Waveform Signal Generation & Exciter Engine for eLynxSDR.
 *
 * Exciter manages signal synthesis and hardware transmission for:
 * - Single/Multi-tone Continuous Wave (CW) via direct DDS.
 * - Spot Noise and WideBand Noise baseband generation.
 * - Linear Frequency Modulation (LFM) and Non-Linear FM (NLFM) chirps.
 * - Stepped-frequency sweeps and pulsed RF (PRI/PW).
 * - Multi-Target radar target synthesis summing up to 5 simultaneous targets.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef EXCITER_H
#define EXCITER_H

#include <QSet>
#include <QVector>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <unistd.h>
#include <iio.h>

#include "components/adrv/ADRV.h"
#include <QMessageBox>
#include "QJsonObject"
#include <QTimer>
#include <iostream>
#include <fstream>
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"


namespace Ui {
/**
 * @class exciter
 * @brief High-level controller class for exciter.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class exciter;
}

/**
 * @class Exciter
 * @brief High-level controller class for Exciter.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Exciter : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new Exciter instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit Exciter(QWidget *parent = nullptr);
    /**
     * @brief Destructor for Exciter, releasing allocated hardware resources, timers, and buffers.
     */
    ~Exciter();
    QDoubleSpinBox * spnCWPower;
    QDoubleSpinBox * spnSweepPower;
    QDoubleSpinBox * spnimpulsePower;
    QDoubleSpinBox * spnWidePower;
    QDoubleSpinBox * spnSpotPower;
    QDoubleSpinBox * spnImpPri;
    QDoubleSpinBox * spnImpPw;
    QSpinBox       * spnDelay;
    QLabel * statusMsg;

    QDoubleSpinBox * spnWBPower;
    //    QDoubleSpinBox * spnCWPower;


    QDoubleSpinBox * spnCWFrq;
    QDoubleSpinBox * spnSweepStop;
    QDoubleSpinBox * spnSweepStart;
    QDoubleSpinBox * spnSweepStep;
    QDoubleSpinBox * spnimpulseFrq;
    QDoubleSpinBox * spnWideFrq;
    QDoubleSpinBox * spnSpotFrq;
    QCheckBox * powerOn;
    QPushButton * startBtn;
    QPushButton * stopBtn;
    QPushButton * btnDisableSpot;
    QPushButton * btnSetCW;
    QPushButton * btnSetWB;
    QPushButton * btnLoadSpot;
    QPushButton * btnDisableCW;
    QPushButton * btnDisableWB;
    QPushButton * btnStopSweep;
    QPushButton * btnDisImpulse;
    QPushButton * btnStartSweep;
    QPushButton * btnLoadImpulse;

    double minFrqLimit{};
    double maxFrqLimit{};


private:
    Ui::exciter *ui;

    adrv * adrvObj ;
    QString ipAddress{};
    bool isConnect{false};
    bool firstRun{true};
    QString fullPath;
    QString fileName;

    bool isExciterOn{false};
    QSet<QString> activeModes;
    /**
     * @brief Sets the mode active parameter and updates internal state.
     * @param mode Input parameter specifying mode (const QString).
     * @param on Input parameter specifying on (bool).
     */
    void setModeActive(const QString &mode, bool on);

    // Phase 5: active ADRV9009 profile bandwidth (100/200/400 MHz)
    int profileBw{200};   // default profile: 200 MHz BW (matches ReceiverMain)

    enum tabState{
        CW,Spot,Sweep,Impulse,WB
    };

    /**
     * @brief Executes the create impulse file procedure for Exciter.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     * @param QString& Input parameter specifying QString& (QString&).
     * @param double Input parameter specifying double (double).
     */
    void createImpulseFile(double, double, QString&, double);
    /**
     * @brief Executes the exists file procedure for Exciter.
     * @param name Input parameter specifying name (const std::string&).
     * @return Output value of type bool.
     */
    bool existsFile (const std::string& name);
    /**
     * @brief Executes the returnfile path procedure for Exciter.
     * @param QString& Input parameter specifying QString& (QString&).
     * @return Output value of type bool.
     */
    bool returnfilePath(QString&);

    // Phase 6: search the plausible folders (app start dir, application
    // binary dir and their parents) for a relative file name and return
    // the first full path that exists (empty if not found anywhere).
    QString resolveFileInAppFolders(const QString &relPath);

    // Phase 6: Bridge Noise tab - band-limited noise files
    // bridge/bridge{N}mhz_{P}.txt (created by
    // files/bridge/generate_bridge.py, same engine as the spot files).
    QDoubleSpinBox *bridgeSpn{nullptr};

    // Phase 6: Multi Target tab - up to 5 selectable targets, each with its
    // own modulation (Spot/CW/Impulse/LFM/NLFM/Bridge noise), its own
    // specification and its own frequency shift (a complex exponential
    // multiplier).  Every selected target is rendered to its own txt file,
    // the shifted targets are summed into one I/Q stream (MultiTarget.txt)
    // which is sent to the DAC buffer.  LFM/NLFM rows carry just the three
    // LFM/NLFM tab parameters (start frequency / BW / T) and NO frequency
    // shift - the chirp formula is the exact one of the LFM/NLFM tabs.
    QCheckBox *mtEnable[5]{};
    QComboBox *mtType[5]{};
    QLabel *mtSpecLbl[5][3]{};
    QDoubleSpinBox *mtSpec[5][3]{};
    QLabel *mtShiftLbl[5]{};
    QDoubleSpinBox *mtShift[5]{};
    /**
     * @brief Updates and synchronizes the update multi target row data stream and visual indicators.
     * @param row Input parameter specifying row (int).
     * @param applyDefaults Input parameter specifying applyDefaults (bool).
     */
    void updateMultiTargetRow(int row, bool applyDefaults);
    /**
     * @brief Executes the build multi target waveform procedure for Exciter.
     * @return Output value of type bool.
     */
    bool buildMultiTargetWaveform();
    // Load a TEXT-header I/Q pair file (files/spot/generate.py and
    // files/bridge/generate_bridge.py format) into the sample buffers.
    // Returns the number of samples read (0 = empty or unreadable).
    int loadIqTextSamples(const QString &path,
                          QVector<double> &ti, QVector<double> &tq,
                          int nMax);

    // Phase 6: CW tab DDS tone parameters (shown on the CW tab like the
    // iio-oscilloscope DDS panel): the CW tab drives the on-chip DDS tone
    // engine on TX1+TX2 ("One CW Tone" mode), not the DAC buffer.
    QDoubleSpinBox *cwDdsFrqSpn{nullptr};
    QDoubleSpinBox *cwDdsScaleSpn{nullptr};
    QDoubleSpinBox *cwDdsPhaseSpn{nullptr};

    // Phase 6: LFM / NLFM tabs (same flow as the spot tab): start
    // frequency / bandwidth / pulse duration; Set synthesizes the chirp
    // at the profile playback rate and sends it to the DAC buffer.
    QDoubleSpinBox *lfmStartSpn{nullptr};
    QDoubleSpinBox *lfmBwSpn{nullptr};
    QDoubleSpinBox *lfmTSpn{nullptr};
    QDoubleSpinBox *nlfmStartSpn{nullptr};
    QDoubleSpinBox *nlfmBwSpn{nullptr};
    QDoubleSpinBox *nlfmTSpn{nullptr};
    bool buildChirpFile(const QString &fileName, bool nlfm,
                        double f0Mhz, double bwMhz, double tUs);
    // Phase 6: the ONE chirp formula shared by the LFM/NLFM tabs and the
    // Multi Target LFM/NLFM rows: phase(tau) = 2*pi*(f0*tau + B*tau^2/2T)
    // over a repeating pulse of duration T (us), NLFM adds raised-cosine
    // amplitude coding.  Fills n samples of I/Q at fsMhz MS/s.
    void fillChirpSamples(QVector<double> &ti, QVector<double> &tq, int n,
                          double fsMhz, bool nlfm,
                          double f0Mhz, double bwMhz, double tUs);

    // Phase 6: Sweep tab - stepped-sine I/Q synthesis (like the LFM tab,
    // in-app sine at baseband, fixed LO): tones at start, start+step, ...
    // toward stop (e.g. start 0, stop 10, step 2 -> 0/2/4/6/8/10 MHz),
    // equal dwell per tone inside the looping file (the per-tone time is
    // not critical), continuous phase across tone changes.
    bool buildSweepFile(const QString &fileName,
                        double fStartMhz, double fStopMhz, double fStepMhz);

    tabState currentTabState;
    QTimer exciterConnection;
    bool isUserLoggedIn{};

//    QPushButton * btnPtr ;

private slots:
    void on_btnDisableSpot_clicked();
    void on_btnDisImpulse_clicked();
    void on_btnStopSweep_clicked();
    /**
     * @brief Retrieves the current data slot value.
     */
    void getDataSlot();
    /**
     * @brief Sets the data slot parameter and updates internal state.
     */
    void setDataSlot();

    /**
     * @brief Slot handler responding to on btn disable c w clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnDisableCW_clicked(bool checked);
    /**
     * @brief Slot handler responding to on btn disable w b clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_btnDisableWB_clicked(bool checked);

public slots:
    /**
     * @brief Executes the joshan func data slot procedure for Exciter.
     */
    void joshanFuncDataSlot();
    /**
     * @brief Executes the joshan status data slot procedure for Exciter.
     */
    void joshanStatusDataSlot();

    // Phase 6: refresh the "Current P (attenuation)" readout - it shows
    // the calibrated TX power (P + TX calibration, see
    // constants/tx_calibration.h).  Called when the P changes and when
    // the TX calibration value of the Calibration tab changes.
    void refreshTxPowerDisplay();

    // Phase 5: active ADRV9009 profile bandwidth (100/200/400 MHz), set from
    // the receiver Profile tab. Selects spot{N}mhz_{P}.txt in the Spot tab.
    void setProfileBw(int bwMHz);
    // DAC-buffer channel pair selection (the Voltage 0/1 and Voltage 2/3
    // checkboxes): true when that pair should transmit the loaded file.
    bool voltage01Selected() const;
    bool voltage23Selected() const;
    //    void receiveIpAddressSlot(bool);
    void connectToDevice();
    void initConnection(bool);
    void addingCartElementsToExciter();
    void changingDacMsg(QString,QString);
    void joshanControlDataSlot(QJsonObject);
    /**
     * @brief Executes the smart noise is active slot procedure for Exciter.
     */
    void smartNoiseIsActiveSlot();
    /**
     * @brief Retrieves the current ser logged in slot value.
     * @param double Input parameter specifying double (double).
     */
    void isUserLoggedInSlot(double);



signals:
    /**
     * @brief Executes the connection status signal procedure for Exciter.
     * @param bool Input parameter specifying bool (bool).
     */
    void connectionStatusSignal(bool);
    /**
     * @brief Transmits send func data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendFuncDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send frq data signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendFrqDataSignal(QString);
    /**
     * @brief Transmits send power to cart command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     */
    void sendPowerToCart(double);
    /**
     * @brief Transmits send sweep to cart command/packet over the communication bus.
     */
    void sendSweepToCart();
    /**
     * @brief Transmits send start frq to cart command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     */
    void sendStartFrqToCart(double);
    /**
     * @brief Transmits send stopt frq to cart command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     */
    void sendStoptFrqToCart(double);
    /**
     * @brief Transmits send step frq to cart command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     */
    void sendStepFrqToCart(double);
    /**
     * @brief Executes the start hopp procedure for Exciter.
     */
    void startHopp();
    /**
     * @brief Executes the stopt hopp procedure for Exciter.
     */
    void stoptHopp();
    /**
     * @brief Transmits send file to card signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     * @param double Input parameter specifying double (double).
     * @param QString Input parameter specifying QString (QString).
     */
    void sendFileToCardSignal(QString, double,QString);
    /**
     * @brief Executes the cw dds params signal procedure for Exciter.
     * @param freqMhz Input parameter specifying freqMhz (double).
     * @param scaleDbfs Input parameter specifying scaleDbfs (double).
     * @param phaseDeg Input parameter specifying phaseDeg (double).
     */
    void cwDdsParamsSignal(double freqMhz, double scaleDbfs, double phaseDeg);
    /**
     * @brief Executes the change dac signal procedure for Exciter.
     * @param QString Input parameter specifying QString (QString).
     */
    void changeDacSignal(QString);
    /**
     * @brief Executes the mode activity signal procedure for Exciter.
     * @param anyModeActive Input parameter specifying anyModeActive (bool).
     */
    void modeActivitySignal(bool anyModeActive);
    /**
     * @brief Executes the turn off smart noise signal procedure for Exciter.
     */
    void turnOffSmartNoiseSignal();
};


#endif // EXCITER_H
