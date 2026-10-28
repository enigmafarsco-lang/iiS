/**
 * @file osc_plugin.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

    /**
     * @brief Executes the bool procedure for the module.
     * @param identify Input parameter specifying identify (*identify).
     */
    bool (*identify)(const struct osc_plugin *plugin);
    QWidget * (*init)(struct osc_plugin *plugin, QWidget *notebook, const char *ini_fn);
    /**
     * @brief Executes the int procedure for the module.
     * @param handle_item Input parameter specifying handle_item (*handle_item).
     */
    int (*handle_item) (struct osc_plugin *plugin, int line, const char *attrib, const char *value);
    /**
     * @brief Executes the int procedure for the module.
     * @param handle_external_request Input parameter specifying handle_external_request (*handle_external_request).
     */
    int (*handle_external_request) (struct osc_plugin *plugin, const char *request);
    /**
     * @brief Executes the void procedure for the module.
     * @param update_active_page Input parameter specifying update_active_page (*update_active_page).
     */
    void (*update_active_page)(struct osc_plugin *plugin, gint active_page, gboolean is_detached);
    /**
     * @brief Executes the void procedure for the module.
     * @param get_preferred_size Input parameter specifying get_preferred_size (*get_preferred_size).
     */
    void (*get_preferred_size)(const struct osc_plugin *plugin, int *width, int *size);
    OscPreferences *(*get_preferences_for_osc)(const osc_plugin *plugin);
    /**
     * @brief Executes the void procedure for the module.
     * @param destroy Input parameter specifying destroy (*destroy).
     */
    void (*destroy)(struct osc_plugin *plugin, const char *ini_fn);

    /**
     * @brief Executes the void procedure for the module.
     * @param save_profile Input parameter specifying save_profile (*save_profile).
     */
    void (*save_profile)(const struct osc_plugin *plugin, const char *ini_fn);
    /**
     * @brief Executes the void procedure for the module.
     * @param load_profile Input parameter specifying load_profile (*load_profile).
     */
    void (*load_profile)(struct osc_plugin *plugin, const char *ini_fn);
    GSList* (*get_dac_dev_names)(const struct osc_plugin *plugin);
    struct plugin_private *priv;
};


#endif // OSC_PLUGIN

