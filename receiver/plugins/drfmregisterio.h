#ifndef DRFMREGISTERIO_H
#define DRFMREGISTERIO_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include <iio.h>

/**
 * One led_count_ip control register as exposed by the IIO led-count driver
 * (the device shown as "led-count" in iio-oscilloscope, whose Debug tab
 * accepts raw data for th0 / dacsel / ...).
 */
struct DrfmRegisterDef
{
    quint32 offset;        // offset inside the led_count_ip AXI window (base 0x43C30000)
    QString label;         // human readable name used in logs / UI
    QStringList attrNames; // IIO attribute name aliases (canonical name first)
    bool isSigned;         // value is a signed 32-bit quantity when rendered
};

/**
 * DrfmRegisterIO writes the led_count_ip registers the same way
 * iio-oscilloscope's Debug tab does:
 *
 *   1. named IIO debug attributes of the led-count device ("th0", "dacsel",
 *      "amplify", ...) - the raw data entry you use in the Debug tab;
 *   2. matching IIO channel attributes (e.g. "frequency" on voltage0 /
 *      voltage1) when the driver exposes a register there;
 *   3. direct register access on the led-count IIO device at its AXI offset
 *      ("the device addresses that IIO writes into");
 *   4. only as a last resort the historical MathWorks bridge
 *      mwipcore0:mmwr0 + reg_access (kept for old bitstreams).
 *
 * Because the led_count_ip AXI read mux does not return the written control
 * registers, confirmation is done over the board UART console with
 * echo / cat on /sys/bus/iio/devices (see UartRegisterConsole).
 */
class DrfmRegisterIO
{
public:
    DrfmRegisterIO();

    // Full register table (offset, label, attribute aliases).
    static const QList<DrfmRegisterDef> &registerMap();
    static const DrfmRegisterDef *defForOffset(quint32 offset);
    static QStringList attrNamesForOffset(quint32 offset);
    static QString labelForOffset(quint32 offset);

    // Corrected register write (paths 1-4 described above).
    bool writeRegister(quint32 offset, quint32 value,
                       const QString &label, QString *details = nullptr);

    // Writes the IIO channel attribute "frequency" of voltage0 / voltage1
    // (in and out) on the led-count device - the registers that appear as
    // voltage0/1 "frequency" entries in IIO / iio-oscilloscope.
    bool writeChannelFrequency(double frequency, QString *details = nullptr);

    // Multi-line report: every debug attribute the led-count device exposes
    // (name = value) with the register it represents (th0/dacsel/...) -
    // the IIO equivalent of scanning the iio-oscilloscope Debug tab.
    QString scanReport();

    // One-line description of the resolved hardware paths.
    QString backendInfo();

private:
    struct iio_device *ledCountDevice();
    struct iio_device *legacyBridgeDevice();
    void setLegacyBridgeEnabled(bool enable);

    QStringList debugAttrNames(struct iio_device *dev) const;
    QString matchAttrName(const QStringList &available,
                          const QStringList &wanted) const;
    bool writeDebugAttr(struct iio_device *dev, const QString &name,
                        quint32 value, bool isSigned, QString *how);
    bool writeChannelAttrs(struct iio_device *dev, const QStringList &wanted,
                           quint32 value, bool isSigned, QString *how);
    static QString valueText(quint32 value, bool isSigned);

    struct iio_device *m_ledDev;
    struct iio_device *m_legacyDev;
    mutable QStringList m_ledDebugAttrs;
    mutable bool m_ledDebugAttrsKnown;
};

#endif // DRFMREGISTERIO_H
