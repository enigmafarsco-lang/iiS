#include "iio_widget.h"

#include <QTimer>

IIO_Widget::IIO_Widget()
{
}

void iio_combo_box_update_value(struct iio_widget *widget,
                                const char *src, size_t len)
{
    int (*compare)(const char *, const char *);
    QComboBox *combo_box;
    QString *iter;
    QStringList *model=new QStringList();
    char text2[1024], *item;
    gchar **items_avail = NULL, **saveditems_avail;
    gboolean has_iter;
    ssize_t ret;

    combo_box = (QComboBox*)widget->widget;

    if(!combo_box)
        return;

    combo_box->clear();

    if (widget->attr_name_avail) {
        if (widget->chn)
            ret = iio_channel_attr_read(widget->chn,
                                        widget->attr_name_avail, text2, sizeof(text2) - 1);
        else
            ret = iio_device_attr_read(widget->dev,
                                       widget->attr_name_avail, text2, sizeof(text2) - 1);
        if (ret >= 0)
            text2[ret < (ssize_t)sizeof(text2) ? ret : (ssize_t)sizeof(text2) - 1] = '\0';
        if (ret < 0)
            return;

        /* may use Q_combo_box_text_remove_all Q3 only */
        //        Q_list_store_clear (Q_LIST_STORE (model));
        saveditems_avail = items_avail = g_strsplit (text2, " ", 0);

        for (; NULL != *items_avail; items_avail++) {
            if (*items_avail[0] == '\0')
                continue;
            combo_box->addItem(*items_avail);

        }

        if (saveditems_avail)
            g_strfreev(saveditems_avail);
    }

    for(int i=0;i<combo_box->children().size();i++)
    {
        model->append(combo_box->itemText(i));
    }

    //        if (widget->priv)
    //            compare = static_cast<int*>(widget->priv);
    //        else

    compare = strcmp;

    for(int i=0;i<model->size();i++) {
        if (compare (src, (model->at(i)).toLocal8Bit().data()) == 0) {
            combo_box->setCurrentIndex(i);
            break;
        }
    }

}

void iio_combo_box_update(struct iio_widget *widget)
{
    ssize_t len;
    char text[1024];

    if (widget->chn)
        len = iio_channel_attr_read(widget->chn,
                                    widget->attr_name, text, sizeof(text) - 1);
    else
        len = iio_device_attr_read(widget->dev,
                                   widget->attr_name, text, sizeof(text) - 1);
    if (len >= 0)
        text[len < (ssize_t)sizeof(text) ? len : (ssize_t)sizeof(text) - 1] = '\0';
    if (len > 0)
        iio_combo_box_update_value(widget, text, len);
}

void iio_combo_box_save(struct iio_widget *widget)
{
    // toLocal8Bit().data() dangles as soon as the temporary QByteArray is
    // destroyed. Hold the bytes until iio_*_attr_write returns.
    const QByteArray text = static_cast<QComboBox*>(widget->widget)->currentText().toLocal8Bit();
    if (text.isNull())
        return;

    if (widget->chn)
        iio_channel_attr_write(widget->chn, widget->attr_name, text.constData());
    else
        iio_device_attr_write(widget->dev, widget->attr_name, text.constData());
}

void iio_widget_init(struct iio_widget *widget,
                     struct iio_device *dev, struct iio_channel *chn, const char *attr_name,
                     const char *attr_name_avail, QWidget *Q_widget, void *priv,
                     void (*update)(struct iio_widget *),
                     void (*update_value)(struct iio_widget *, const char *, size_t),
                     void (*save)(struct iio_widget *))
{
    if (!Q_widget) {
        const char *name = iio_device_get_name(dev) ?:
                               iio_device_get_id(dev);
        printf("Missing widget for %s/%s\n", name, attr_name);
    }

    memset(widget, 0, sizeof(*widget));

    widget->dev = dev;
    widget->chn = chn;
    widget->attr_name = attr_name;
    widget->attr_name_avail = attr_name_avail;
    widget->widget = Q_widget;
    widget->update = update;
    widget->update_value = update_value;
    widget->save = save;
    widget->priv = priv;
}

void IIO_Widget::iio_widget_update(struct iio_widget *widget){
    widget->update(widget);
}


/*
 * Set a user function to be called when the progress is completed. The function
 * can take one generic pointer as parameter.
 */
void IIO_Widget::iio_spin_button_set_on_complete_function(struct iio_widget *iio_w,
                                                          void(*on_complete)(void *), void *data)
{
    struct progress_data *pdata =(struct progress_data *) iio_w->priv_progress;

    QString className=(iio_w->widget)->metaObject()->className();
    if (className !="QDoubleSpinBox" && className !="QSpinBox" ) {

        const char *name = iio_device_get_name(iio_w->dev) ?:
                               iio_device_get_id(iio_w->dev);
        printf("The widget connected to the attribute: %s of device: %s is not a GtkSpinButton\n",
               iio_w->attr_name, name);
        return;
    }

    pdata->on_complete = on_complete;
    pdata->on_complete_data = data;
}


void IIO_Widget::iio_combo_box_init(struct iio_widget *widget, struct iio_device *dev,
                                    struct iio_channel *chn, const char *attr_name, const char *attr_name_avail,
                                    QWidget *combo_box, int (*compare)(const char *a, const char *b))
{

    iio_widget_init(widget, dev, chn, attr_name, attr_name_avail, combo_box,
                    (void *)compare, iio_combo_box_update,
                    iio_combo_box_update_value, iio_combo_box_save);

    if(!(QComboBox*)widget->widget)
        return;

    ((QComboBox*)widget->widget)->itemText(0);

}

static void iio_toggle_button_save(struct iio_widget *widget)
{
    bool active = ((QAbstractButton*)(widget->widget))->isChecked();
    active = widget->priv ? !active : active;

    if (widget->chn)
        iio_channel_attr_write_bool(widget->chn,
                                    widget->attr_name, active);
    else
        iio_device_attr_write_bool(widget->dev,
                                   widget->attr_name, active);
}

static void iio_toggle_button_update_value(struct iio_widget *widget,
                                           const char *src, size_t len)
{
    bool active;

    if (len != 2)
        return;

    active = src[0] == '1' || src[0] == 'Y';
    active = widget->priv ? !active : active;
    ((QAbstractButton*)(widget->widget))->setChecked(active);
}

static void iio_toggle_button_update(struct iio_widget *widget)
{
    char buf[0x100];
    ssize_t ret;

    if (widget->chn)
        ret = iio_channel_attr_read(widget->chn,
                                    widget->attr_name, buf, sizeof(buf) - 1);
    else
        ret = iio_device_attr_read(widget->dev,
                                   widget->attr_name, buf, sizeof(buf) - 1);
    if (ret >= 0)
        buf[ret < (ssize_t)sizeof(buf) ? ret : (ssize_t)sizeof(buf) - 1] = '\0';
    if (ret > 0)
        iio_toggle_button_update_value(widget, buf, ret);
    else if (ret == -ENODEV)
        ((QWidget*)(widget->widget))->setVisible(false);
}

void IIO_Widget::iio_toggle_button_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, QWidget *toggle_button, const bool invert)
{
    iio_widget_init(widget, dev, chn, attr_name, NULL, toggle_button,
                    (void *)invert, iio_toggle_button_update,
                    iio_toggle_button_update_value, iio_toggle_button_save);

    iio_toggle_button_update(widget);

}

void iio_button_save(struct iio_widget *widget)
{
    if (widget->chn)
        iio_channel_attr_write_bool(widget->chn,
                                    widget->attr_name, 1);
    else
        iio_device_attr_write_bool(widget->dev,
                                   widget->attr_name, 1);
}

void iio_button_update_value(struct iio_widget *widget,
                             const char *src, size_t len)
{

}

void iio_button_update(struct iio_widget *widget)
{

}

void IIO_Widget::iio_button_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, QWidget *button)
{
    iio_widget_init(widget, dev, chn, attr_name, NULL, button,
                    NULL, iio_button_update,
                    iio_button_update_value, iio_button_save);
}

void iio_spin_button_update_value(struct iio_widget *widget,
                                  const char *src, size_t len)
{
    gdouble freq, mag, min, max;
    gdouble scale = widget->priv ? *(gdouble *)widget->priv : 1.0;
    char *end;

    mag = ((QDoubleSpinBox*)(widget->widget))->value();

    min=((QDoubleSpinBox*)(widget->widget))->minimum();
    max=((QDoubleSpinBox*)(widget->widget))->maximum();

    freq = g_ascii_strtod(src, &end);
    if (end == src)
        return;

    if (widget->priv_convert_function)
        freq = ((double (*)(double, bool))widget->priv_convert_function)(freq, true);

    freq /= fabs(scale);

    /* if scale is negative, we treat things a little differently */
    if (scale < 0) {
        /* if the setting is negative, and it can be set negative */
        if (mag < 0 && min < 0)
            freq *= -1;
        else if (min >= 0)
            freq *= -1;
    }

    ((QDoubleSpinBox*)(widget->widget))->setValue(freq);
}

void iio_spin_button_update(struct iio_widget *widget)
{
    ssize_t ret;
    char buf[0x100];

    if (widget->chn)
        ret = iio_channel_attr_read(widget->chn,
                                    widget->attr_name, buf, sizeof(buf));
    else
        ret = iio_device_attr_read(widget->dev,
                                   widget->attr_name, buf, sizeof(buf));
    if (ret > 0)
        iio_spin_button_update_value(widget, buf, ret);
    else if (ret == -ENODEV)
        widget->widget->hide();
}

void spin_button_save(struct iio_widget *widget, bool is_double)
{
    gdouble freq, min;
    gdouble scale = widget->priv ? *(gdouble *)widget->priv : 1.0;

    freq = ((QDoubleSpinBox*)(widget->widget))->value();
    min = ((QDoubleSpinBox*)(widget->widget))->minimum();

    if (scale < 0 && min < 0)
        freq = fabs(freq * scale);
    else
        freq *= scale;

    if (widget->priv_convert_function)
        freq = ((double (*)(double, bool))widget->priv_convert_function)(freq, false);

    if (widget->chn) {
        if (is_double)
            iio_channel_attr_write_double(widget->chn,
                                          widget->attr_name, freq);
        else
            iio_channel_attr_write_longlong(widget->chn,
                                            widget->attr_name, (long long) freq);
    } else {
        if (is_double)
            iio_device_attr_write_double(widget->dev,
                                         widget->attr_name, freq);
        else
            iio_device_attr_write_longlong(widget->dev,
                                           widget->attr_name, (long long) freq);
    }
}

void iio_spin_button_savedbl(struct iio_widget *widget)
{
    return spin_button_save(widget, true);
}

void iio_spin_button_save(struct iio_widget *widget)
{
    return spin_button_save(widget, false);
}

void IIO_Widget::iio_spin_button_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, QWidget *spin_button, const gdouble *scale)
{
    iio_widget_init(widget, dev, chn, attr_name, NULL, spin_button,
                    (void *)scale, iio_spin_button_update,
                    iio_spin_button_update_value, iio_spin_button_savedbl);

//    iio_spin_button_update(widget);
}


/*
 * Allow user to disable the function that saves the value of the widget
 * to the driver when progress bar reaches 100%. User may use
 * iio_spin_button_set_on_complete_function() in order to provide a
 * more complex logic before saving the value of the widget.
 */

void IIO_Widget::iio_spin_button_skip_save_on_complete(struct iio_widget *iio_w,
                                                       gboolean skip)
{
    struct progress_data *pdata =(struct progress_data *) iio_w->priv_progress;

    pdata->skip_widget_save = skip;
}


void IIO_Widget::iio_spin_button_set_convert_function(struct iio_widget *iio_w,
                                                      double (*convert)(double, bool))
{
    iio_w->priv_convert_function = (void*)convert;
}

void IIO_Widget::iio_spin_button_add_progress(iio_widget *iio_w)
{
    struct progress_data *pdata;

    if (!iio_w->widget)
        return;

    QString className=(iio_w->widget)->metaObject()->className();
    if (className !="QDoubleSpinBox" && className !="QSpinBox" ) {
        const char *name = iio_device_get_name(iio_w->dev) ?:
                               iio_device_get_id(iio_w->dev);
        printf("The widget connected to the attribute: %s of device: %s is not a QDoubleSpinBox\n",
               iio_w->attr_name, name);
        return;
    }

    pdata =(progress_data *) malloc(sizeof(struct progress_data));
    pdata->is_progress_spin_button = TRUE;
    pdata->progress = 0.0;
    pdata->timeoutID = -1;
    pdata->value_changed_hid = -1;
    pdata->skip_widget_save = FALSE;
    pdata->on_complete_data = NULL;
    pdata->on_complete = NULL;
    iio_w->priv_progress = pdata;
}

void IIO_Widget::iio_spin_button_s64_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, QWidget *spin_button, const gdouble *scale)
{
    iio_widget_init(widget, dev, chn, attr_name, NULL, spin_button,
                    (void *)scale, iio_spin_button_update,
                    iio_spin_button_update_value, iio_spin_button_save);
}

void IIO_Widget::iio_spin_button_int_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, QWidget *spin_button, const gdouble *scale)
{
    iio_widget_init(widget, dev, chn, attr_name, NULL, spin_button,
                    (void *)scale, iio_spin_button_update,
                    iio_spin_button_update_value, iio_spin_button_save);
}


static int __cb_dev_update(struct iio_device *dev, const char *attr,
                           const char *value, size_t len, void *d)
{
    unsigned int i;
    struct update_widgets_params *params = (update_widgets_params *)d;

    for (i = 0; i < params->nb; i++) {
        struct iio_widget *widget = &params->widgets[i];
        if (widget->update_value && !widget->chn &&
            widget->dev == dev &&
            !strcmp(widget->attr_name, attr)) {
            widget->update_value(widget, value, len);
            return 0;
        }
    }

    return 0;
}

static int __cb_chn_update(struct iio_channel *chn, const char *attr,
                           const char *value, size_t len, void *d)
{
    unsigned int i;
    struct update_widgets_params *params = (update_widgets_params *)d;

    for (i = 0; i < params->nb; i++) {
        struct iio_widget *widget = &params->widgets[i];
        if (widget->update_value
            && widget->chn == chn
            && !strcmp(widget->attr_name, attr)) {
            widget->update_value(widget, value, len);
            return 0;
        }
    }

    return 0;
}

void IIO_Widget::iio_update_widgets_of_device(iio_widget *widgets, unsigned int num_widgets, iio_device *dev)
{

    unsigned int i;

    update_widgets_params *params=new update_widgets_params();
    params->widgets = widgets;
    params->nb = num_widgets;


    iio_device_attr_read_all(dev, __cb_dev_update, params);

    for (i = 0; i < iio_device_get_channels_count(dev); i++)
        iio_channel_attr_read_all(iio_device_get_channel(dev, i),
                                  __cb_chn_update, params);
}

void IIO_Widget::iio_update_widgets(iio_widget *widgets, unsigned int num_widgets)
{
    unsigned int i;

    for (i = 0; i < num_widgets; i++)
        iio_widget_update(&widgets[i]);
}


void iio_widget_save(struct iio_widget *widget)
{
    widget->save(widget);
    widget->update(widget);
}

/*
 * Gets called periodically to increase the progress with one step.
 * When progress is complete saves the spinbutton value to file, clears the
 * progress and stops the function to be called periodically.
 */
static gboolean spin_button_progress_step(struct iio_widget *iio_w)
{
    struct progress_data *pdata = (struct progress_data *)iio_w->priv_progress;
    void (*on_complete_cb)(void *) = pdata->on_complete;

//    if (pdata->progress < 1.0) {
//        pdata->progress += 0.095;
//        //gtk_entry_set_progress_fraction(GTK_ENTRY(iio_w->widget), pdata->progress);

//        return TRUE;
//    } else {
        pdata->progress = 0.0;
        //        gtk_entry_set_progress_fraction(GTK_ENTRY(iio_w->widget), pdata->progress);
        if (!pdata->skip_widget_save)
            iio_widget_save(iio_w);
        if (pdata->on_complete != NULL)
            on_complete_cb(pdata->on_complete_data);
        pdata->timeoutID = -1;

        return FALSE;
//    }
}


/*
 * When a "value-changed" event of the spinbutton occurs the progress bar of the
 * spinbutton starts to increase until reaches the complete state. If another
 * event occurs while the progress is not finished, the progress will be reset.
 */

static void delayed_spin_button_update_cb(struct iio_widget *iio_w)
{
    struct progress_data *pdata =(struct progress_data *) iio_w->priv_progress;

//    if (pdata->timeoutID != - 1)
//        pdata->progress = 0.0;
//    else
//    {
//        pdata->timeoutID = 1;

//        QTimer *timer=new QTimer();
//        timer->setInterval(G_PRIORITY_DEFAULT_IDLE);

//        QObject::connect(timer,&QTimer::timeout,[=](){
            spin_button_progress_step(iio_w);
//        });
//        timer->start();

        //g_timeout_add_full(G_PRIORITY_DEFAULT_IDLE, 90,
        //               (GSourceFunc)spin_button_progress_step, iio_w, NULL);
//    }
}


void IIO_Widget::iio_spin_button_progress_activate(iio_widget *iio_w)
{
    struct progress_data *pdata =(struct progress_data *) iio_w->priv_progress;

    QString className=(iio_w->widget)->metaObject()->className();
    if (className !="QDoubleSpinBox" && className !="QSpinBox" ) {
        const char *name = iio_device_get_name(iio_w->dev) ?:
                               iio_device_get_id(iio_w->dev);
        printf("The widget connected to the attribute: %s of device: %s is not a QSpinBox\n",
               iio_w->attr_name, name);
        return;
    }

    QObject::connect(((QDoubleSpinBox*)iio_w->widget),
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged)
                     ,[=](){
                         delayed_spin_button_update_cb(iio_w);
                     });

}
