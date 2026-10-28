/**
 * @file dacDataManager.h
 * @brief High-Speed DAC Buffer Management & DDS Direct Digital Synthesis for IIO Transceivers.
 *
 * dacDataManager manages high-speed baseband sample streams uploaded to DAC DMA cores:
 * - Direct memory-mapped cyclic DMA buffer transfers via libiio (`iio_buffer_create`).
 * - Formatted text and binary IQ sample parsing (signed 16-bit integers, interleaved I/Q).
 * - Multi-tone Direct Digital Synthesis (DDS) carrier generation on hardware transceivers.
 * - Multi-channel selection (Voltage 0/1 for TX1, Voltage 2/3 for TX2).
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef DAC_DATA_MANAGER_H
#define DAC_DATA_MANAGER_H

#include <QGroupBox>
#include <QScrollArea>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QWidget>
#include <glib.h>
#include <stdarg.h>
#include <matio.h>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QDoubleSpinBox>
#include <QTreeWidget>
#include <QFileDialog>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef __linux__
#include <sys/utsname.h>
#endif
#include <unistd.h>

#include "receiver/iio_widget.h"

#define DDS_DISABLED  0
#define DDS_ONE_TONE  1
#define DDS_TWO_TONE  2
#define DDS_INDEPDENT 3
#define DDS_BUFFER    4

static const gdouble abs_mhz_scale = -1000000.0;
static const gdouble khz_scale = 1000.0;

static char * dma_devices[] = {
    "ad9122",
    "ad9144",
    "ad9250",
    "ad9361",
    "ad9643",
    "ad9680",
    "ad9371"
};

#define DMA_DEVICES_COUNT (sizeof(dma_devices) / sizeof(dma_devices[0]))

enum dds_tone_type {
    TONE_I,
    TONE_Q
};

enum dds_tone_index {
    TONE_1,
    TONE_2
};

enum dds_widget_type {
    WIDGET_FREQUENCY,
    WIDGET_PHASE,
    WIDGET_SCALE,
    WIDGET_RAW,
    WIDGET_SAMPLING_FREQ,
    WIDGET_COUNT
};

struct dds_widget {
    QDoubleSpinBox * spin;
    gdouble scale;
    gdouble default_value;
    struct iio_channel * chn;
    const char * attr_name;
    const char * channel_name;
};

struct dds_tone {
    struct dds_widget widgets[WIDGET_COUNT];
    int index;
    int type;
};

struct dds_channel {
    struct dds_tone tones[2];
    int index;
    const char * name;
    QGroupBox * dds_group;
    QGroupBox * data_group;
    int dds_mode;
    QComboBox * dds_mode_combo;
    QDoubleSpinBox * sampling_freq;
    QTextEdit * dac_buffer_file;
    bool is_tx;
};

/**
 * @class dac_data_manager
 * @brief High-speed DAC DMA buffer and Direct Digital Synthesis manager for ADRV9009.
 */
class dac_data_manager : public iio_widget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs dac_data_manager widget.
     * @param dev IIO device handle for DAC / TX core.
     * @param can_change_mode Whether DDS mode can be altered dynamically.
     * @param is_tx True if managing transmit channels.
     * @param has_buffer Whether device supports cyclic DMA sample buffers.
     * @param has_independent_dds Whether channel supports dual independent DDS tones.
     * @param has_cw_mode Whether channel supports single CW tone mode.
     */
    dac_data_manager(struct iio_device * dev,
                     bool can_change_mode,
                     bool is_tx,
                     bool has_buffer,
                     bool has_independent_dds,
                     bool has_cw_mode);

    ~dac_data_manager();

    /**
     * @brief Loads an IQ sample file and streams it to the cyclic DAC DMA buffer.
     * @param file Path to IQ sample file.
     * @param scale Digital scaling multiplier.
     * @param voltage01 Route to TX1 (voltage0 / voltage1).
     * @param voltage23 Route to TX2 (voltage2 / voltage3).
     * @return Status description message.
     */
    QString setFile(QString file, double scale = 1.0,
                    bool voltage01 = true, bool voltage23 = true);

    /**
     * @brief Disables DAC transmission and destroys active DMA buffers.
     */
    void DisableDac();

    /**
     * @brief Configures DDS tone parameters across transmit channels.
     * @param freqMHz Tone frequency in MHz.
     * @param scaleDbfs Tone scale in dBFS.
     * @param phaseDeg Tone phase in degrees.
     */
    void setCwDdsParams(double freqMHz, double scaleDbfs, double phaseDeg);

private:
    struct iio_device * dac_device;
    struct iio_buffer * dac_buf;
    bool m_is_tx;
    bool m_has_buffer;
};

#endif // DAC_DATA_MANAGER_H
