/**
 * @file exciter.h
 * @brief Multi-Waveform Signal Generation & Exciter Engine for eLynxSDR.
 *
 * Exciter generates complex baseband radar and electronic warfare waveforms:
 * - Continuous Wave (CW) single-tone generator via ADRV9009 internal DDS engine.
 * - Spot Noise: Gaussian band-limited noise for spot jamming.
 * - Frequency Sweep: Stepped or continuous chirp across specified bandwidths.
 * - Impulse / Pulsed RF: Configurable pulse width (Tp) and pulse repetition interval (PRI).
 * - Wideband Noise: High-bandwidth pseudo-random Gaussian noise.
 * - Bridge Noise: Multi-carrier comb noise for wideband channel stimulation.
 * - Multi-Target Simulator: Up to 5 concurrent independent radar target returns.
 * - Linear Frequency Modulated (LFM Chirp): Radar chirp pulses.
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
#include <QPushButton>
#include <QTimer>
#include <unistd.h>
#include <iio.h>

#include "components/adrv/ADRV.h"
#include <QMessageBox>
#include "QJsonObject"
#include <iostream>
#include <fstream>
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"

namespace Ui {
class exciter;
}

/**
 * @class Exciter
 * @brief GUI and Signal Synthesis engine for multi-waveform RF transmission.
 */
class Exciter : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the Exciter widget and initializes waveform sub-tabs.
     * @param parent Optional parent QWidget pointer.
     */
    explicit Exciter(QWidget *parent = nullptr);

    /**
     * @brief Destructor releasing allocated resources.
     */
    ~Exciter();

    // Public UI controls accessed by MainWindow for bi-directional synchronization
    QDoubleSpinBox * spnCWPower;       ///< Output attenuation control for CW mode.
    QDoubleSpinBox * spnSweepPower;    ///< Output attenuation control for Sweep mode.
    QDoubleSpinBox * spnimpulsePower;  ///< Output attenuation control for Impulse mode.
    QDoubleSpinBox * spnWidePower;     ///< Output attenuation control for WideBand mode.
    QDoubleSpinBox * spnSpotPower;     ///< Output attenuation control for Spot noise mode.
    QDoubleSpinBox * spnImpPri;        ///< Pulse Repetition Interval (PRI) spinbox in us.
    QDoubleSpinBox * spnImpPw;         ///< Pulse Width (Pw) spinbox in us.
    QSpinBox       * spnDelay;         ///< Hop / trigger delay spinbox.
    QLabel         * statusMsg;        ///< Status message label for DMA updates.
    QDoubleSpinBox * spnWBPower;       ///< WideBand noise power spinbox.

    QDoubleSpinBox * spnCWFrq;         ///< CW center frequency spinbox.
    QDoubleSpinBox * spnSweepStop;     ///< Sweep stop frequency spinbox.
    QDoubleSpinBox * spnSweepStart;    ///< Sweep start frequency spinbox.
    QDoubleSpinBox * spnSweepStep;     ///< Sweep frequency step spinbox.
    QDoubleSpinBox * spnimpulseFrq;    ///< Impulse center frequency spinbox.
    QDoubleSpinBox * spnWideFrq;       ///< WideBand noise center frequency spinbox.
    QDoubleSpinBox * spnSpotFrq;       ///< Spot noise center frequency spinbox.
    QCheckBox      * powerOn;          ///< Master TX power amplifier enable toggle.
    QPushButton    * startBtn;         ///< Start hopping trigger button.
    QPushButton    * stopBtn;          ///< Stop hopping trigger button.
    QPushButton    * btnDisableSpot;   ///< Disable spot noise button.
    QPushButton    * btnSetCW;         ///< Enable CW mode button.
    QPushButton    * btnSetWB;         ///< Enable WideBand noise button.
    QPushButton    * btnLoadSpot;      ///< Enable Spot noise button.
    QPushButton    * btnDisableCW;     ///< Disable CW mode button.
    QPushButton    * btnDisableWB;     ///< Disable WideBand noise button.
    QPushButton    * btnStopSweep;     ///< Stop sweep button.
    QPushButton    * btnDisImpulse;    ///< Disable impulse button.
    QPushButton    * btnStartSweep;    ///< Start sweep button.
    QPushButton    * btnLoadImpulse;   ///< Enable impulse button.

    double minFrqLimit{};              ///< Minimum allowable RF tuning frequency.
    double maxFrqLimit{};              ///< Maximum allowable RF tuning frequency.

private:
    Ui::exciter * ui;

    adrv * adrvObj;                    ///< Low-level ADRV9009 hardware interface helper.
    QString ipAddress{};               ///< Target hardware IP address.
    bool isConnect{false};             ///< Hardware connection status.
    bool firstRun{true};               ///< First run initialization flag.
    QString fullPath;                  ///< Full filesystem path of active DAC sample file.
    QString fileName;                  ///< Filename of active DAC sample file.

    bool isExciterOn{false};           ///< Master exciter active transmission status.
    QSet<QString> activeModes;         ///< Set of currently active exciter waveform modes.

    /**
     * @brief Tracks active status for an exciter mode and broadcasts modeActivitySignal.
     * @param mode Mode name (e.g. "cw", "spot", "lfm").
     * @param on True if active, false if disabled.
     */
    void setModeActive(const QString &mode, bool on);

    int profileBw{200};                ///< Active ADRV9009 profile bandwidth (100, 200, or 400 MHz).

    enum tabState { CW, Spot, Sweep, Impulse, WB };

    /**
     * @brief Synthesizes pulsed RF IQ samples and writes them to a formatted text file.
     * @param pulseWidth Pulse duration in microseconds.
     * @param pri Pulse repetition interval in microseconds.
     * @param filePath Output file destination path.
     * @param sampleRate Sampling rate in MSPS.
     */
    void createImpulseFile(double pulseWidth, double pri, QString &filePath, double sampleRate);

    /**
     * @brief Checks if a file exists on the local filesystem.
     * @param name File path string.
     * @return True if exists.
     */
    bool existsFile(const std::string &name);

    /**
     * @brief Resolves relative file path against application search directories.
     * @param path In/out path reference.
     * @return True if resolved.
     */
    bool returnfilePath(QString &path);

    /**
     * @brief Resolves relative file name against standard directories.
     * @param relativeName Filename to locate.
     * @return Absolute file path if found, or relativeName if not.
     */
    QString resolveDataPath(const QString &relativeName) const;

    // CW Tab DDS Controls
    QDoubleSpinBox * cwDdsFrqSpn{nullptr};   ///< DDS Tone frequency in MHz.
    QDoubleSpinBox * cwDdsScaleSpn{nullptr}; ///< DDS Tone scale in dBFS.
    QDoubleSpinBox * cwDdsPhaseSpn{nullptr}; ///< DDS Tone initial phase in degrees.

    // Bridge Noise Controls
    QDoubleSpinBox * bridgeSpn{nullptr};     ///< Bridge noise bandwidth spinbox.

    // Multi-Target Simulator Controls (5 concurrent targets)
    struct MultiTargetRow {
        QCheckBox      * chkEnable;  ///< Target channel enable checkbox.
        QComboBox      * cmbWave;    ///< Waveform selector (Spot, CW, LFM).
        QDoubleSpinBox * spnBw;      ///< Bandwidth in MHz.
        QDoubleSpinBox * spnShift;   ///< Doppler frequency shift in MHz.
    };
    QVector<MultiTargetRow> multiTargetRows; ///< Target rows array.
    QPushButton * btnGenerateMultiTarget{nullptr};
    QPushButton * btnDisableMultiTarget{nullptr};

    // LFM Chirp Controls
    QDoubleSpinBox * lfmStartFrqSpn{nullptr}; ///< LFM chirp start frequency in MHz.
    QDoubleSpinBox * lfmBwSpn{nullptr};       ///< LFM chirp bandwidth in MHz.
    QDoubleSpinBox * lfmTSpn{nullptr};        ///< LFM pulse duration in microseconds.
    QPushButton    * btnSetLfm{nullptr};      ///< Set LFM button.
    QPushButton    * btnDisableLfm{nullptr};  ///< Disable LFM button.

    // DAC TX Channel Selection (Voltage 0/1 vs Voltage 2/3)
    QCheckBox * chkVoltage01{nullptr};        ///< Route to TX1 (Voltage 0/1).
    QCheckBox * chkVoltage23{nullptr};        ///< Route to TX2 (Voltage 2/3).

public:
    /**
     * @brief Checks if Voltage 0/1 (TX1) is selected for DAC transmission.
     * @return True if checked.
     */
    bool voltage01Selected() const;

    /**
     * @brief Checks if Voltage 2/3 (TX2) is selected for DAC transmission.
     * @return True if checked.
     */
    bool voltage23Selected() const;

signals:
    /**
     * @brief Emitted to upload a generated IQ waveform file into the ADRV9009 DAC DMA buffer.
     * @param filePath Absolute path to sample file.
     * @param scaleFactor Digital amplitude scaling factor.
     * @param mode Waveform mode label string.
     */
    void sendFileToCardSignal(QString filePath, double scaleFactor, QString mode);

    /**
     * @brief Emitted to switch or disable DAC transmission mode.
     * @param mode Mode string (e.g. "CW", "Spot", "disable").
     */
    void changeDacSignal(QString mode);

    /**
     * @brief Emitted when DDS tone parameters change on CW tab.
     * @param freqMHz DDS tone frequency in MHz.
     * @param scaleDbfs DDS tone amplitude in dBFS.
     * @param phaseDeg DDS tone phase in degrees.
     */
    void cwDdsParamsSignal(double freqMHz, double scaleDbfs, double phaseDeg);

    /**
     * @brief Emitted to update transceiver RF carrier frequency.
     * @param frq Target center frequency in MHz.
     */
    void sendFrqDataSignal(double frq);

    /**
     * @brief Emitted to synchronize output power attenuation with receiver tab.
     * @param power Power attenuation in dB.
     */
    void sendPowerToCart(double power);

    /**
     * @brief Emitted when any exciter mode starts or all modes stop.
     * @param anyActive True if at least one waveform mode is currently transmitting.
     */
    void modeActivitySignal(bool anyActive);

    /**
     * @brief Emitted to start frequency hopping mode.
     */
    void startHopp();

    /**
     * @brief Emitted to stop frequency hopping mode.
     */
    void stoptHopp();

    /**
     * @brief Emitted to disable smart noise tracking mode.
     */
    void turnOffSmartNoiseSignal();

    /**
     * @brief Emitted to report functional telemetry to Joshan controller.
     * @param data Telemetry payload struct.
     */
    void sendFuncDataToJoshanSignal(JoshanFuncDataStruct data);

    /**
     * @brief Emitted to report operational status to Joshan controller.
     * @param data Status payload struct.
     */
    void sendStatusDataToJoshanSignal(JoshanStatusDataStruct data);

public slots:
    /**
     * @brief Updates active profile bandwidth (100, 200, 400 MHz) and adapts waveform files.
     * @param bwMhz Bandwidth in MHz.
     */
    void setProfileBw(int bwMhz);

    /**
     * @brief Updates calibrated TX output power readout on the UI.
     * @param rfFrqMhz Current RF carrier frequency in MHz.
     * @param tx1AttenDb TX1 digital attenuation in dB.
     * @param tx2AttenDb TX2 digital attenuation in dB.
     */
    void refreshTxPowerDisplay(double rfFrqMhz, double tx1AttenDb, double tx2AttenDb);

    /**
     * @brief Handles IP address received from main connection dialog.
     * @param ip Hardware IP address string.
     */
    void receiveIpAddressSlot(QString ip);

    /**
     * @brief Initializes hardware connection for Exciter.
     * @param status Connection success flag.
     */
    void initConnection(bool status);

    /**
     * @brief Receives DAC status messages from transceiver plugin.
     * @param msg Status message string.
     */
    void changingDacMsg(QString msg);

    /**
     * @brief Updates UI controls based on logged-in user permissions.
     * @param isLoggedIn User authentication state.
     * @param role User permission role string.
     */
    void isUserLoggedInSlot(bool isLoggedIn, QString role);

    /**
     * @brief Main slot executing waveform generation and DAC upload when a Set button is clicked.
     */
    void setDataSlot();

    /**
     * @brief Receives Joshan control commands for electronic countermeasure modes.
     * @param data Control payload struct.
     */
    void joshanControlDataSlot(JoshanControlDataStruct data);

    /**
     * @brief Requests functional data report for Joshan controller.
     */
    void joshanFuncDataSlot();

    /**
     * @brief Requests status data report for Joshan controller.
     */
    void joshanStatusDataSlot();

    /**
     * @brief Handles smart noise activation state updates.
     * @param active True if smart noise is active.
     */
    void smartNoiseIsActiveSlot(bool active);
};

#endif // EXCITER_H
