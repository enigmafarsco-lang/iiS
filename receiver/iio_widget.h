#ifndef IIO_WIDGET_H
#define IIO_WIDGET_H


#include <QComboBox>
#include <QWidget>
#include <QTreeView>
#include <glib.h>
#include <QDebug>
#include <QAbstractButton>
#include <QSpinBox>
#include <iio.h>
#include <math.h>

struct iio_widget {
    struct iio_device *dev;
    struct iio_channel *chn;
    const char *attr_name;
    const char *attr_name_avail;
    const char *value;
    QWidget *widget;
    void *priv;
    void *priv_progress;
    void *priv_convert_function;

    void (*save)(struct iio_widget *);
    void (*update)(struct iio_widget *);
    void (*update_value)(struct iio_widget *, const char *, size_t);
    void *sig_handler_data;
};

struct update_widgets_params {
    iio_widget * widgets;
    unsigned int nb;
};

struct progress_data {
    gboolean is_progress_spin_button;
    gfloat progress;
    gint timeoutID;
    gint value_changed_hid;
    gboolean skip_widget_save;
    void *on_complete_data;
    void (*on_complete)(void *data);
};


class IIO_Widget
{
public:
    IIO_Widget();
    void iio_widget_update(iio_widget *widget);
    void iio_combo_box_init(iio_widget *widget, iio_device *dev, iio_channel *chn, const char *attr_name, const char *attr_name_avail, QWidget *combo_box, int (*compare)(const char *, const char *));
    void iio_toggle_button_init(struct iio_widget *widget,
        struct iio_device *dev, struct iio_channel *chn, const char *attr_name,
        QWidget *toggle_button, const bool invert);
    void iio_button_init(struct iio_widget *widget,
                    struct iio_device *dev, struct iio_channel *chn, const char *attr_name,
                    QWidget *button);
    void iio_spin_button_init(struct iio_widget *widget, struct iio_device *dev,
        struct iio_channel *chn, const char *attr_name,
        QWidget *spin_button, const gdouble *scale);
    void iio_spin_button_add_progress(struct iio_widget *iio_w);
    void iio_spin_button_s64_init(struct iio_widget *widget, struct iio_device *dev,
        struct iio_channel *chn, const char *attr_name,
        QWidget *spin_button, const gdouble *scale);
    void iio_spin_button_int_init(struct iio_widget *widget, struct iio_device *dev,
        struct iio_channel *chn, const char *attr_name,
        QWidget *spin_button, const gdouble *scale);
    void iio_update_widgets_of_device(struct iio_widget *widgets,
            unsigned int num_widgets, struct iio_device *dev);
    void iio_update_widgets(struct iio_widget *widgets, unsigned int num_widgets);
    void iio_spin_button_progress_activate(struct iio_widget *iio_w);
    void iio_spin_button_skip_save_on_complete(iio_widget *iio_w, gboolean skip);
    void iio_spin_button_set_on_complete_function(iio_widget *iio_w, void (*on_complete)(void *), void *data);
    void iio_spin_button_set_convert_function(iio_widget *iio_w, double (*convert)(double, bool));
private:
};

#endif // IIO_WIDGET_H
