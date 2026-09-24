#include "drfmregisterio.h"

#include <receiver/globals.h>

#include <QDebug>
#include <QRegularExpression>

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
// callback typedef this project already compiles against.
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

// Paste-able board-console command that finds and reads one IIO sysfs file -
// the user runs this in picocom; the software never opens the UART itself.
QString consoleCheckCommand(const QString &sysfsName)
{
    return QStringLiteral(
                "for f in /sys/bus/iio/devices/iio:device*/%1 "
                "/sys/bus/iio/devices/iio:device*/debug/%1; "
                "do [ -f \"$f\" ] && { echo $f; cat $f; break; }; done")
            .arg(sysfsName);
}

// led-count-iio names its channels "count1_th0", "count15_dacseles", ... -
// strip the "countN" index so the register name remains.
QString stripCountPrefix(const QString &identifier)
{
    static const QRegularExpression countPrefix(
                QStringLiteral("^count[0-9]+_?"),
                QRegularExpression::CaseInsensitiveOption);
    QString stripped = identifier;
    stripped.remove(countPrefix);
    return stripped.trimmed();
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
          QStringList() << QStringLiteral("inchann") << QStringLiteral("inchannel")
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
          QStringList() << QStringLiteral("dacseles") << QStringLiteral("dacsel")
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

    // The DTS binding is "led-count@43c30000"; the board driver registers
    // the IIO device as "led-count-iio".
    const char *exactNames[] = {
        "led-count-iio", "led_count_iio", "led-count", "led_count",
        "led-count-ip", "led_count_ip", "ledcount", nullptr
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

    // Last resort: the device whose channels carry the th0/dacseles labels,
    // even when it was renamed.
    for (unsigned int i = 0; i < count; ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (!dev)
            continue;

        ChannelRawMatch m;
        const bool hasTh0 = findChannelRaw(dev, QStringList() << QStringLiteral("th0"), &m);
        const bool hasDacsel = findChannelRaw(dev,
                                              QStringList() << QStringLiteral("dacseles")
                                                            << QStringLiteral("dacsel"),
                                              &m);
        if (hasTh0 && hasDacsel)
        {
            m_ledDev = dev;
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

QStringList DrfmRegisterIO::channelNameCandidates(struct iio_channel *ch)
{
    QStringList candidates;
    if (!ch)
        return candidates;

    // The driver labels each channel with its register name
    // (out_count1_th0_label = "th0").
    if (iio_channel_find_attr(ch, "label"))
    {
        char buf[128] = {};
        const ssize_t n = iio_channel_attr_read(ch, "label", buf, sizeof(buf) - 1);
        if (n > 0)
            candidates << QString::fromLocal8Bit(buf).trimmed();
    }

    const QString id = iio_channel_get_id(ch)
            ? QString::fromLocal8Bit(iio_channel_get_id(ch)) : QString();
    const QString name = iio_channel_get_name(ch)
            ? QString::fromLocal8Bit(iio_channel_get_name(ch)) : QString();

    const QStringList bases = QStringList() << id << name;
    for (int i = 0; i < bases.size(); ++i)
    {
        if (!bases.at(i).isEmpty())
            candidates << bases.at(i);
        const QString stripped = stripCountPrefix(bases.at(i));
        if (!stripped.isEmpty())
            candidates << stripped;
    }
    return candidates;
}

bool DrfmRegisterIO::findChannelRaw(struct iio_device *dev, const QStringList &wanted,
                                    ChannelRawMatch *match) const
{
    if (!dev || !match || wanted.isEmpty())
        return false;

    const unsigned int channelCount = iio_device_get_channels_count(dev);
    for (unsigned int c = 0; c < channelCount; ++c)
    {
        struct iio_channel *ch = iio_device_get_channel(dev, c);
        if (!ch)
            continue;

        const QString channelDesc =
                QStringLiteral("channel id '%1'%2")
                .arg(iio_channel_get_id(ch) ? iio_channel_get_id(ch) : "?",
                     iio_channel_is_output(ch) ? QStringLiteral(" (output)")
                                               : QStringLiteral(" (input)"));

        // 1) channel identified by its name / label as one of the registers
        const QStringList candidates = channelNameCandidates(ch);
        QString matchedAlias;
        for (int w = 0; w < wanted.size() && matchedAlias.isEmpty(); ++w)
        {
            const QString wantKey = normalizedAttrKey(wanted.at(w));
            for (int a = 0; a < candidates.size(); ++a)
                if (normalizedAttrKey(candidates.at(a)) == wantKey)
                {
                    matchedAlias = wanted.at(w);
                    break;
                }
        }

        if (!matchedAlias.isEmpty())
        {
            if (iio_channel_find_attr(ch, "raw"))
            {
                match->channel = ch;
                match->attrName = QStringLiteral("raw");
                const char *fn = iio_channel_attr_get_filename(ch, "raw");
                match->sysfsName = fn ? QString::fromLocal8Bit(fn) : QString();
                match->channelDesc = channelDesc;
                return true;
            }
        }

        // 2) alternate layout: attribute named "<register>_raw" on any
        //    channel (e.g. channel "count1" with attribute "th0_raw")
        for (int w = 0; w < wanted.size(); ++w)
        {
            const QByteArray fname = (wanted.at(w) + QLatin1String("_raw")).toLatin1();
            if (!iio_channel_find_attr(ch, fname.constData()))
                continue;
            match->channel = ch;
            match->attrName = QString::fromLatin1(fname);
            const char *fn = iio_channel_attr_get_filename(ch, fname.constData());
            match->sysfsName = fn ? QString::fromLocal8Bit(fn) : QString();
            match->channelDesc = channelDesc;
            return true;
        }
    }
    return false;
}

bool DrfmRegisterIO::writeChannelRaw(struct iio_device *dev, const QStringList &wanted,
                                     quint32 value, bool isSigned, QString *how,
                                     ChannelRawMatch *usedMatch)
{
    ChannelRawMatch match;
    if (!findChannelRaw(dev, wanted, &match))
        return false;

    const long long llValue = isSigned ? static_cast<long long>(static_cast<qint32>(value))
                                       : static_cast<long long>(value);

    const QByteArray attrName = match.attrName.toLocal8Bit();
    ssize_t ret = iio_channel_attr_write_longlong(match.channel, attrName.constData(),
                                                  llValue);
    if (ret < 0)
    {
        const QByteArray text = valueText(value, isSigned).toLatin1();
        ret = iio_channel_attr_write(match.channel, attrName.constData(),
                                     text.constData());
    }
    if (ret < 0)
        return false;

    if (usedMatch)
        *usedMatch = match;

    if (how)
    {
        const QString fileName = match.sysfsName.isEmpty() ? match.attrName
                                                           : match.sysfsName;
        *how = QStringLiteral(
                    "IIO channel raw write: %1 [%2] <= %3 (iio-oscilloscope Debug tab entry)")
                .arg(fileName, match.channelDesc, valueText(value, isSigned));
    }
    return true;
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
    QString checkFileName;

    // Path 1: IIO channel "raw" attribute of the led-count device - the
    // out_countN_<name>_raw entries you write raw data for in the
    // iio-oscilloscope Debug tab (out_count1_th0_raw, out_count15_dacseles_raw, ...).
    if (ledDev && !wanted.isEmpty())
    {
        QString how;
        ChannelRawMatch usedMatch;
        if (writeChannelRaw(ledDev, wanted, value, isSigned, &how, &usedMatch))
        {
            written = true;
            checkFileName = usedMatch.sysfsName.isEmpty() ? usedMatch.attrName
                                                          : usedMatch.sysfsName;
            pathLog << how;
        }
        else
        {
            pathLog << QStringLiteral("no channel raw attribute matched (%1)")
                       .arg(wanted.join(QLatin1String(", ")));
        }

        // Path 2: named IIO debug attribute (other drivers / bitstreams).
        if (!written)
        {
            const QString found = matchAttrName(debugAttrNames(ledDev), wanted);
            if (!found.isEmpty())
            {
                QString sub;
                if (writeDebugAttr(ledDev, found, value, isSigned, &sub))
                {
                    written = true;
                    checkFileName = found;
                    pathLog << QStringLiteral("IIO debug attribute: %1").arg(sub);
                }
                else
                {
                    pathLog << QStringLiteral("IIO debug attribute '%1' rejected the write").arg(found);
                }
            }
            else
            {
                pathLog << QStringLiteral("no matching debug attribute either");
            }
        }
    }
    else
    {
        pathLog << QStringLiteral("led-count IIO device not found");
    }

    // Path 3: direct register write to the IIO-visible led-count device at
    // its AXI offset (led_count_ip base 0x43C30000 + offset).
    if (!written && ledDev)
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
        const QString checkName = checkFileName.isEmpty() ? wanted.value(0)
                                                          : checkFileName;
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

QString DrfmRegisterIO::scanReport()
{
    struct iio_device *ledDev = ledCountDevice();
    if (!ledDev)
        return QStringLiteral("led-count IIO device not found in the IIO context.");

    QStringList lines;
    const QString devId = iio_device_get_id(ledDev)
            ? QString::fromLocal8Bit(iio_device_get_id(ledDev)) : QStringLiteral("iio:device?");
    lines << QStringLiteral("led-count IIO register map (device '%1', %2):")
             .arg(QString::fromLocal8Bit(iio_device_get_name(ledDev)
                                         ? iio_device_get_name(ledDev) : "?"), devId);
    lines << QStringLiteral("(these are the raw-data entries of the iio-oscilloscope Debug tab)");
    lines << QString();

    const QList<DrfmRegisterDef> &map = registerMap();
    for (int i = 0; i < map.size(); ++i)
    {
        const DrfmRegisterDef &def = map.at(i);
        ChannelRawMatch match;
        if (findChannelRaw(ledDev, def.attrNames, &match))
        {
            QString value = QStringLiteral("<unreadable>");
            if (match.channel)
            {
                char buf[128] = {};
                const QByteArray attr = match.attrName.toLocal8Bit();
                const ssize_t n = iio_channel_attr_read(match.channel, attr.constData(),
                                                        buf, sizeof(buf) - 1);
                if (n > 0)
                    value = QString::fromLocal8Bit(buf).trimmed();
            }
            lines << QStringLiteral("%1 (AXI 0x%2) = %3 = %4   [%5]")
                     .arg(def.label,
                          hexText(0x43C30000u + def.offset, 8),
                          match.sysfsName.isEmpty() ? match.attrName : match.sysfsName,
                          value,
                          match.channelDesc);
        }
        else
        {
            lines << QStringLiteral("%1 (AXI 0x%2) = <no matching IIO channel attribute>")
                     .arg(def.label, hexText(0x43C30000u + def.offset, 8));
        }
    }

    // Raw channel inventory so any unmapped register is visible as well.
    lines << QString();
    lines << QStringLiteral("IIO channel inventory:");
    const unsigned int channelCount = iio_device_get_channels_count(ledDev);
    for (unsigned int c = 0; c < channelCount; ++c)
    {
        struct iio_channel *ch = iio_device_get_channel(ledDev, c);
        if (!ch)
            continue;

        QString label;
        if (iio_channel_find_attr(ch, "label"))
        {
            char buf[128] = {};
            if (iio_channel_attr_read(ch, "label", buf, sizeof(buf) - 1) > 0)
                label = QString::fromLocal8Bit(buf).trimmed();
        }

        QStringList files;
        const char *probeAttrs[] = { "raw", "label", nullptr };
        for (int a = 0; probeAttrs[a]; ++a)
            if (iio_channel_find_attr(ch, probeAttrs[a]))
            {
                const char *fn = iio_channel_attr_get_filename(ch, probeAttrs[a]);
                if (fn)
                    files << QString::fromLocal8Bit(fn);
            }

        lines << QStringLiteral("  %1 id='%2' label='%3' files: %4")
                 .arg(iio_channel_is_output(ch) ? QStringLiteral("out") : QStringLiteral("in"),
                      iio_channel_get_id(ch) ? iio_channel_get_id(ch) : "?",
                      label,
                      files.isEmpty() ? QStringLiteral("-") : files.join(QLatin1String(", ")));
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
                "write path: IIO channel raw attrs (out_countN_<name>_raw, "
                "iio-osc Debug tab) -> debug attrs -> led-count reg_write -> legacy %2")
            .arg(ledName,
                 legacy ? QStringLiteral("mwipcore0:mmwr0 (available)")
                        : QStringLiteral("mmwr bridge (not present)"));
}
