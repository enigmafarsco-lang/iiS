/**
 * @file oscpreferences.cpp
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "oscpreferences.h"


/**
 * @brief Constructs a new OscPreferences instance and initializes subsystem components and UI state.
 */
OscPreferences::OscPreferences()
{

}

IioChnPreferences *OscPreferences::iio_chn_preferences_new(const char *chn_name)
{
    IioChnPreferences *chn_pref = g_new0(IioChnPreferences, 1);

    chn_pref->chn_name = g_strdup(chn_name);

    return chn_pref;
}

/**
 * @brief Executes the iio chn preferences free procedure for OscPreferences.
 * @param pref Input parameter specifying pref (IioChnPreferences).
 */
void OscPreferences::iio_chn_preferences_free(IioChnPreferences *pref)
{
    g_free(pref->chn_name);
    pref->chn_name = NULL;
}

IioDevPreferences *OscPreferences::iio_dev_preferences_new(const char *dev_name)
{
    IioDevPreferences *dev_pref = g_new0(IioDevPreferences, 1);

    dev_pref->dev_name = g_strdup(dev_name);

    return dev_pref;
}

/**
 * @brief Executes the iio dev preferences free procedure for OscPreferences.
 * @param pref Input parameter specifying pref (IioDevPreferences).
 */
void OscPreferences::iio_dev_preferences_free(IioDevPreferences *pref)
{

    g_free(pref->dev_name);
    pref->dev_name = NULL;

    g_free(pref->fft_correction);
    pref->fft_correction = NULL;

//    g_list_free_full(pref->channels_pref_list,
//                     (GDestroyNotify)iio_chn_preferences_free);

}

OscPlotPreferences *OscPreferences::osc_plot_preferences_new()
{
    return g_new0(OscPlotPreferences, 1);
}

/**
 * @brief Executes the osc plot preferences free procedure for OscPreferences.
 * @param pref Input parameter specifying pref (OscPlotPreferences).
 */
void OscPreferences::osc_plot_preferences_free(OscPlotPreferences *pref)
{
    g_free(pref->sample_count);
    pref->sample_count = NULL;

//    g_list_free_full(pref->devices_pref_list,
//                     (GDestroyNotify)iio_dev_preferences_free);
}

OscPreferences *OscPreferences::osc_preferences_new()
{
    return g_new0(OscPreferences, 1);
}

/**
 * @brief Executes the osc preferences delete procedure for OscPreferences.
 * @param pref Input parameter specifying pref (OscPreferences).
 */
void OscPreferences::osc_preferences_delete(OscPreferences *pref)
{
    if (pref->plot_preferences) {
        osc_plot_preferences_free(pref->plot_preferences);
    }
}

