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
class exciter;
}

class Exciter : public QWidget
{
    Q_OBJECT

public:
    explicit Exciter(QWidget *parent = nullptr);
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
    void setModeActive(const QString &mode, bool on);

    // Phase 5: active ADRV9009 profile bandwidth (100/200/400 MHz)
    int profileBw{200};   // default profile: 200 MHz BW (matches ReceiverMain)

    enum tabState{
        CW,Spot,Sweep,Impulse,WB
    };

    void createImpulseFile(double, double, QString&, double);
    bool existsFile (const std::string& name);
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
    void updateMultiTargetRow(int row, bool applyDefaults);
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
    void getDataSlot();
    void setDataSlot();

    void on_btnDisableCW_clicked(bool checked);
    void on_btnDisableWB_clicked(bool checked);

public slots:
    void joshanFuncDataSlot();
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
    void smartNoiseIsActiveSlot();
    void isUserLoggedInSlot(double);



signals:
    void connectionStatusSignal(bool);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);
    void sendFrqDataSignal(QString);
    void sendPowerToCart(double);
    void sendSweepToCart();
    void sendStartFrqToCart(double);
    void sendStoptFrqToCart(double);
    void sendStepFrqToCart(double);
    void startHopp();
    void stoptHopp();
    void sendFileToCardSignal(QString, double,QString);
    void cwDdsParamsSignal(double freqMhz, double scaleDbfs, double phaseDeg);
    void changeDacSignal(QString);
    void modeActivitySignal(bool anyModeActive);
    void turnOffSmartNoiseSignal();
};


#endif // EXCITER_H
