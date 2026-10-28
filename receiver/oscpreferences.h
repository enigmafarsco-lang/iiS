/**
 * @file oscpreferences.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef OSCPREFERENCES_H
#define OSCPREFERENCES_H


#include <glib.h>
#include <iio.h>

//typedef struct osc_preferences OscPreferences;
typedef struct oscplot_preferences OscPlotPreferences;
typedef struct iio_device_preferences IioDevPreferences;
typedef struct iio_channel_preferences IioChnPreferences;

struct oscplot_preferences {
        unsigned int *sample_count;
        GList *devices_pref_list;
};

struct iio_channel_preferences {
        char *chn_name;
        unsigned int *constraints;
};

struct iio_device_preferences {
        char *dev_name;
        double *fft_correction;
        GList *channels_pref_list;
};


/**
 * @class OscPreferences
 * @brief High-level controller class for OscPreferences.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class OscPreferences
{
public:
    /**
     * @brief Constructs a new OscPreferences instance and initializes subsystem components and UI state.
     */
    OscPreferences();

    IioChnPreferences *iio_chn_preferences_new(const char *chn_name);
    /**
     * @brief Executes the iio chn preferences free procedure for OscPreferences.
     * @param pref Input parameter specifying pref (IioChnPreferences).
     */
    void iio_chn_preferences_free(IioChnPreferences *pref);

    IioDevPreferences *iio_dev_preferences_new(const char *dev_name);
    /**
     * @brief Executes the iio dev preferences free procedure for OscPreferences.
     * @param pref Input parameter specifying pref (IioDevPreferences).
     */
    void iio_dev_preferences_free(IioDevPreferences *pref);

    OscPlotPreferences *osc_plot_preferences_new();
    /**
     * @brief Executes the osc plot preferences free procedure for OscPreferences.
     * @param pref Input parameter specifying pref (OscPlotPreferences).
     */
    void osc_plot_preferences_free(OscPlotPreferences *pref);

    OscPreferences *osc_preferences_new();
    /**
     * @brief Executes the osc preferences delete procedure for OscPreferences.
     * @param pref Input parameter specifying pref (OscPreferences).
     */
    void osc_preferences_delete(OscPreferences *pref);

    struct oscplot_preferences *plot_preferences;
};

#endif // OSCPREFERENCES_H
