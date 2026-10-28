// ============================================================================
// File: controlunitadrv9009.h
// Subsystem: eLynxSDR plugins
// Description: Header definitions and interface declarations for controlunitadrv9009.h.
// ============================================================================

#ifndef CONTROLUNITADRV9009_H
#define CONTROLUNITADRV9009_H

#include <cmath>
#include <algorithm>
#include <limits>
#include <QWidget>
#include <QPushButton>
#include <QtGlobal>
#include <QStringList>
#include "qdebug.h"
#include <receiver/globals.h>
#include <receiver/plugins/drfmregisterio.h>

namespace Ui {
class ControlUnitADRV9009;
}

class ControlUnitADRV9009 : public QWidget
{
    Q_OBJECT

public:
    explicit ControlUnitADRV9009(QWidget *parent = nullptr);
    ~ControlUnitADRV9009();

    void init();
    //saeid raziani
    QSpinBox * TH1 = new QSpinBox;
    QSpinBox * TH2 = new QSpinBox;
    QDoubleSpinBox * THCW1 = new QDoubleSpinBox;
    QSpinBox * THCW2 = new QSpinBox;
    QSpinBox * THCW3 = new QSpinBox;
    QPushButton * btnTh1;
    QPushButton * btnTh2;
    QPushButton * btnThCw1;
    QPushButton * btnThCw2;
    QPushButton * btnThCw3;
    QPushButton * btnWrite;
    //    QPushButton * btnWrite_th1;
    //    QPushButton * btnWrite_th2;
    QComboBox * logouti;
    //
    QString readDevice="mwipcore0:mmrd0";
    QString writeDevice="mwipcore0:mmwr0";
    QString attr_name="reg_access";

    QMap<QString,double> readItemAddress;
    QMap<QString,double> writeItemAddress;
    QMap<QString,double> readItemValues;
    QMap<QString,double> writeItemValues;



private:
    Ui::ControlUnitADRV9009 *ui;
    struct iio_device *readDev = nullptr;
    struct iio_device *writeDev = nullptr;

    void EnableWrite();
    void DisableWrite();

    void resolveRegisterDevices();
    bool writeRegisterVerified(quint32 offset, quint32 value,
                               const QString &label, QString *details);
    // Historical mwipcore0:mmwr0 transaction (reg_access + reg_write),
    // kept unchanged for every tab/menu outside the DRFM tab.
    bool writeRegisterLikeThreshold(quint32 offset, quint32 value,
                                    const QString &label, QString *details = nullptr);
    // Corrected write path for the DRFM tab controls ONLY (th0/amplify,
    // dacsel, pdw/VGPO, inchann, thcw).  Routes through DrfmRegisterIO:
    // the IIO channel "raw" attributes of the led-count-iio device
    // (out_count1_th0_raw, out_count15_dacseles_raw, ... - the raw-data
    // entries of the iio-oscilloscope Debug tab), then named debug
    // attributes and direct led-count register access - with mwipcore0:mmwr0
    // only as a fallback for old bitstreams.  The software never opens the
    // UART; every write prints the cat command to confirm the value in the
    // board console (picocom).
    bool writeRegisterToHw(quint32 offset, quint32 value,
                           const QString &label, QString *details = nullptr);
    bool tryReadRegister(quint32 offset, quint32 *value);
    QString deviceDisplayName(struct iio_device *dev) const;

    DrfmRegisterIO registerIo;

public:
    // Technical Specification Constants
    static constexpr double SPEED_OF_LIGHT                = 3.0e8;      // m/s (approx 3.0e8 m/s)
    static constexpr double SYSTEM_INTRINSIC_LATENCY_NS   = 80.0;       // ns (~12 m round-trip)
    static constexpr double SYSTEM_INTRINSIC_RANGE_M      = 12.0;       // m
    static constexpr double MIN_TARGET_RANGE_M            = 150.0;      // 150 m
    static constexpr double MAX_TARGET_RANGE_M            = 25000.0;    // 25 km
    static constexpr double RANGE_RESOLUTION_M            = 10.0;       // 10 m
    static constexpr double DOPPLER_RESOLUTION_HZ         = 10.0;       // 10 Hz
    static constexpr double MAX_RF_FREQUENCY_MHZ          = 6000.0;     // 6 GHz
    static constexpr double MIN_WAVELENGTH_M              = 0.05;       // 5 cm (at 6 GHz)

    // Mathematical Formulations for Radar Target Simulation
    static double calculateDelayUs(double rangeMeters, double c = SPEED_OF_LIGHT)
    {
        return (2.0 * rangeMeters / c) * 1.0e6; // delay in microseconds (us)
    }

    static double calculateRangeMeters(double delayUs, double c = SPEED_OF_LIGHT)
    {
        return (c * (delayUs * 1.0e-6)) / 2.0; // range in meters (m)
    }

    static double calculateTotalOscilloscopeDelayUs(double rangeMeters,
                                                    double intrinsicNs = SYSTEM_INTRINSIC_LATENCY_NS,
                                                    double c = SPEED_OF_LIGHT)
    {
        return calculateDelayUs(rangeMeters, c) + (intrinsicNs * 1.0e-3);
    }

    static double calculateWavelengthM(double centerFreqMhz, double c = SPEED_OF_LIGHT)
    {
        if (centerFreqMhz <= 0.0) return 0.0;
        return c / (centerFreqMhz * 1.0e6);
    }

    static double calculateDopplerHz(double velocityMps, double centerFreqMhz, double c = SPEED_OF_LIGHT)
    {
        // fd = 2 * v * f0 / c = 2 * v / lambda
        return (2.0 * velocityMps * (centerFreqMhz * 1.0e6)) / c;
    }

    static double calculateVelocityMps(double dopplerHz, double centerFreqMhz, double c = SPEED_OF_LIGHT)
    {
        if (centerFreqMhz <= 0.0) return 0.0;
        // v = fd * c / (2 * f0) = fd * lambda / 2
        return (dopplerHz * c) / (2.0 * (centerFreqMhz * 1.0e6));
    }

    static qint32 calculatePhaseStep(double dopplerHz, double sampleRateMhz = 122.88)
    {
        // thcw = round(fd * 2^32 / Fs)
        const double fs = sampleRateMhz * 1.0e6;
        if (fs <= 0.0) return 0;
        return static_cast<qint32>(std::round(dopplerHz * (4294967296.0 / fs)));
    }

    static double calculateDopplerFromPhaseStep(qint32 phaseStep, double sampleRateMhz = 122.88)
    {
        const double fs = sampleRateMhz * 1.0e6;
        return (static_cast<double>(phaseStep) * fs) / 4294967296.0;
    }

    // Hardware map confirmed from the supplied tcl07020.tcl,
    // AddressSegments.csv, led_count_ip_addr_decoder.v and MUXDACS_ip.v:
    // led_count_ip_0 AXI4-Lite base             = 0x43C30000
    // TH0      -> MUXDACS amplify               = +0x100
    // PDW      -> MUXDACS doppler_enable        = +0x110
    // INchann  -> MUXDACS doppler_phase_offset  = +0x118
    // thcw     -> MUXDACS doppler_phase_step    = +0x11C
    // DACseles -> MUXDACS DacSel                = +0x138
    // DacSel=1 selects DRFM; DacSel=0 selects the DMA/noise path.
    static constexpr quint32 LED_COUNT_AXI_BASE = 0x43C30000u;
    static constexpr quint32 REG_AMPLY          = 0x00000100u;
    static constexpr quint32 REG_VGPO_ENABLE    = 0x00000110u;
    static constexpr quint32 REG_VGPO_OFFSET    = 0x00000118u;
    static constexpr quint32 REG_VGPO_STEP      = 0x0000011Cu;
    static constexpr quint32 REG_DRFM_SELECT    = 0x00000138u;

    // Apply a requested state/value to the FPGA and report the actual
    // register transaction in details. Nothing is written until the user
    // presses Set in the DRFM control tab.
    bool setDrfmEnabled(bool enabled, QString *details = nullptr);
    bool setAmplifyValue(quint16 value, bool enabled = true, QString *details = nullptr);
    bool setVgpoEnabled(bool enabled, QString *details = nullptr);
    bool setVgpoParameters(quint32 phaseOffset, qint32 phaseStep, QString *details = nullptr);
    bool setVgpoValue(quint32 phaseOffset, qint32 phaseStep, bool enabled = true, QString *details = nullptr);
    // Compatibility overload: phase offset defaults to zero.
    bool setVgpoValue(qint32 phaseStep, bool enabled = true, QString *details = nullptr)
    { return setVgpoValue(0u, phaseStep, enabled, details); }

    // Set Target Range & Propagation Delay
    bool setTargetRange(double rangeMeters, QString *details = nullptr);
    bool setTargetDelay(double delayUs, QString *details = nullptr);

    // Set Target Radial Velocity & Doppler Shift
    bool setTargetVelocity(double velocityMps, double centerFreqMhz, bool enabled = true, QString *details = nullptr);
    bool setTargetDoppler(double dopplerHz, double centerFreqMhz, bool enabled = true, QString *details = nullptr);

    // Scan report: every led-count IIO debug attribute (name = value) and the
    // register it represents (th0/dacsel/...).  Pure IIO - no UART.
    QString scanIioRegisters() { return registerIo.scanReport(); }
    QString registerBackendInfo();

public slots:
    void WriteAll();

private slots:
    void ReadAll();

    void Write(uint32_t address,uint32_t val);

signals:
    void sendTransmitSignal(int);
    void setNoiseSignal(int);
    void sendFileToDacSignal();
    void setAutoAmpSignal(int);
    void setChannelSignal(double);
    void setChannelFromTable(double);
    void setInchannelSignal(double);

    void setPdivSignal     (int);
    void setteTa_azSignal  (int);
    void setNum_SideSignal (int);
    void setNum_BackSignal (int);
    void setP_to_BSignal   (int);
    void setNPSSignal      (int);
    void setP_to_SSignal   (int);
    void enPatternSignal   (int);
};


#endif // CONTROLUNITADRV9009_H
