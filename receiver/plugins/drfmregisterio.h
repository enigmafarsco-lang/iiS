#ifndef DRFMREGISTERIO_H
#define DRFMREGISTERIO_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include <iio.h>

/**
 * One led_count_ip control register as exposed by the "led-count-iio"
 * driver (iio:device0 on the board).  The driver maps every register to an
 * IIO channel with a "raw" attribute - the same entries you write raw data
 * for in the iio-oscilloscope Debug tab:
 *
 *     out_count1_th0_raw       <- TH0 / amplify   (AXI 0x43C30100)
 *     out_count5_pdw_raw       <- PDW             (AXI 0x43C30110)
 *     out_count7_inchann_raw   <- phase offset    (AXI 0x43C30118)
 *     out_count8_thcw_raw      <- phase step      (AXI 0x43C3011C)
 *     out_count15_dacseles_raw <- dacsel          (AXI 0x43C30138)
 *     ...
 *
 * Each channel also has a "label" attribute naming the register ("th0",
 * "dacseles", ...), which is what this class matches against.
 */
struct DrfmRegisterDef
{
    quint32 offset;        // offset inside the led_count_ip AXI window (base 0x43C30000)
    QString label;         // human readable name used in logs / UI
    QStringList attrNames; // register-name aliases (canonical name first)
    bool isSigned;         // value is a signed 32-bit quantity when rendered
};

/**
 * DrfmRegisterIO writes the led_count_ip registers the same way
 * iio-oscilloscope's Debug tab writes raw data for th0 / dacsel / ...:
 *
 *   1. the IIO channel "raw" attribute whose channel/label represents the
 *      register (out_countN_<name>_raw on led-count-iio) - this is the
 *      correct write mechanism;
 *   2. matching named IIO debug attributes, when a driver exposes any
 *      (kept for other bitstreams);
 *   3. direct register access on the led-count IIO device at its AXI offset;
 *   4. only as a last resort the historical MathWorks bridge
 *      mwipcore0:mmwr0 + reg_access (old bitstreams).
 *
 * The software never opens the UART.  Because the AXI read mux cannot return
 * the written control registers, each write prints the exact `cat` command
 * for the board console (picocom) so the value can be confirmed by hand.
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

    // Multi-line report: every IIO channel attribute of the led-count device
    // (label + raw + sysfs file) with the register it represents - the IIO
    // equivalent of reading the iio-oscilloscope Debug tab list.
    QString scanReport();

    // One-line description of the resolved hardware paths.
    QString backendInfo();

private:
    // A channel/attribute pair that carries a register's raw value, e.g.
    // channel "count1_th0" with attribute "raw" (sysfs out_count1_th0_raw).
    struct ChannelRawMatch
    {
        struct iio_channel *channel;
        QString attrName;   // "raw", or "<name>_raw" for the alternate layout
        QString sysfsName;  // e.g. "out_count1_th0_raw"
        QString channelDesc;
    };

    struct iio_device *ledCountDevice();
    struct iio_device *legacyBridgeDevice();
    void setLegacyBridgeEnabled(bool enable);

    // Finds the channel raw attribute that represents one of the register
    // names in `wanted` (matched against channel id/name with any leading
    // "countN" stripped and against the channel's "label" attribute).
    bool findChannelRaw(struct iio_device *dev, const QStringList &wanted,
                        ChannelRawMatch *match) const;
    bool writeChannelRaw(struct iio_device *dev, const QStringList &wanted,
                         quint32 value, bool isSigned, QString *how,
                         ChannelRawMatch *usedMatch = nullptr);

    QStringList debugAttrNames(struct iio_device *dev) const;
    QString matchAttrName(const QStringList &available,
                          const QStringList &wanted) const;
    bool writeDebugAttr(struct iio_device *dev, const QString &name,
                        quint32 value, bool isSigned, QString *how);
    static QString valueText(quint32 value, bool isSigned);
    static QStringList channelNameCandidates(struct iio_channel *ch);

    struct iio_device *m_ledDev;
    struct iio_device *m_legacyDev;
    mutable QStringList m_ledDebugAttrs;
    mutable bool m_ledDebugAttrsKnown;
};

#endif // DRFMREGISTERIO_H
