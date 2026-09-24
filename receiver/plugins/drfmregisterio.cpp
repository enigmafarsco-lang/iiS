#include "drfmregisterio.h"

#include <receiver/globals.h>

#include <QDebug>
#include <QPair>

#include <cstring>

namespace {

QString normalizedAttrKey(const QString &name)
{
    QString key = name.toLower();
    key.remove(QLatin1Char('_'));
    key.remove(QLatin1Char('-'));
    key.remove(QLatin1Char(' '));
    return key;
}

QString hexText(quint32 value, int width)
{
    return QStringLiteral("0x%1")
            .arg(QString::number(value, 16).toUpper(), width, QLatin1Char('0'));
}

// Same signature as globals::__connect_widget so it works with the libiio
// callback typedef this project already compiles against.  Collects the
// debug-attribute names the led-count driver exposes (th0, dacsel, ...).
int collectDebugAttrNames(struct iio_device *dev, const char *attr,
                          const char *value, size_t len, void *d)
{
    Q_UNUSED(dev)
    Q_UNUSED(value)
    Q_UNUSED(len)

    QStringList *names = static_cast<QStringList *>(d);
    if (attr && *attr)
        names->append(QString::fromLocal8Bit(attr));
    return 0;
}

// Collects name = value pairs for the scan report.
int collectDebugAttrPairs(struct iio_device *dev, const char *attr,
                          const char *value, size_t len, void *d)
{
    Q_UNUSED(dev)

    QList<QPair<QString, QString> > *pairs =
            static_cast<QList<QPair<QString, QString> > *>(d);
    if (!attr || !*attr)
        return 0;

    const int copyLen = value ? static_cast<int>(qMin<size_t>(len, 96)) : 0;
    const QString valueText = value ? QString::fromLocal8Bit(value, copyLen).trimmed()
                                    : QString();
    pairs->append(qMakePair(QString::fromLocal8Bit(attr), valueText));
    return 0;
}

// Paste-able board-console command that finds and reads one IIO register -
// the user runs this in picocom; the software never opens the UART itself.
QString consoleCheckCommand(const QString &attrName)
{
    return QStringLiteral(
                "for f in /sys/bus/iio/devices/iio:device*/%1 "
                "/sys/bus/iio/devices/iio:device*/debug/%1; "
                "do [ -f \"$f\" ] && { echo $f; cat $f; break; }; done")
            .arg(attrName);
}

} // namespace

DrfmRegisterIO::DrfmRegisterIO()
    : m_ledDev(nullptr)
    , m_legacyDev(nullptr)
    , m_ledDebugAttrsKnown(false)
{
}

const QList<DrfmRegisterDef> &DrfmRegisterIO::registerMap()
{
    static const QList<DrfmRegisterDef> map = {
        { 0x100u, QStringLiteral("TH0 / amplify"),
          QStringList() << QStringLiteral("th0") << QStringLiteral("amp")
                        << QStringLiteral("amplify") << QStringLiteral("amplifier"),
          false },
        { 0x104u, QStringLiteral("En_PATTERN"),
          QStringList() << QStringLiteral("en_pattern") << QStringLiteral("en0")
                        << QStringLiteral("enpattern") << QStringLiteral("pattern_en"),
          false },
        { 0x108u, QStringLiteral("TH1"),
          QStringList() << QStringLiteral("th1"), false },
        { 0x10Cu, QStringLiteral("TH2"),
          QStringList() << QStringLiteral("th2"), false },
        { 0x110u, QStringLiteral("PDW / doppler_enable"),
          QStringList() << QStringLiteral("pdw") << QStringLiteral("vgpo_enable")
                        << QStringLiteral("doppler_enable") << QStringLiteral("doppler_en"),
          false },
        { 0x114u, QStringLiteral("logouti"),
          QStringList() << QStringLiteral("logouti") << QStringLiteral("logout"),
          false },
        { 0x118u, QStringLiteral("INchann / phase offset"),
          QStringList() << QStringLiteral("inchannel") << QStringLiteral("inchann")
                        << QStringLiteral("in_chann") << QStringLiteral("phase_offset"),
          false },
        { 0x11Cu, QStringLiteral("thcw / phase step"),
          QStringList() << QStringLiteral("thcw") << QStringLiteral("th0cw")
                        << QStringLiteral("phase_step") << QStringLiteral("phasestep"),
          true },
        { 0x120u, QStringLiteral("th1cw"),
          QStringList() << QStringLiteral("th1cw"), true },
        { 0x124u, QStringLiteral("th2cw"),
          QStringList() << QStringLiteral("th2cw"), true },
        { 0x128u, QStringLiteral("NPS"),
          QStringList() << QStringLiteral("nps"), false },
        { 0x12Cu, QStringLiteral("P_to_B"),
          QStringList() << QStringLiteral("p_to_b") << QStringLiteral("ptob"),
          false },
        { 0x130u, QStringLiteral("P_to_S"),
          QStringList() << QStringLiteral("p_to_s") << QStringLiteral("ptos"),
          false },
        { 0x134u, QStringLiteral("pdiv"),
          QStringList() << QStringLiteral("pdiv"), false },
        { 0x138u, QStringLiteral("DACseles / dacsel"),
          QStringList() << QStringLiteral("dacsel") << QStringLiteral("dacseles")
                        << QStringLiteral("dacselect") << QStringLiteral("dacsele")
                        << QStringLiteral("dac_sel"),
          false },
        { 0x13Cu, QStringLiteral("CHsel"),
          QStringList() << QStringLiteral("chsel") << QStringLiteral("chselect")
                        << QStringLiteral("ch_sel"),
          false },
        { 0x140u, QStringLiteral("Teta_Az"),
          QStringList() << QStringLiteral("teta_az") << QStringLiteral("tetaaz"),
          false },
        { 0x144u, QStringLiteral("Num_Side"),
          QStringList() << QStringLiteral("num_side") << QStringLiteral("numside"),
          false },
        { 0x148u, QStringLiteral("Num_Back"),
          QStringList() << QStringLiteral("num_back") << QStringLiteral("numback"),
          false },
    };
    return map;
}

const DrfmRegisterDef *DrfmRegisterIO::defForOffset(quint32 offset)
{
    const QList<DrfmRegisterDef> &map = registerMap();
    for (int i = 0; i < map.size(); ++i)
        if (map.at(i).offset == offset)
            return &map.at(i);
    return nullptr;
}

QStringList DrfmRegisterIO::attrNamesForOffset(quint32 offset)
{
    const DrfmRegisterDef *def = defForOffset(offset);
    return def ? def->attrNames : QStringList();
}

QString DrfmRegisterIO::labelForOffset(quint32 offset)
{
    const DrfmRegisterDef *def = defForOffset(offset);
    return def ? def->label : QStringLiteral("register %1").arg(hexText(offset, 3));
}

QString DrfmRegisterIO::valueText(quint32 value, bool isSigned)
{
    if (isSigned)
        return QString::number(static_cast<qint32>(value));
    return QString::number(value);
}

struct iio_device *DrfmRegisterIO::ledCountDevice()
{
    if (m_ledDev || !globals::ctx)
        return m_ledDev;

    // The DTS binding is "led-count@43c30000" (compatible xlnx,led-count-ip).
    const char *exactNames[] = {
        "led-count", "led_count", "led-count-ip", "led_count_ip",
        "ledcount", "led-count-ip-1.0", nullptr
    };
    for (int i = 0; exactNames[i]; ++i)
    {
        m_ledDev = iio_context_find_device(globals::ctx, exactNames[i]);
        if (m_ledDev)
            return m_ledDev;
    }

    const unsigned int count = iio_context_get_devices_count(globals::ctx);
    for (unsigned int i = 0; i < count; ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (!dev)
            continue;

        const QString name = QString::fromLocal8Bit(iio_device_get_name(dev)
                                                    ? iio_device_get_name(dev) : "");
        const QString id = QString::fromLocal8Bit(iio_device_get_id(dev)
                                                  ? iio_device_get_id(dev) : "");
        const QString key = (name + QLatin1Char(' ') + id).toLower();

        if (key.contains(QStringLiteral("led_count")) ||
            key.contains(QStringLiteral("led-count")) ||
            key.contains(QStringLiteral("ledcount")))
        {
            m_ledDev = dev;
            return m_ledDev;
        }
    }

    // Last resort: the device that exposes the th0/dacsel debug attributes,
    // even when it was renamed.
    for (unsigned int i = 0; i < count; ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (!dev)
            continue;

        const QStringList names = debugAttrNames(dev);
        bool hasTh0 = false, hasDacsel = false;
        for (int n = 0; n < names.size(); ++n)
        {
            const QString key = normalizedAttrKey(names.at(n));
            if (key == QLatin1String("th0"))
                hasTh0 = true;
            if (key == QLatin1String("dacsel") || key == QLatin1String("dacseles"))
                hasDacsel = true;
        }
        if (hasTh0 && hasDacsel)
        {
            m_ledDev = dev;
            m_ledDebugAttrs = names;
            m_ledDebugAttrsKnown = true;
            return m_ledDev;
        }
    }

    return m_ledDev;
}

struct iio_device *DrfmRegisterIO::legacyBridgeDevice()
{
    if (m_legacyDev || !globals::ctx)
        return m_legacyDev;

    const char *exactNames[] = { "mwipcore0:mmwr0", "mmwr0", nullptr };
    for (int i = 0; exactNames[i]; ++i)
    {
        m_legacyDev = iio_context_find_device(globals::ctx, exactNames[i]);
        if (m_legacyDev)
            return m_legacyDev;
    }

    const unsigned int count = iio_context_get_devices_count(globals::ctx);
    for (unsigned int i = 0; i < count; ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (!dev)
            continue;
        const QString name = QString::fromLocal8Bit(iio_device_get_name(dev)
                                                    ? iio_device_get_name(dev) : "");
        const QString id = QString::fromLocal8Bit(iio_device_get_id(dev)
                                                  ? iio_device_get_id(dev) : "");
        const QString key = (name + QLatin1Char(' ') + id).toLower();
        if (key.contains(QStringLiteral("mmwr")) ||
            key.contains(QStringLiteral("memorywrite")) ||
            (key.contains(QStringLiteral("mwipcore")) &&
             iio_device_find_attr(dev, "reg_access")))
        {
            m_legacyDev = dev;
            return m_legacyDev;
        }
    }
    return m_legacyDev;
}

void DrfmRegisterIO::setLegacyBridgeEnabled(bool enable)
{
    struct iio_device *dev = legacyBridgeDevice();
    if (!dev || !iio_device_find_attr(dev, "reg_access"))
        return;
    iio_device_attr_write(dev, "reg_access", enable ? "enabled" : "disabled");
}

QStringList DrfmRegisterIO::debugAttrNames(struct iio_device *dev) const
{
    if (!dev)
        return QStringList();
    if (dev == m_ledDev && m_ledDebugAttrsKnown)
        return m_ledDebugAttrs;

    QStringList names;
    iio_device_debug_attr_read_all(dev, collectDebugAttrNames, &names);

    if (dev == m_ledDev)
    {
        m_ledDebugAttrs = names;
        m_ledDebugAttrsKnown = true;
    }
    return names;
}

QString DrfmRegisterIO::matchAttrName(const QStringList &available,
                                      const QStringList &wanted) const
{
    for (int w = 0; w < wanted.size(); ++w)
    {
        const QString wantKey = normalizedAttrKey(wanted.at(w));
        for (int a = 0; a < available.size(); ++a)
            if (normalizedAttrKey(available.at(a)) == wantKey)
                return available.at(a);
    }
    return QString();
}

bool DrfmRegisterIO::writeDebugAttr(struct iio_device *dev, const QString &name,
                                    quint32 value, bool isSigned, QString *how)
{
    if (!dev || name.isEmpty())
        return false;

    const QByteArray attrName = name.toLocal8Bit();
    const QByteArray text = valueText(value, isSigned).toLatin1();

    const ssize_t ret = iio_device_debug_attr_write(dev, attrName.constData(),
                                                    text.constData());
    if (ret >= 0)
    {
        if (how)
            *how = QStringLiteral("debug attr '%1' <= %2 (iio-oscilloscope Debug tab write)")
                    .arg(name, QString::fromLatin1(text));
        return true;
    }

    // Retry with a hex rendering ("raw data" style) and the longlong helper.
    const QByteArray hex = hexText(value, 8).toLatin1();
    const ssize_t retHex = iio_device_debug_attr_write(dev, attrName.constData(),
                                                       hex.constData());
    if (retHex >= 0)
    {
        if (how)
            *how = QStringLiteral("debug attr '%1' <= %2 (raw hex write)")
                    .arg(name, QString::fromLatin1(hex));
        return true;
    }

    const long long llValue = isSigned ? static_cast<long long>(static_cast<qint32>(value))
                                       : static_cast<long long>(value);
    const ssize_t retLl = iio_device_debug_attr_write_longlong(dev, attrName.constData(),
                                                               llValue);
    if (retLl >= 0)
    {
        if (how)
            *how = QStringLiteral("debug attr '%1' <= %2 (longlong write)")
                    .arg(name, QString::number(llValue));
        return true;
    }
    return false;
}

bool DrfmRegisterIO::writeChannelAttrs(struct iio_device *dev, const QStringList &wanted,
                                       quint32 value, bool isSigned, QString *how)
{
    if (!dev)
        return false;

    bool any = false;
    const unsigned int channelCount = iio_device_get_channels_count(dev);
    for (unsigned int c = 0; c < channelCount; ++c)
    {
        struct iio_channel *ch = iio_device_get_channel(dev, c);
        if (!ch)
            continue;

        const QString channelName = QString::fromLocal8Bit(iio_channel_get_id(ch)
                                                           ? iio_channel_get_id(ch) : "?")
                + (iio_channel_is_output(ch) ? QStringLiteral("_out") : QStringLiteral("_in"));

        for (int w = 0; w < wanted.size(); ++w)
        {
            const QByteArray attr = wanted.at(w).toLocal8Bit();
            if (!iio_channel_find_attr(ch, attr.constData()))
                continue;

            const long long llValue = isSigned
                    ? static_cast<long long>(static_cast<qint32>(value))
                    : static_cast<long long>(value);
            const ssize_t ret = iio_channel_attr_write_longlong(ch, attr.constData(), llValue);
            if (ret >= 0)
            {
                any = true;
                if (how)
                {
                    if (!how->isEmpty())
                        *how += QStringLiteral(" | ");
                    *how += QStringLiteral("channel attr %1/%2 <= %3")
                            .arg(channelName, wanted.at(w),
                                 QString::number(llValue));
                }
            }
        }
    }
    return any;
}

bool DrfmRegisterIO::writeRegister(quint32 offset, quint32 value,
                                   const QString &label, QString *details)
{
    QStringList pathLog;

    struct iio_device *ledDev = ledCountDevice();
    const DrfmRegisterDef *def = defForOffset(offset);
    const QStringList wanted = def ? def->attrNames : QStringList();
    const bool isSigned = def ? def->isSigned : false;
    const QString niceLabel = label.isEmpty() ? labelForOffset(offset) : label;

    bool written = false;
    QString how;
    QString debugAttrUsed;

    if (ledDev && !wanted.isEmpty())
    {
        // Path 1: named IIO debug attribute - identical to writing raw data
        // for th0 / dacsel / ... in the iio-oscilloscope Debug tab.
        const QString found = matchAttrName(debugAttrNames(ledDev), wanted);
        if (!found.isEmpty())
        {
            QString sub;
            if (writeDebugAttr(ledDev, found, value, isSigned, &sub))
            {
                written = true;
                debugAttrUsed = found;
                pathLog << QStringLiteral("IIO debug attribute: %1").arg(sub);
            }
            else
            {
                pathLog << QStringLiteral("IIO debug attribute '%1' rejected the write").arg(found);
            }
        }
        else
        {
            // Path 1b: attribute exists but is write-only (read_all skipped
            // it).  Trying to write the alias doubles as the existence test.
            QStringList variants = wanted;
            for (int w = 0; w < wanted.size(); ++w)
            {
                variants << wanted.at(w).toUpper();
                variants << wanted.at(w).toLower();
            }
            for (int v = 0; v < variants.size() && !written; ++v)
            {
                QString sub;
                if (writeDebugAttr(ledDev, variants.at(v), value, isSigned, &sub))
                {
                    written = true;
                    debugAttrUsed = variants.at(v);
                    pathLog << QStringLiteral("IIO debug attribute (probed): %1").arg(sub);
                }
            }
            if (!written)
                pathLog << QStringLiteral("no matching debug attribute among (%1)")
                           .arg(wanted.join(QLatin1String(", ")));
        }

        // Path 2: matching IIO channel attributes (e.g. "frequency" entries
        // that represent voltage0/1) - written "also", as requested.
        QString channelHow;
        if (writeChannelAttrs(ledDev, wanted, value, isSigned, &channelHow))
            pathLog << QStringLiteral("IIO channel attribute: %1").arg(channelHow);
    }

    // Path 3: direct register write to the IIO-visible led-count device at
    // its AXI offset (led_count_ip base 0x43C30000 + offset).
    if (!written)
    {
        if (!ledDev)
        {
            pathLog << QStringLiteral("led-count IIO device not found");
        }
        else
        {
            const int ret = iio_device_reg_write(ledDev, offset, value);
            if (ret == 0)
            {
                written = true;
                pathLog << QStringLiteral("IIO register write on '%1' offset %2 <= %3")
                           .arg(QString::fromLocal8Bit(iio_device_get_id(ledDev)
                                                       ? iio_device_get_id(ledDev) : "led-count"),
                                hexText(offset, 3), hexText(value, 8));
            }
            else
            {
                pathLog << QStringLiteral("IIO register write failed (%1)").arg(ret);
            }
        }
    }

    // Path 4: legacy MathWorks bridge (old bitstreams only).
    if (!written)
    {
        struct iio_device *legacy = legacyBridgeDevice();
        if (legacy)
        {
            setLegacyBridgeEnabled(true);
            const int ret = iio_device_reg_write(legacy, offset, value);
            setLegacyBridgeEnabled(false);
            if (ret == 0)
            {
                written = true;
                pathLog << QStringLiteral("legacy bridge 'mwipcore0:mmwr0' offset %1 <= %2")
                           .arg(hexText(offset, 3), hexText(value, 8));
            }
            else
            {
                pathLog << QStringLiteral("legacy bridge write failed (%1)").arg(ret);
            }
        }
        else
        {
            pathLog << QStringLiteral("no legacy mmwr bridge either");
        }
    }

    if (details)
    {
        *details = QStringLiteral("%1 %2 | AXI %3 <= %4 (%5) | %6")
                .arg(niceLabel,
                     written ? QStringLiteral("OK") : QStringLiteral("FAILED"),
                     hexText(0x43C30000u + offset, 8),
                     QString::number(value),
                     hexText(value, 8),
                     pathLog.join(QStringLiteral(" ; ")));

        // The confirmation command for the board console (picocom) - run it
        // by hand to cat the register and confirm the value really landed.
        const QString checkName = !debugAttrUsed.isEmpty() ? debugAttrUsed
                                                          : wanted.value(0);
        if (!checkName.isEmpty())
            *details += QStringLiteral("\n       console check (picocom): %1")
                    .arg(consoleCheckCommand(checkName));
    }

    if (written)
        qInfo() << "DrfmRegisterIO:" << (details ? *details : niceLabel);
    else
        qWarning() << "DrfmRegisterIO:" << (details ? *details : niceLabel);

    return written;
}

bool DrfmRegisterIO::writeChannelFrequency(double frequency, QString *details)
{
    QStringList pathLog;
    bool any = false;

    // Prefer the led-count device; fall back to any device exposing a
    // "frequency" attribute on voltage0 / voltage1.
    struct iio_device *order[2] = { ledCountDevice(), nullptr };
    QList<struct iio_device *> devices;
    if (order[0])
        devices << order[0];

    const unsigned int count = iio_context_get_devices_count(globals::ctx);
    for (unsigned int i = 0; i < count; ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (dev && !devices.contains(dev))
            devices << dev;
    }

    const char *channelNames[] = { "voltage0", "voltage1", nullptr };
    const char *attrNames[] = { "frequency", "freq", nullptr };

    for (int d = 0; d < devices.size(); ++d)
    {
        struct iio_device *dev = devices.at(d);
        const QString devName = QString::fromLocal8Bit(iio_device_get_id(dev)
                                                       ? iio_device_get_id(dev) : "?");
        for (int c = 0; channelNames[c]; ++c)
        {
            for (int out = 0; out < 2; ++out)
            {
                struct iio_channel *ch = iio_device_find_channel(dev, channelNames[c],
                                                                 out != 0);
                if (!ch)
                    continue;
                for (int a = 0; attrNames[a]; ++a)
                {
                    if (!iio_channel_find_attr(ch, attrNames[a]))
                        continue;
                    const ssize_t ret = iio_channel_attr_write_double(
                                ch, attrNames[a], frequency);
                    if (ret >= 0)
                    {
                        any = true;
                        pathLog << QStringLiteral("%1/%2%3_%4 <= %5 Hz")
                                   .arg(devName, QString::fromLatin1(channelNames[c]),
                                        out ? QStringLiteral("_out") : QStringLiteral("_in"),
                                        QString::fromLatin1(attrNames[a]),
                                        QString::number(frequency));
                    }
                }
            }
        }
        // voltage0/1 "frequency" only needs to be written once per found
        // device family; keep scanning but do not duplicate on identical dev.
    }

    if (details)
    {
        *details = any
                ? QStringLiteral("IIO frequency registers written: %1")
                  .arg(pathLog.join(QStringLiteral(" ; ")))
                : QStringLiteral("no 'frequency' channel attribute on voltage0/1 was found (%1)")
                  .arg(pathLog.join(QStringLiteral(" ; ")));
        *details += QStringLiteral("\n       console check (picocom): %1")
                .arg(consoleCheckCommand(QStringLiteral("in_voltage0_frequency")));
    }
    return any;
}

QString DrfmRegisterIO::scanReport()
{
    struct iio_device *ledDev = ledCountDevice();
    if (!ledDev)
        return QStringLiteral("led-count IIO device not found in the IIO context.");

    QList<QPair<QString, QString> > pairs;
    iio_device_debug_attr_read_all(ledDev, collectDebugAttrPairs, &pairs);

    QStringList mappedNames;
    QStringList lines;
    lines << QStringLiteral("led-count IIO debug attributes (= the th0/dacsel/... registers of");
    lines << QStringLiteral("the iio-oscilloscope Debug tab):");
    lines << QString();

    const QList<DrfmRegisterDef> &map = registerMap();
    for (int i = 0; i < map.size(); ++i)
    {
        const DrfmRegisterDef &def = map.at(i);
        const QString name = matchAttrName(debugAttrNames(ledDev), def.attrNames);
        QString value = QStringLiteral("<not exposed>");
        if (!name.isEmpty())
        {
            mappedNames << name;
            for (int p = 0; p < pairs.size(); ++p)
                if (QString::compare(pairs.at(p).first, name, Qt::CaseInsensitive) == 0)
                {
                    value = pairs.at(p).second;
                    break;
                }
            if (value == QStringLiteral("<not exposed>"))
                value = QStringLiteral("<write-only>");
        }
        lines << QStringLiteral("%1 (AXI 0x%2) = attr '%3' = %4")
                 .arg(def.label,
                      hexText(0x43C30000u + def.offset, 8),
                      name.isEmpty() ? QStringLiteral("-") : name,
                      value);
    }

    bool haveExtra = false;
    for (int p = 0; p < pairs.size(); ++p)
    {
        bool known = false;
        for (int m = 0; m < mappedNames.size(); ++m)
            if (QString::compare(pairs.at(p).first, mappedNames.at(m),
                                 Qt::CaseInsensitive) == 0)
            {
                known = true;
                break;
            }
        if (known)
            continue;
        if (!haveExtra)
        {
            lines << QString();
            lines << QStringLiteral("other debug attributes on the device:");
            haveExtra = true;
        }
        lines << QStringLiteral("  %1 = %2").arg(pairs.at(p).first, pairs.at(p).second);
    }

    return lines.join(QStringLiteral("\n"));
}

QString DrfmRegisterIO::backendInfo()
{
    struct iio_device *ledDev = ledCountDevice();
    struct iio_device *legacy = legacyBridgeDevice();

    QString ledName = QStringLiteral("<not found>");
    if (ledDev)
    {
        ledName = QString::fromLocal8Bit(iio_device_get_name(ledDev)
                                         ? iio_device_get_name(ledDev) : "");
        if (ledName.isEmpty())
            ledName = QString::fromLocal8Bit(iio_device_get_id(ledDev)
                                             ? iio_device_get_id(ledDev) : "led-count");
    }

    return QStringLiteral(
                "led_count_ip base 0x43C30000 | IIO led-count device: %1 | "
                "write path: named debug attrs (iio-osc Debug tab) -> voltage0/1 "
                "channel attrs -> led-count reg_write -> legacy %2")
            .arg(ledName,
                 legacy ? QStringLiteral("mwipcore0:mmwr0 (available)")
                        : QStringLiteral("mmwr bridge (not present)"));
}
