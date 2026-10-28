/**
 * @file osc.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef OSC_H
#define OSC_H

#include <complex.h>
#include <iio.h>
#include <receiver/globals.h>

#include <receiver/libini2.h>
#include <receiver/osc_plugin.h>
#include <receiver/datatypes.h>

#include <glib.h>
#include <glib-object.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <glib.h>
#include <receiver/compat.h>

#include <QDebug>

extern int osc_iio_device_get_trigger(const struct iio_device *dev,
                                      const struct iio_device **trigger);

/**
 * @class osc
 * @brief High-level controller class for osc.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class osc
{

public:
    /**
     * @brief Constructs a new osc instance and initializes subsystem components and UI state.
     */
    osc();
    /**
     * @brief Executes the rx update device sampling freq procedure for osc.
     * @param device Input parameter specifying device (const char).
     * @param freq Input parameter specifying freq (double).
     * @return Output value of type bool.
     */
    bool rx_update_device_sampling_freq(const char *device, double freq);
    bool rx_update_channel_lo_freq(const char *device, const char *channel,
                                   double lo_freq);
};

#endif // OSC_H
