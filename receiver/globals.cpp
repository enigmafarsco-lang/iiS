#include "globals.h"

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
    unsigned int i, nb_items = attrs->size();
    char str[80];
    int bit, ret;

    for (i = 0; i < nb_items; i++) {
        ret = sscanf(attrs->at(i)->name, "%[^'#']#%d", str, &bit);
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
