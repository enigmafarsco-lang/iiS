// ============================================================================
// File: dacDataManager.h
// Subsystem: eLynxSDR plugins
// Description: Subsystem implementation and interface guidance for dacDataManager.h.
// ============================================================================

#ifndef DAC_DATA_MANAGER_H
#define DAC_DATA_MANAGER_H

#include <QGroupBox>
#include <QScrollArea>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QWidget>
#include <glib.h>
#include <receiver/iio_widget.h>
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
    WIDGET_SCALE,
    WIDGET_PHASE
};


struct _complex_ref {
    double *re;
    double *im;
};



#define I_CHANNEL 'I'
#define Q_CHANNEL 'Q'

#define FREQUENCY_SPIN_DIGITS 6
#define SCALE_SPIN_DIGITS 0
#define PHASE_SPIN_DIGITS 3

#define SCALE_MINUS_INFINITE -91

#define TX_NB_TONES 4
#define CHANNEL_NB_TONES 2

#define TX_T1_I 0
#define TX_T2_I 1
#define TX_T1_Q 2
#define TX_T2_Q 3


#define IIO_SPIN_SIGNAL "value-changed"
#define IIO_COMBO_SIGNAL "changed"

#define TX_CHANNEL_NAME 0
#define TX_CHANNEL_ACTIVE 1
#define TX_CHANNEL_REF_INDEX 2

#define WAVEFORM_TXT_INVALID_FORMAT 1
#define WAVEFORM_MAT_INVALID_FORMAT 2


#define TONE_ID "altvoltage"
#define TONE_ID_SIZE (sizeof(TONE_ID) - 1)



struct dds_tone {
    struct dds_channel *parent;

    unsigned number;
    struct iio_device *iio_dac;
    struct iio_channel *iio_ch;

    struct iio_widget iio_freq;
    struct iio_widget iio_scale;
    struct iio_widget iio_phase;

    double scale_state;

    gint dds_freq_hid;
    gint dds_scale_hid;
    gint dds_phase_hid;

    QDoubleSpinBox *freq;
    QDoubleSpinBox *scale;
    QDoubleSpinBox *phase;
    QGroupBox *frame;
};

struct dds_channel {
    struct dds_tx *parent;

    char type;
    struct dds_tone t1;
    struct dds_tone t2;

    QGroupBox *frame;
};

struct dds_tx {
    struct dds_dac *parent;

    unsigned index;
    struct dds_channel ch_i;
    struct dds_channel ch_q;
    struct dds_tone *dds_tones[4];

    QGroupBox *frame;
    QComboBox *dds_mode_widget;
};

struct dds_dac {
    struct dac_data_manager *parent;

    unsigned index;
    const char *name;
    struct iio_device *iio_dac;
    unsigned tx_count;
    struct dds_tx *txs;
    int dds_mode;
    unsigned tones_count;

    QGroupBox *frame;
};

struct dac_buffer {
    struct dac_data_manager *parent;

    QString *dac_buf_filename;
    int scan_elements_count;
    struct iio_device *dac_with_scanelems;

    QGroupBox *frame;
    QWidget *buffer_fchooser_btn;
    QTreeWidget *tx_channels_view;
    QDoubleSpinBox *scale;
    QTextEdit *load_status_buf;
};

struct dac_data_manager {
    struct dds_dac dac1;
    struct dds_dac dac2;
    struct dac_buffer dac_buffer_module;

    struct iio_context *ctx;
    unsigned dacs_count;
    unsigned tones_count;
    unsigned alignment;
    bool hw_reported_alignment;
    GSList *dds_tones;
    bool scale_available_mode;
    double lowest_scale_point;
    bool dds_activated;
    bool dds_disabled;
    struct iio_buffer *dds_buffer;
    bool is_local;

    QHBoxLayout *container;

};

class dacDataManager : public QObject
{

    Q_OBJECT

private:


    QComboBox *mode = new QComboBox();
//struct dac_data_manager *manager;

    // Executes dac channels assign routine.
    int dac_channels_assign(struct dds_dac *ddac);
    char *get_tone_name(struct iio_channel *ch);
    char *build_default_channel_name_from_index(guint ch_index);
    // Executes gui manager create routine.
    void gui_manager_create(struct dac_data_manager *manager);
    // Executes manager iio setup routine.
    void manager_iio_setup(struct dac_data_manager *manager);
    // Executes dac data manager free routine.
    void dac_data_manager_free(struct dac_data_manager *manager);
    // Executes dac buffer init routine.
    void dac_buffer_init(struct dac_data_manager *manager, struct dac_buffer *d_buffer);
    // Executes device scan elements count routine.
    int device_scan_elements_count(struct iio_device *dev);
    // Retrieves current iio_tones_count state.
    unsigned get_iio_tones_count(struct iio_device *dev);
    // Executes dds non iq tx init routine.
    void dds_non_iq_tx_init(struct dds_dac *ddac, struct dds_tx *tx, unsigned dds_index);
    // Executes dds tx init routine.
    void dds_tx_init(struct dds_dac *ddac, struct dds_tx *tx, unsigned dds_index);
    // Executes enable dds routine.
    void enable_dds(dac_data_manager *manager, bool on_off);

    QGroupBox *gui_dac_create(dds_dac *ddac);
    QGroupBox *gui_tx_create(dds_tx *tx);
    QGroupBox *gui_channel_create(dds_channel *ch);
    QGroupBox *gui_tone_create(dds_tone *tone);
    QFrame *gui_dds_mode_chooser_create(dds_tx *tx);
    QGroupBox *frame_with_table_create(const char *frm_title);
    // Reads and parses incoming parse_wavefile_line data.
    int parse_wavefile_line(const char *line, double *vals, unsigned int max_num_vals);
    // Executes analyse wavefile routine.
    int analyse_wavefile(dac_data_manager *manager, const char *file_name, char **buf, int *count, int tx_channels, double full_scale);
    // Executes dac offset get value routine.
    double dac_offset_get_value(iio_device *dac);
    // Executes tx channels check valid setup routine.
    bool tx_channels_check_valid_setup(dac_buffer *dbuf);
    // Executes dma valid selection routine.
    bool dma_valid_selection(const char *device, unsigned int mask, unsigned int channel_count);
    const char *get_adi_part_code(const char *device_name);
    // Executes tx enabled channels count routine.
    int tx_enabled_channels_count(QTreeWidget *treeview, unsigned int *enabled_mask);
    // Executes convert routine.
    unsigned short convert(double scale, float val, double offset);
    // Executes replicate tx data channels routine.
    void replicate_tx_data_channels(_complex_ref *data, int count);
    // Executes freq spin range update routine.
    void freq_spin_range_update(dds_tone *tone, double tx_sample_rate);
    // Executes free channel routine.
    void freeChannel(char *ch_name);

    // Executes enable dds channels routine.
    void enable_dds_channels(dac_buffer *db);
    // Executes db full scale convert routine.
    static double db_full_scale_convert(double value, bool inverse);
    // Executes compare gain routine.
    static int compare_gain(const char *a, const char *b);
    // Executes save scale widget value routine.
    static void save_scale_widget_value(void *data);
    // Executes dds scale set value routine.
    void dds_scale_set_value(QWidget *scale, gdouble value);
    // Executes dds scale get value routine.
    double dds_scale_get_value(QWidget *scale);
    // Executes tone setup routine.
    void tone_setup(dds_tone *tone);
    // Executes dds tone iio widgets update routine.
    void dds_tone_iio_widgets_update(dds_tone *tone);
public:

    // Constructor: Initializes dacDataManager subsystem components, default parameters, and state.
    explicit dacDataManager(QObject *parent = nullptr);
    struct dac_data_manager *dac_data_manager_new(struct iio_device *dac,
                                                  struct iio_device *second_dac, struct iio_context *ctx);
    // Executes dac data manager set buffer size alignment routine.
    void dac_data_manager_set_buffer_size_alignment(struct dac_data_manager *manager, unsigned align);
    int dac_manager_init(struct dac_data_manager *manager,
                         struct iio_device *dac, struct iio_device *second_dac, struct iio_context *ctx);
    int dds_dac_init(struct dac_data_manager *manager,
                     struct dds_dac *ddac, struct iio_device *iio_dac);
    // Executes manager channels assign routine.
    int manager_channels_assign(struct dac_data_manager *manager);
    // Executes dac data manager update iio widgets routine.
    void dac_data_manager_update_iio_widgets(struct dac_data_manager *manager);
    QHBoxLayout *dac_data_manager_get_gui_container(struct dac_data_manager *manager);

    // Executes manage dds mode routine.
    void manage_dds_mode(QComboBox *box,dds_tx *tx);
    // Executes dds locked scale cb routine.
    void dds_locked_scale_cb(dds_tx *tx);
    // Executes dds locked phase cb routine.
    void dds_locked_phase_cb(dds_tx *tx);
    // Executes dds locked freq cb routine.
    void dds_locked_freq_cb(dds_tx *tx);
    // Executes scale spin button output cb routine.
    gboolean scale_spin_button_output_cb(QDoubleSpinBox *spin, gpointer data);
    QGroupBox *gui_dac_buffer_create(dac_buffer *d_buffer);
    // Event/Signal handler for waveform load button clicked cb.
    void waveform_load_button_clicked_cb(dac_buffer *dbuf);
    // Executes process dac buffer file routine.
    int process_dac_buffer_file(dac_data_manager *manager, const char *file_name, char **stat_msg);
    // Executes dac buffer config file set cb routine.
    void dac_buffer_config_file_set_cb(QString fileName, dac_buffer &dbuf);
    QScrollArea *gui_dac_channels_tree_create(dac_buffer *d_buffer);
    // Executes dac data manager freq widgets range update routine.
    void dac_data_manager_freq_widgets_range_update(dac_data_manager *manager, double tx_sample_rate);
    // Sets file value and updates internal subsystem configuration.
    QString setFile(QString fileName,double scale );
    // Executes disable dac routine.
    void DisableDac();
//    dac_data_manager *manager;
    QComboBox * dfdf;

//    QPushButton *fchooser_btn;

public slots:
    // Retrieves current data state.
    void getData(bool);
    // Retrieves current val state.
    void getVal(QComboBox*, bool);


};

#endif // DAC_DATA_MANAGER_H
