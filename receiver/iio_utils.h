/**
 * @file iio_utils.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef IIO_UTILS_H
#define IIO_UTILS_H

#include <string.h>
#include <ctype.h>
#include<glib.h>
#include "iio.h"
#include <gmodule.h>

/**
 * @class iio_utils
 * @brief High-level controller class for iio_utils.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class iio_utils
{
public:
    /**
     * @brief Constructs a new iio_utils instance and initializes subsystem components and UI state.
     */
    iio_utils();
    GArray *get_iio_channels_naturally_sorted(iio_device *dev);
    char *iio_get_device_label(const iio_device *dev);
    GArray *get_iio_devices_starting_with(iio_context *ctx, const char *sequence);
    /**
     * @brief Executes the iio dev cmp by name procedure for iio_utils.
     * @param ptr_a Input parameter specifying ptr_a (gconstpointer).
     * @param ptr_b Input parameter specifying ptr_b (gconstpointer).
     * @return Output value of type static gint.
     */
    static gint iio_dev_cmp_by_name(gconstpointer ptr_a, gconstpointer ptr_b);
};

#endif // IIO_UTILS_H
