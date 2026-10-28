/**
 * @file iio_widget.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

    /**
     * @brief Executes the void procedure for the module.
     * @param save Input parameter specifying save (*save).
     */
    void (*save)(struct iio_widget *);
    /**
     * @brief Executes the void procedure for the module.
     * @param update Input parameter specifying update (*update).
     */
    void (*update)(struct iio_widget *);
    /**
     * @brief Executes the void procedure for the module.
     * @param update_value Input parameter specifying update_value (*update_value).
     */
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
    /**
     * @brief Executes the void procedure for the module.
     * @param on_complete Input parameter specifying on_complete (*on_complete).
     */
    void (*on_complete)(void *data);
};


/**
 * @class IIO_Widget
 * @brief High-level controller class for IIO_Widget.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class IIO_Widget
{
public:
    /**
     * @brief Constructs a new IIO_Widget instance and initializes subsystem components and UI state.
     */
    IIO_Widget();
    /**
     * @brief Executes the iio widget update procedure for IIO_Widget.
     * @param widget Input parameter specifying widget (iio_widget).
     */
    void iio_widget_update(iio_widget *widget);
    /**
     * @brief Executes the iio combo box init procedure for IIO_Widget.
     * @param widget Input parameter specifying widget (iio_widget).
     * @param dev Input parameter specifying dev (iio_device).
     * @param chn Input parameter specifying chn (iio_channel).
     * @param attr_name Input parameter specifying attr_name (const char).
     * @param attr_name_avail Input parameter specifying attr_name_avail (const char).
     * @param combo_box Input parameter specifying combo_box (QWidget).
     * @param (*compare Input parameter specifying (*compare (int).
     */
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
    /**
     * @brief Executes the iio spin button add progress procedure for IIO_Widget.
     * @param iio_w Input parameter specifying iio_w (struct iio_widget).
     */
    void iio_spin_button_add_progress(struct iio_widget *iio_w);
    void iio_spin_button_s64_init(struct iio_widget *widget, struct iio_device *dev,
        struct iio_channel *chn, const char *attr_name,
        QWidget *spin_button, const gdouble *scale);
    void iio_spin_button_int_init(struct iio_widget *widget, struct iio_device *dev,
        struct iio_channel *chn, const char *attr_name,
        QWidget *spin_button, const gdouble *scale);
    void iio_update_widgets_of_device(struct iio_widget *widgets,
            unsigned int num_widgets, struct iio_device *dev);
    /**
     * @brief Executes the iio update widgets procedure for IIO_Widget.
     * @param widgets Input parameter specifying widgets (struct iio_widget).
     * @param num_widgets Input parameter specifying num_widgets (unsigned int).
     */
    void iio_update_widgets(struct iio_widget *widgets, unsigned int num_widgets);
    /**
     * @brief Executes the iio spin button progress activate procedure for IIO_Widget.
     * @param iio_w Input parameter specifying iio_w (struct iio_widget).
     */
    void iio_spin_button_progress_activate(struct iio_widget *iio_w);
    /**
     * @brief Executes the iio spin button skip save on complete procedure for IIO_Widget.
     * @param iio_w Input parameter specifying iio_w (iio_widget).
     * @param skip Input parameter specifying skip (gboolean).
     */
    void iio_spin_button_skip_save_on_complete(iio_widget *iio_w, gboolean skip);
    /**
     * @brief Executes the iio spin button set on complete function procedure for IIO_Widget.
     * @param iio_w Input parameter specifying iio_w (iio_widget).
     * @param (*on_complete Input parameter specifying (*on_complete (void).
     */
    void iio_spin_button_set_on_complete_function(iio_widget *iio_w, void (*on_complete)(void *), void *data);
    /**
     * @brief Executes the iio spin button set convert function procedure for IIO_Widget.
     * @param iio_w Input parameter specifying iio_w (iio_widget).
     * @param (*convert Input parameter specifying (*convert (double).
     */
    void iio_spin_button_set_convert_function(iio_widget *iio_w, double (*convert)(double, bool));
private:
};

#endif // IIO_WIDGET_H
