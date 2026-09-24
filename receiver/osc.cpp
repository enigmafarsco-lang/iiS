#include "osc.h"
#include <receiver/plot.h>
#include <receiver/oscmain.h>

OSCMain *oscMain;
#define DMA_DEVICES_COUNT (sizeof(dma_devices) / sizeof(dma_devices[0]))

/*
 * There is a bug in libiio that lets it return -EIO if no trigger is
 * assigned. Assume that EIO means there is a trigger, but none
 * assigned. Drop since once libiio has been fixed for a while.
 */
int osc_iio_device_get_trigger(const struct iio_device *dev,const struct iio_device **trigger)
{
    int ret;

    ret = iio_device_get_trigger(dev, trigger);
    if (ret == -EIO) {
        ret = 0;
        *trigger = NULL;
    }

    return ret;
}

/**
 * @brief read_sampling_frequency
 * @param dev
 * @return
 */
static double read_sampling_frequency(const struct iio_device *dev)
{
    double freq = 400.0;
    int ret = -1;
    unsigned int i, nb_channels = iio_device_get_channels_count(dev);
    const char *attr;
    char buf[1024];

    for (i = 0; i < nb_channels; i++) {
        struct iio_channel *ch = iio_device_get_channel(dev, i);

        if (iio_channel_is_output(ch) || strncmp(iio_channel_get_id(ch),
                                                 "voltage", sizeof("voltage") - 1))
            continue;

        ret = iio_channel_attr_read(ch, "sampling_frequency",
                                    buf, sizeof(buf));
        if (ret > 0)
            break;
    }

    if (ret < 0)
        ret = iio_device_attr_read(dev, "sampling_frequency",
                                   buf, sizeof(buf));
    if (ret < 0) {
        const struct iio_device *trigger;

        ret = osc_iio_device_get_trigger(dev, &trigger);
        if (ret == 0 && trigger) {
            attr = iio_device_find_attr(trigger, "sampling_frequency");
            if (!attr)
                attr = iio_device_find_attr(trigger, "frequency");
            if (attr)
                ret = iio_device_attr_read(trigger, attr, buf,
                                           sizeof(buf));
            else
                ret = -ENOENT;
        }
    }

    if (ret > 0)
        sscanf(buf, "%lf", &freq);

    if (freq < 0)
        freq += 4294967296.0;

    return freq;
}

/**
 * @brief osc::rx_update_device_sampling_freq
 * @param device
 * @param freq
 * @return
 */
bool osc::rx_update_device_sampling_freq(const char *device, double freq)
{
    struct iio_device *dev;
    struct extra_dev_info *info;

    g_return_val_if_fail(device, false);

    dev = iio_context_find_device(globals::ctx, device);
    if (!dev) {
        fprintf(stderr, "Device: %s not found!\n", device);
        return false;
    }

    info = (extra_dev_info *)iio_device_get_data(dev);
    if (!info) {
        fprintf(stderr, "Device: %s extra info not found!\n", device);
        return false;
    }

    if (freq >= 0)
        info->adc_freq = freq;
    else
        info->adc_freq = read_sampling_frequency(dev);

    if (info->adc_freq >= 1000000) {
        info->adc_scale = 'M';
        info->adc_freq /= 1000000.0;
    } else if (info->adc_freq >= 1000) {
        info->adc_scale = 'k';
        info->adc_freq /= 1000.0;
    } else if (info->adc_freq >= 0) {
        info->adc_scale = ' ';
    } else {
        info->adc_scale = '?';
        info->adc_freq = 0.0;
    }

    GList *node;

    //OscPlot *oscplotInstance;
//    for (node = plot_list; node; node = g_list_next(node))
//        oscplotInstance->osc_plot_update_rx_lbl(OSC_PLOT(node->data), NORMAL_UPDATE);

    return true;
}

/**
 * @brief osc::rx_update_channel_lo_freq
 * @param device
 * @param channel
 * @param lo_freq
 * @return
 */
bool osc::rx_update_channel_lo_freq(const char *device, const char *channel, double lo_freq)
{
    struct iio_device *dev;
    struct iio_channel *chn;
    struct extra_info *chn_info;

    g_return_val_if_fail(device, false);
    g_return_val_if_fail(channel, false);

    dev = iio_context_find_device(globals::ctx, device);
    if (!dev) {
        fprintf(stderr, "Device: %s not found\n!", device);
        return false;
    }

    if (!strcmp(channel, "all")) {
        bool success = true;
        unsigned int i = 0;
        for (; i < iio_device_get_channels_count(dev); i++) {
            chn = iio_device_get_channel(dev, i);
            if (!iio_channel_is_scan_element(chn) ||
                    iio_channel_is_output(chn)) {
                continue;
            }
            chn_info = (extra_info *)iio_channel_get_data(chn);
            if (chn_info) {
                chn_info->lo_freq = lo_freq;
            } else {
                fprintf(stderr, "Channel: %s extra info "
                                "not found!\n", channel);
                success = false;
            }
        }
        return success;
    }

    chn = iio_device_find_channel(dev, channel, false);
    if (!chn) {
        fprintf(stderr, "Channel: %s not found!\n", channel);
        return false;
    }

    chn_info = (extra_info *)iio_channel_get_data(chn);
    if (!chn_info) {
        fprintf(stderr, "Channel: %s extra info not found!\n", channel);
        return false;
    }

    chn_info->lo_freq = lo_freq;

    return true;
}

/**
 * @brief global_enabled_channels_mask
 * @param dev
 * @return
 */
unsigned global_enabled_channels_mask(struct iio_device *dev)
{
    unsigned mask = 0;
    int scan_i = 0;
    unsigned int i = 0;

    for (; i < iio_device_get_channels_count(dev); i++) {
        struct iio_channel *chn = iio_device_get_channel(dev, i);

        if (iio_channel_is_scan_element(chn)) {
            if (iio_channel_is_enabled(chn))
                mask |= 1 << scan_i;
            scan_i++;
        }
    }

    return mask;
}
