#include "globals.h"

#include <QProcess>

iio_context * globals::ctx=nullptr;
bool globals::status=false;
QList<w_info*> * globals::attrs=nullptr;
QList<plotData> * globals::PlotsData=nullptr;
iio_device * globals::dev=nullptr;
bool globals::hopping=false;
double globals::peakValue=0;
double globals::peakValueDb=0;
double globals::temp7291=0;
double globals::temp9009=0;

/**
 * @brief globals::globals
 */
globals::globals()
{
}

/**
 * @brief globals::osc_destroy_context
 */
void globals::osc_destroy_context()
{
    iio_context_destroy(ctx);
}

/**
 * @brief globals::set_widget_value
 * @param widget
 * @param item
 * @param val
 * @return
 */
char * globals::set_widget_value(QWidget *widget, struct w_info *item, long long val)
{
    QString className=widget->metaObject()->className();

    className=className.toLower();

    if(className.toLower()=="qcheckbox")
    {
        ((QCheckBox*)widget)->setChecked(!!val);

        QObject::connect(((QCheckBox*)widget),&QCheckBox::clicked,[=](){
            QCheckBox *chk=(QCheckBox *)widget;
            iio_device_debug_attr_write_longlong(globals::dev, item->name, chk->isChecked()?1:0);
        });

    }
    else if(className=="qcombobox"){
        for (int i = 0; i < item->lut_len; i++)
            if (val == item->lut[i])
            {
                ((QComboBox*)widget)->setCurrentIndex(i);
                break;
            }

        ((QComboBox*)widget)->setProperty("tag",item->name);

        QObject::connect(&static_cast<QComboBox&>(*widget),
                         static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
                         ,[=](int index){

            QComboBox* combo=(QComboBox*)widget;

            QString attr=combo->property("tag").toString();
            for (int i = 0; i < attrs->size(); i++) {

                if (attrs->at(i)->name==attr) {

                    index = attrs->at(i)->lut[index];

                    iio_device_debug_attr_write_longlong(dev, attrs->at(i)->name, index);

                    break;
                }
            }

        });
    }
    else if(className=="qspinbox"){
        ((QSpinBox*)widget)->setValue(val);

        QObject::connect(&static_cast<QSpinBox&>(*widget),
                         static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)
                         ,[=](int value){
            iio_device_debug_attr_write_longlong(dev, item->name, value);
        });

    }
    else if(className=="qdoublespinbox"){
        ((QDoubleSpinBox*)widget)->setValue(val);

        QObject::connect(&static_cast<QDoubleSpinBox&>(*widget),
                         static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged)
                         ,[=](double value){
            iio_device_debug_attr_write_longlong(dev, item->name, value);
        });
    }


    return NULL;
}

/**
 * @brief globals::connect_widget
 * @param widget
 * @param item
 * @param val
 */
void globals::connect_widget(QWidget *widget, struct w_info *item, long long val)
{
    set_widget_value(widget, item, val);
}

/**
 * @brief globals::__connect_widget
 * @param dev
 * @param attr
 * @param value
 * @param len
 * @param d
 * @return
 */
int globals::__connect_widget(struct iio_device *dev, const char *attr,
                              const char *value, size_t len, void *d)
{
    unsigned int i, nb_items = attrs ? attrs->size() : 0;
    char str[80];
    int bit, ret;

    for (i = 0; i < nb_items; i++) {
        if (!attrs->at(i) || !attrs->at(i)->name || !attr)
            continue;
        // Width limit: an unbounded scanset writes past str[80] and aborts
        // with "stack smashing detected" when an attribute name is long.
        str[0] = '\0';
        ret = sscanf(attrs->at(i)->name, "%79[^'#']#%d", str, &bit);
        if (ret < 1)
            continue;
        if (!strcmp(str, attr)) {
            connect_widget(attrs->at(i)->widget, attrs->at(i), atoll(value));

            if (ret == 1 && attrs->at(i)->lut_len == 0) {
                return 0;
            }
        }
    }

    return 0;
}

/**
 * @brief globals::connect_widgets
 * @param builder
 * @return
 */
int globals::connect_widgets(QWidget *builder)
{
    return iio_device_debug_attr_read_all(dev, __connect_widget, builder);
}

/**
 * @brief globals::boardSerialNumber
 * Read the board serial number over LAN: preferred source is the iiod
 * context attribute "hw_serial", otherwise read the production EEPROM
 * (/sys/bus/i2c/devices/0-0050/eeprom) with a key-authenticated ssh exec
 * (BatchMode: never prompts for a password).
 */
QString globals::boardSerialNumber()
{
    QString sn;

    if (globals::ctx) {
        const char *v = iio_context_get_attr_value(globals::ctx, "hw_serial");
        if (v && *v)
            sn = QString::fromUtf8(v).trimmed();
    }

    if (sn.isEmpty() && globals::ctx) {
        QString host;
        const char *uri = iio_context_get_attr_value(globals::ctx, "uri");
        if (uri)
            host = QString::fromUtf8(uri);
        host.remove(QLatin1String("ip:"));
        host = host.section(QLatin1Char(':'), 0, 0);

        if (!host.isEmpty()) {
            QProcess proc;
            proc.start(QStringLiteral("ssh"),
                       QStringList() << QStringLiteral("-o") << QStringLiteral("BatchMode=yes")
                       << QStringLiteral("-o") << QStringLiteral("ConnectTimeout=2")
                       << (QStringLiteral("root@") + host)
                       << QStringLiteral("head -c 16 /sys/bus/i2c/devices/0-0050/eeprom"));
            if (proc.waitForFinished(3500) && proc.exitCode() == 0) {
                const QByteArray raw = proc.readAllStandardOutput();
                for (unsigned char c : raw) {
                    if (c < 0x20 || c > 0x7e)
                        break;
                    sn.append(QChar(c));
                }
                sn = sn.trimmed();
            }
        }
    }

    return sn;
}

bool globals::serialIsSn001(const QString &sn)
{
    const QString snUp = sn.trimmed().toUpper();
    return snUp.endsWith(QLatin1String("SN001")) ||
           snUp.endsWith(QLatin1String("001"));
}

bool globals::serialIsSn003(const QString &sn)
{
    const QString snUp = sn.trimmed().toUpper();
    return snUp.endsWith(QLatin1String("SN003")) ||
           snUp.endsWith(QLatin1String("003"));
}
