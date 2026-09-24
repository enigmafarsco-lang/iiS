#ifndef IIO_UTILS_H
#define IIO_UTILS_H

#include <string.h>
#include <ctype.h>
#include<glib.h>
#include "iio.h"
#include <gmodule.h>

class iio_utils
{
public:
    iio_utils();
    GArray *get_iio_channels_naturally_sorted(iio_device *dev);
    char *iio_get_device_label(const iio_device *dev);
    GArray *get_iio_devices_starting_with(iio_context *ctx, const char *sequence);
    static gint iio_dev_cmp_by_name(gconstpointer ptr_a, gconstpointer ptr_b);
};

#endif // IIO_UTILS_H
