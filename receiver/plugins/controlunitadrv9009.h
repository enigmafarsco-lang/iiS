/**
 * @file controlunitadrv9009.h
 * @brief Low-Level AXI Register Interface & DRFM Hardware Control Unit.
 *
 * ControlUnitADRV9009 manages direct FPGA memory-mapped AXI registers:
 * - Direct register read/write operations for DRFM target parameters (Range, Doppler, TH0/TH1/TH2, THCW).
 * - Fast IIO channel raw attribute writes for low-latency hardware synchronization.
 * - Hardware AXI status register monitoring (FIFO state, sync locks).
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef CONTROLUNITADRV9009_H
#define CONTROLUNITADRV9009_H

#include <cmath>
#include <algorithm>
#include <limits>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMap>
#include <QtGlobal>
#include <QStringList>
#include <QDebug>

#include "receiver/globals.h"
#include "receiver/plugins/drfmregisterio.h"

namespace Ui {
class ControlUnitADRV9009;
}

/**
 * @class ControlUnitADRV9009
 * @brief Widget and driver managing memory-mapped AXI registers for transceiver & DRFM IP.
 */
class ControlUnitADRV9009 : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs ControlUnitADRV9009 widget.
     * @param parent Optional parent QWidget pointer.
     */
    explicit ControlUnitADRV9009(QWidget *parent = nullptr);

    /**
     * @brief Destructor releasing device pointers.
     */
    ~ControlUnitADRV9009();

    /**
     * @brief Initializes register address lookup tables and binds UI elements.
     */
    void init();

    QSpinBox       * TH1;
    QSpinBox       * TH2;
    QDoubleSpinBox * THCW1;
    QSpinBox       * THCW2;
    QSpinBox       * THCW3;
    QPushButton    * btnTh1;
    QPushButton    * btnTh2;
    QPushButton    * btnThCw1;
    QPushButton    * btnThCw2;
    QPushButton    * btnThCw3;
    QPushButton    * btnWrite;
    QComboBox      * logouti;

    QString readDevice{"mwipcore0:mmrd0"};
    QString writeDevice{"mwipcore0:mmwr0"};
    QString attr_name{"reg_access"};

    QMap<QString, double> readItemAddress;
    QMap<QString, double> writeItemAddress;
    QMap<QString, double> readItemValues;
    QMap<QString, double> writeItemValues;

private:
    Ui::ControlUnitADRV9009 * ui;
    struct iio_device * readDev{nullptr};
    struct iio_device * writeDev{nullptr};

    void EnableWrite();
    void DisableWrite();

    void resolveRegisterDevices();
    bool writeRegisterVerified(quint32 offset, quint32 value,
                               const QString &label, QString *details);
    bool writeRegisterLikeThreshold(quint32 offset, quint32 value,
                                    const QString &label, QString *details = nullptr);
    bool writeDrfmRegister(quint32 offset, quint32 value,
                           const QString &label, QString *details = nullptr);

    DrfmRegisterIO drfmRegIO;

    static const quint32 ADDR_TH0_AMPLIFY  = 0x43C30100;
    static const quint32 ADDR_PDW          = 0x43C30110;
    static const quint32 ADDR_INCHANN      = 0x43C30118;
    static const quint32 ADDR_THCW         = 0x43C3011C;
    static const quint32 ADDR_DACSELES     = 0x43C30138;
    static const quint32 ADDR_VGPO         = 0x43C3013C;
    static const quint32 ADDR_TH1          = 0x43C30140;
    static const quint32 ADDR_TH2          = 0x43C30144;
    static const quint32 ADDR_DOPPLER_FREQ = 0x43C30160;

public slots:
    /**
     * @brief Sets DRFM amplitude attenuation (TH0).
     * @param value Linear attenuation / scale word.
     */
    void setTh0Amplify(quint32 value);

    /**
     * @brief Sets DRFM pulse description word (PDW).
     * @param value 32-bit PDW value.
     */
    void setPdw(quint32 value);

    /**
     * @brief Sets DRFM input channel phase offset.
     * @param value 32-bit phase word.
     */
    void setInchann(quint32 value);

    /**
     * @brief Sets DRFM Doppler NCO phase step (THCW).
     * @param value 32-bit phase increment word: thcw = round(fd * 2^32 / Fs).
     */
    void setThcw(quint32 value);

    /**
     * @brief Sets DRFM DAC multiplexer selector (DACSEL).
     * @param value 0 = DMA buffer, 1 = DRFM target echo.
     */
    void setDacseles(quint32 value);

    /**
     * @brief Sets VGPO register value.
     * @param value 32-bit register value.
     */
    void setVgpo(quint32 value);

    /**
     * @brief Sets secondary threshold TH1.
     * @param value 32-bit threshold value.
     */
    void setTh1(quint32 value);

    /**
     * @brief Sets secondary threshold TH2.
     * @param value 32-bit threshold value.
     */
    void setTh2(quint32 value);

    /**
     * @brief Sets target Doppler frequency register directly.
     * @param value Doppler frequency in Hz.
     */
    void setDopplerFreq(qint32 value);
};

#endif // CONTROLUNITADRV9009_H
