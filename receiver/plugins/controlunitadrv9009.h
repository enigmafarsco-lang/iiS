#ifndef CONTROLUNITADRV9009_H
#define CONTROLUNITADRV9009_H

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
    // dacsel, pdw/VGPO, inchannel, thcw).  Routes through DrfmRegisterIO:
    // named IIO debug attributes of the led-count device (the iio-oscilloscope
    // Debug tab registers th0/dacsel/...), matching IIO channel attributes
    // (e.g. frequency on voltage0/1), then direct led-count register access -
    // with mwipcore0:mmwr0 only as a fallback for old bitstreams.  The
    // software never opens the UART; every write prints the echo/cat command
    // to confirm the value in the board console (picocom).
    bool writeRegisterToHw(quint32 offset, quint32 value,
                           const QString &label, QString *details = nullptr);
    bool tryReadRegister(quint32 offset, quint32 *value);
    QString deviceDisplayName(struct iio_device *dev) const;

    DrfmRegisterIO registerIo;

public:
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
    // Writes the IIO channel register "frequency" that is represented by the
    // voltage0 / voltage1 channels (in_voltage0_frequency etc. in sysfs).
    bool setIioFrequency(double frequency, QString *details = nullptr);

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
