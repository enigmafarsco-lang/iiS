#ifndef OSC_PLUGIN
#define OSC_PLUGIN

#include "oscpreferences.h"

#include <QWidget>
#include <glib.h>

/* Information needed to create a new plugin */
struct osc_plugin_context {
    char *plugin_name;
    GList *required_devices;
};


struct osc_plugin {
    void *handle;
    const char *name;
    QThread *thd;
    bool dynamically_created;

    bool (*identify)(const struct osc_plugin *plugin);
    QWidget * (*init)(struct osc_plugin *plugin, QWidget *notebook, const char *ini_fn);
    int (*handle_item) (struct osc_plugin *plugin, int line, const char *attrib, const char *value);
    int (*handle_external_request) (struct osc_plugin *plugin, const char *request);
    void (*update_active_page)(struct osc_plugin *plugin, gint active_page, gboolean is_detached);
    void (*get_preferred_size)(const struct osc_plugin *plugin, int *width, int *size);
    OscPreferences *(*get_preferences_for_osc)(const osc_plugin *plugin);
    void (*destroy)(struct osc_plugin *plugin, const char *ini_fn);

    void (*save_profile)(const struct osc_plugin *plugin, const char *ini_fn);
    void (*load_profile)(struct osc_plugin *plugin, const char *ini_fn);
    GSList* (*get_dac_dev_names)(const struct osc_plugin *plugin);
    struct plugin_private *priv;
};


#endif // OSC_PLUGIN

