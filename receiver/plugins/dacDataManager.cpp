#include "dacDataManager.h"

#pragma region Constructor {

/**
 * @brief dacDataManager::dacDataManager
 * @param parent
 */
dacDataManager::dacDataManager(QObject *parent): QObject(parent)
{    

//    QComboBox * dfdf2 = new QComboBox;
//    dfdf = dfdf2;
//    manager =(dac_data_manager *) calloc(1, sizeof(struct dac_data_manager));

    //    jjj = new QComboBox;
}

#pragma endregion }

#pragma region Initialize UI {

/**
 * @brief dacDataManager::dac_data_manager_new
 * @param dac
 * @param second_dac
 * @param ctx
 * @return
 */
dac_data_manager *dacDataManager::dac_data_manager_new(iio_device *dac, iio_device *second_dac, iio_context *ctx )
{
    int ret;

     dac_data_manager *manager =(dac_data_manager *) calloc(1, sizeof(struct dac_data_manager));

    if (!manager) {
        printf("Memory allocation of struct dac_data_manager failed\n");
        return NULL;
    }

    if (!ctx || !dac)
        goto init_error;

    ret = dac_manager_init(manager, dac, second_dac, ctx);
    if (ret < 0)
        goto init_error;

    ret = manager_channels_assign(manager);
    if (ret < 0)
        goto init_error;

    gui_manager_create(manager);
    manager_iio_setup(manager);

    return manager;

init_error:
    dac_data_manager_free(manager);
    return NULL;
}

/**
 * @brief dacDataManager::dac_manager_init
 * @param manager
 * @param dac
 * @param second_dac
 * @param ctx
 * @return
 */
int dacDataManager::dac_manager_init(dac_data_manager *manager, iio_device *dac, iio_device *second_dac, iio_context *ctx)
{
    long long alignment;
    int ret = 0;

    ret = dds_dac_init(manager, &manager->dac1, dac);
    if (ret < 0)
        return ret;
    ret = dds_dac_init(manager, &manager->dac2, second_dac);
    if (ret < 0)
        return ret;

    dac_buffer_init(manager, &manager->dac_buffer_module);

    struct iio_channel *ch = iio_device_find_channel(dac, "altvoltage0", true);

    if (iio_channel_find_attr(ch, "scale_available"))
        manager->scale_available_mode = true;
    else
        manager->scale_available_mode = false;

    manager->is_local = strcmp(iio_context_get_name(ctx), "local") ? false : true;
    manager->ctx = ctx;

    if (iio_device_buffer_attr_read_longlong(manager->dac_buffer_module.dac_with_scanelems,
                                             "length_align_bytes",
                                             &alignment) == 0) {
        manager->alignment = alignment;
        manager->hw_reported_alignment = true;
    } else {
        manager->alignment = 8;
        manager->hw_reported_alignment = false;
    }

    return ret;
}

/**
 * @brief dacDataManager::dds_dac_init
 * @param manager
 * @param ddac
 * @param iio_dac
 * @return
 */
int dacDataManager::dds_dac_init(dac_data_manager *manager, dds_dac *ddac, iio_device *iio_dac)
{
    int ret = 0;

    if (!iio_dac)
        return 0;

    ddac->parent = manager;
    ddac->iio_dac = iio_dac;
    ddac->name = iio_device_get_name(iio_dac);
    ddac->tones_count = get_iio_tones_count(iio_dac);

    guint tx_count = ddac->tones_count / TX_NB_TONES;
    guint extra_tones = ddac->tones_count % TX_NB_TONES;
    if (tx_count == 0) {
        /* Some devices don't have the I-Q concept. One use case is: AD9739A eval board */
        if (extra_tones == 2) {
            ddac->txs = (dds_tx*)calloc(1, sizeof(struct dds_tx));
            dds_non_iq_tx_init(ddac, &ddac->txs[0], 1);
        } else {
            fprintf(stderr, "DacDataManager can't handle a device"
                            "with %u number of tones\n", ddac->tones_count);
            return -1;
        }
    } else {
        ddac->txs =(dds_tx*) calloc(tx_count, sizeof(struct dds_tx));
        guint tx = 0;
        for (; tx < tx_count; tx++) {
            dds_tx_init(ddac, &ddac->txs[tx], tx + 1);
        }
    }

    manager->dacs_count++;
    ddac->index = manager->dacs_count;

    return ret;
}

/**
 * @brief dacDataManager::dds_non_iq_tx_init
 * @param ddac
 * @param tx
 * @param dds_index
 */
void dacDataManager::dds_non_iq_tx_init(dds_dac *ddac, dds_tx *tx, unsigned dds_index)
{
    tx->index = dds_index;
    tx->ch_i.type = I_CHANNEL;
    tx->ch_i.t1.number = 1;
    tx->ch_i.t2.number = 2;
    tx->ch_q.type = CHAR_MAX;
    tx->ch_q.t1.number = 0;
    tx->ch_q.t2.number = 0;

    tx->parent = ddac;
    tx->ch_i.parent = tx;
    tx->ch_q.parent = NULL;
    tx->ch_i.t1.parent = &tx->ch_i;
    tx->ch_i.t2.parent = &tx->ch_i;
    tx->ch_q.t1.parent = NULL;
    tx->ch_q.t2.parent = NULL;

    tx->dds_tones[0] = &tx->ch_i.t1;
    tx->dds_tones[1] = &tx->ch_i.t2;
    tx->dds_tones[2] = NULL;
    tx->dds_tones[3] = NULL;

    ddac->tx_count++;
}

/**
 * @brief dacDataManager::gui_manager_create
 * @param manager
 */
void dacDataManager::gui_manager_create(dac_data_manager *manager)
{
    QHBoxLayout *hbox= new QHBoxLayout;
    QVBoxLayout *vbox= new QVBoxLayout;

    if (!manager)
        return;

    vbox->addWidget(gui_dac_create(&manager->dac1));

    if (manager->dacs_count == 2)
        vbox->addWidget(gui_dac_create(&manager->dac2));

    vbox->addWidget(gui_dac_buffer_create(&manager->dac_buffer_module));

    hbox->addItem(vbox);

    manager->container = hbox;
//    mode = manager->dac1.txs[0].dds_mode_widget;

}

/**
 * @brief frame_with_table_create
 * @param frm_title
 * @param rows
 * @param columns
 * @return
 */
QGroupBox *dacDataManager::frame_with_table_create(const char *frm_title)
{
    QGroupBox *groupBox= new QGroupBox;
    groupBox->setTitle(frm_title);
    return groupBox;
}

/**
 * @brief gui_dds_mode_chooser_create
 * @param tx
 * @return
 */
QFrame *dacDataManager::gui_dds_mode_chooser_create(struct dds_tx *tx)
{
    QHBoxLayout *box= new QHBoxLayout();
    QLabel *dds_mode_lbl= new QLabel("DDS Mode:");
    QComboBox *dds_mode = new QComboBox();

    dds_mode->addItem("Disable");
    dds_mode->addItem("One CW Tone");
    dds_mode->addItem("Two CW Tones");
    dds_mode->addItem("Independent I/Q Control");

    bool no_buffer_support = !strcmp(tx->parent->name, "axi-ad9739a-hpc");
    if (no_buffer_support == false)
        dds_mode->addItem("DAC Buffer Output");

    dds_mode->setCurrentIndex(0);

    box->addWidget(dds_mode_lbl);
    box->addWidget(dds_mode);

    dds_mode->setCurrentIndex(DDS_ONE_TONE);

    tx->dds_mode_widget = dds_mode;

    QFrame *frame=new QFrame();

    frame->setLayout(box);

//    getVal(dds_mode,false);




//     mode = dds_mode;

    return frame;
}


void dacDataManager::getVal(QComboBox * cmb,bool isWork)
{
//    mode = cmb;

    if(isWork) cmb->setCurrentIndex(3);
}

/**
 * @brief create tone groupbox
 * @param tone : tone item
 * @return tone groupbox
 */
QGroupBox *dacDataManager::gui_tone_create(struct dds_tone *tone)
{
    QGroupBox *tone_frm;
    bool combobox_scales;
    char tone_label[16];

    // Extract scale combobox availability
    combobox_scales = tone->parent->parent->parent->parent->scale_available_mode;

    // generate tone label name
    snprintf(tone_label, sizeof(tone_label), "Tone %u", tone->number);

    // Create an empty tone groupbox
    tone_frm = frame_with_table_create(tone_label);
    tone->frame = tone_frm;

    // Initialize tone widgets
    QLabel *freq, *scale, *phase;

    freq = new QLabel("Frequency(MHz):");
    phase = new QLabel("Phase(degrees):");

    tone->freq = new QDoubleSpinBox();
    tone->freq->setMinimum(-122.878124);
    tone->freq->setMaximum(122.878124);
    tone->freq->setSingleStep(1.0);
    tone->freq->setDecimals(FREQUENCY_SPIN_DIGITS);
    tone->freq->setMinimumHeight(40);
    tone->freq->setMaximumHeight(40);
//    tone->freq->setValue(50);

    tone->phase = new QDoubleSpinBox();
    tone->phase->setMinimum(0.0);
    tone->phase->setMaximum(360.0);
    tone->phase->setSingleStep(1.0);
    tone->phase->setDecimals(PHASE_SPIN_DIGITS);
    tone->phase->setMinimumHeight(40);
    tone->phase->setMaximumHeight(40);

    if (combobox_scales) {
        scale = new QLabel("Scale:");
        //tone->scale =new QDoubleSpinBox();
    } else {
        scale = new QLabel("Scale(dBFS):");
        tone->scale = new QDoubleSpinBox();
        tone->scale->setMinimum(-91.0);
        tone->scale->setMaximum(0.0);
        tone->scale->setSingleStep(1.0);
        tone->scale->setDecimals(SCALE_SPIN_DIGITS);
        tone->scale->setMinimumHeight(40);
        tone->scale->setMaximumHeight(40);
    }

    QHBoxLayout *hbox=new QHBoxLayout;
    QVBoxLayout *vbox1=new QVBoxLayout;
    QVBoxLayout *vbox2=new QVBoxLayout;

    // Add labels to first vertical layout
    vbox1->addWidget(freq);
    vbox1->addWidget(scale);
    vbox1->addWidget(phase);

    // Add spinboxes to second vertical layout
    vbox2->addWidget(tone->freq);
    vbox2->addWidget(tone->scale);
    vbox2->addWidget(tone->phase);

    // add above vertical layouts to a horiznotal layout
    hbox->addItem(vbox1);
    hbox->addItem(vbox2);

    // set above horizontal layout as tone layout
    tone_frm->setLayout(hbox);

    return tone_frm;
}

/**
 * @brief Create channel groupbox
 * @param ch : channel item
 * @return channel groupbox
 */
QGroupBox *dacDataManager::gui_channel_create(struct dds_channel *ch)
{
    QGroupBox *channel_frm;
    char channel_label[32];

    // Generate channel groupbox title
    snprintf(channel_label, sizeof(channel_label), "Channel %c", ch->type);

    // Create an empty groupbox with channel title
    channel_frm = frame_with_table_create(channel_label);

    // Create a horizontal layout
    QHBoxLayout *hbox=new QHBoxLayout;

    // Generate and add first tone groupbox to horizontal layout
    hbox->addWidget(gui_tone_create(&ch->t1));

    // Generate and add second tone groupbox to horizontal layout
    hbox->addWidget(gui_tone_create(&ch->t2));

    // Set above horizontal layout as channel group box layout
    channel_frm->setLayout(hbox);

    // Set created channel groupbox as channel item frame
    ch->frame = channel_frm;

    // return created channel groupbox
    return channel_frm;
}

/**
 * @brief Create tx groupbox
 * @param tx : tx item
 * @return tx groupbox
 */
QGroupBox *dacDataManager::gui_tx_create(struct dds_tx *tx)
{
    QGroupBox *txmodule_frm;
    char txmodule_label[16];

    unsigned int dac_index = (tx->parent->index - 1) * (tx->parent->tones_count / 4);

    // Generate title for tx group box header
    snprintf(txmodule_label, sizeof(txmodule_label), "TX %u", dac_index + tx->index);

    // Create an empty group box with generate title
    txmodule_frm = frame_with_table_create(txmodule_label);

    // Create a vertical layout
    QVBoxLayout *vbox=new QVBoxLayout;

    // Append dds mode chooser frame as first widget to vertical layout
    vbox->addWidget(gui_dds_mode_chooser_create(tx));

    // Append channel i frame as second widget to vertical layout
    vbox->addWidget(gui_channel_create(&tx->ch_i));

    vbox->addStretch(1);

    if (tx->ch_q.type != CHAR_MAX)
        // Append channel q frame as third widget to vertical layout
        vbox->addWidget(gui_channel_create(&tx->ch_q));

    // Set above verical layout as tx groupbox layout
    txmodule_frm->setLayout(vbox);

    // Set the tx created groupbox layout as the tx item frame
    tx->frame = txmodule_frm;

    // Return created tx groupbox
    return txmodule_frm;
}

/**
 * @brief gui_dac_create
 * @param ddac
 * @return
 */
QGroupBox *dacDataManager::gui_dac_create(struct dds_dac *ddac)
{
    QGroupBox *dac_frm;
    gchar *frm_title;
    guint i;

    if (!ddac->iio_dac)
        return NULL;

    frm_title = g_strdup_printf("%s", ddac->name);
    dac_frm = frame_with_table_create(frm_title);

    QHBoxLayout *hbox=new QHBoxLayout;

    for (i = 0; i < ddac->tx_count; i++)
        hbox->addWidget(gui_tx_create(&ddac->txs[i]));

    hbox->addStretch(1);
    dac_frm->setLayout(hbox);

    ddac->frame = dac_frm;

    return dac_frm;
}

/**
 * @brief dacDataManager::gui_dac_channels_tree_create
 * @param d_buffer
 * @return
 */
QScrollArea *dacDataManager::gui_dac_channels_tree_create(struct dac_buffer *d_buffer)
{
    QTreeWidget *treeview= new QTreeWidget;;
    QScrollArea *scrolled_window= new QScrollArea();

    scrolled_window->setMinimumHeight(150);
    scrolled_window->setMaximumHeight(150);
    scrolled_window->setWidget(treeview);

    struct iio_device *dac = d_buffer->dac_with_scanelems;

    unsigned int i;

    for (i = 0; i < iio_device_get_channels_count(dac); i++) {
        struct iio_channel *ch = iio_device_get_channel(dac, i);

        if (!iio_channel_is_scan_element(ch))
            continue;

        QTreeWidgetItem* item=new  QTreeWidgetItem();

        // Set channel text and checkbox
        item->setText (0,iio_channel_get_id(ch));
        item->setFlags (item->flags ()|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);

        if(iio_channel_is_enabled(ch))
            item->setCheckState (0,Qt::Checked);
        else
            item->setCheckState (0,Qt::Unchecked);

        treeview->addTopLevelItem(item);
    }

    QObject::connect(treeview,&QTreeWidget::itemClicked,[=](QTreeWidgetItem *item, int column){

        if(item->checkState(0))
            item->setCheckState(0,Qt::Unchecked);
        else
            item->setCheckState(0,Qt::Checked);

    });

    d_buffer->tx_channels_view=treeview;

    return scrolled_window;
}

/**
 * @brief dacDataManager::dac_data_manager_freq_widgets_range_update
 * @param manager
 * @param tx_sample_rate
 */
void dacDataManager::dac_data_manager_freq_widgets_range_update(struct dac_data_manager *manager, double tx_sample_rate)
{
    GSList *node;

    if (!manager)
        return;

    for (node = manager->dds_tones; node; node = g_slist_next(node))
        freq_spin_range_update((dds_tone *)node->data, tx_sample_rate);

}

/**
 * @brief freq_spin_range_update
 * @param tone
 * @param tx_sample_rate
 */
void dacDataManager::freq_spin_range_update(struct dds_tone *tone, double tx_sample_rate)
{
    QDoubleSpinBox *adj;
    gdouble val;

    adj = (QDoubleSpinBox*)(tone->freq);
    val = adj->value();
    if (fabs(val) > tx_sample_rate)
        adj->setValue(tx_sample_rate);
    adj->setMinimum(-1 * tx_sample_rate);
    adj->setMaximum(tx_sample_rate);
}

/**
 * @brief dacDataManager::dac_channels_assign
 * @param ddac
 * @return
 */

int dacDataManager::dac_channels_assign(dds_dac *ddac)
{
    struct dac_data_manager *manager;
    struct iio_device *dac = ddac->iio_dac;
    char *ch_name;
    unsigned int i, processed_ch = 0;

    if (!dac)
        return 0;

    manager = ddac->parent;

    for (i = 0; i < iio_device_get_channels_count(dac); i++) {

        struct iio_channel *chn = iio_device_get_channel(dac, i);
        ch_name = get_tone_name(chn);

        if (!ch_name || strlen(ch_name) == 0)
        {
            freeChannel(ch_name);
            continue;
        }

        int tx_index;
        char ch_type;
        int tone_index;

        char *s;

        if (!(s = strstr(ch_name, "TX")))
            freeChannel(ch_name);

        tx_index = atoi(&s[2]);

        if ((s = strstr(ch_name, "_I_")))
            ch_type = I_CHANNEL;
        else if ((s = strstr(ch_name, "_Q_")))
            ch_type = Q_CHANNEL;
        else
        {
            freeChannel(ch_name);
            continue;
        }
        if (!(s = strstr(ch_name, "_F")))
        {
            freeChannel(ch_name);
            continue;
        }
        tone_index = s[2] - '0';

        struct dds_tx *tx = &ddac->txs[tx_index - 1]; /* Index extracted from name starts from 1 */

        struct dds_tone *matching_tone = NULL;
        if (ch_type == I_CHANNEL) {
            if (tone_index == 1)
                matching_tone = &tx->ch_i.t1;
            else if (tone_index == 2)
                matching_tone = &tx->ch_i.t2;
        } else if (ch_type == Q_CHANNEL) {
            if (tone_index == 1)
                matching_tone = &tx->ch_q.t1;
            else if (tone_index == 2)
                matching_tone = &tx->ch_q.t2;
        }

        if (!matching_tone)
        {
            freeChannel(ch_name);
            continue;
        }

        g_free(ch_name);

        matching_tone->iio_dac = dac;
        matching_tone->iio_ch = chn;
        manager->dds_tones = g_slist_prepend(manager->dds_tones, matching_tone);

        processed_ch++;

    }

    if (processed_ch != ddac->tones_count)
        return -1;

    return 0;
}

/**
 * @brief dacDataManager::get_tone_name
 * @param ch
 * @return
 */
char *dacDataManager::get_tone_name(iio_channel *ch)
{
    char *name;
    char tone_index;

    name = g_strdup( iio_channel_get_name(ch));

    /* If name convention "TX*_I|Q_F* is missing */
    if (name && strncmp(name, "TX", 2) != 0) {
        g_free(name);
        name = g_strdup(iio_channel_get_id(ch));
        if (name && !strncmp(name, TONE_ID, TONE_ID_SIZE)) {
            tone_index = name[TONE_ID_SIZE];
            if (tone_index && g_ascii_isdigit(tone_index))
                name = build_default_channel_name_from_index(tone_index - '0');
            else
                name = NULL;
        } else {
            name = NULL;
        }
    }

    return name;
}

#pragma endregion }

#pragma region Dac buffer output {

#pragma region Analyze wave file {


/**
 * @brief parse_wavefile_line
 * @param line
 * @param vals
 * @param max_num_vals
 * @return
 */
int dacDataManager::parse_wavefile_line(const char *line, double *vals,
                                        unsigned int max_num_vals)
{
    unsigned int n = 0;
    gchar *endptr;

    while (n < max_num_vals) {
        /* Skip white space */
        while (*line == ' ' || *line == '\t' || *line == ',')
            line++;

        if (*line == '\n' || *line == '\r' || *line == '\0')
            break;

        vals[n++] = g_ascii_strtod(line, &endptr);

        /*
         * If endptr did not advance this means the value is not a
         * number. In that case abort and return an error.
         */
        if (errno || line == (const char *)endptr)
            return -1;
        line = (const char *)endptr;
    }

    return n;
}

/**
 * @brief dac_offset_get_value
 * @param dac
 * @return
 */
double dacDataManager::dac_offset_get_value(struct iio_device *dac)
{
    double offset;
    const char *dev_name;

    if (!dac)
        return 0.0;

    offset = 0.0;
    dev_name = iio_device_get_name(dac);
    if (!strcmp(dev_name, "cf-ad9122-core-lpc"))
        offset = 32767.0;

    return offset;
}

/**
 * Fill empty channels with copies of other channels
 * E.g. (data, NULL, NULL, NULL) becomes (data, data, data, data)
 * E.g. (data1, data2, NULL, NULL) becomes (data1, data2, data1, data2)
 * ...
 */
void dacDataManager::replicate_tx_data_channels(struct _complex_ref *data, int count)
{
    if (!data)
        return;

    if (count > 2)
        replicate_tx_data_channels(data, count / 2);

    int i, half = count / 2;

    /* Check if the second half of the array needs to be filled */
    if (data[half].re == NULL || data[half].im == NULL) {
        for (i = 0; i < half; i++) {
            data[half + i].re = data[i].re;
            data[half + i].im = data[i].im;
        }
    }
}

/**
 * @brief dacDataManager::analyse_wavefile
 * @param manager
 * @param file_name
 * @param buf
 * @param count
 * @param tx_channels
 * @param full_scale
 * @return
 */
int dacDataManager::analyse_wavefile(struct dac_data_manager *manager,
                                     const char *file_name, char **buf, int *count, int tx_channels, double full_scale)
{
    int ret, rep;
    unsigned int size, j, i = 0;
    double max = 0.0, val[8], scale = 0.0;
    double offset;
    char line[80];
    mat_t *matfp;
    matvar_t **matvars;


    FILE *infile = fopen(file_name, "r");

    *buf = NULL;

    if (infile == NULL)
        return -errno;

    offset = dac_offset_get_value(manager->dac1.iio_dac);

    if (fgets(line, 80, infile) != NULL) {
        if (strncmp(line, "TEXT", 4) == 0) {
            /* Unscaled samples need to be in the range +- 2047 */
            if (strncmp(line, "TEXTU", 5) == 0)
                scale = 16.0;	/* scale up to 16-bit */
            ret = sscanf(line, "TEXT%*c REPEAT %d", &rep);
            if (ret != 1) {
                rep = 1;
            }
            size = 0;
            while (fgets(line, 80, infile)) {
                ret = parse_wavefile_line(line, val, 8);
                if (ret == 0)
                    continue;
                if (!(ret == 4 || ret == 2 || ret == 8)) {
                    fclose(infile);
                    fprintf(stderr, "ERROR: No 2, 4 or 8 columns of data inside the text file\n");
                    return WAVEFORM_TXT_INVALID_FORMAT;
                }

                for (i = 0; i < (unsigned int) ret; i++)
                    if (fabs(val[i]) > max)
                        max = fabs(val[i]);

                size += tx_channels * 2;
            }

            size *= rep;
            if (scale == 0.0)
                scale = 32767.0 * full_scale / max;

            while ((size % manager->alignment) != 0)
                size *= 2;

            *buf =(char *) malloc(size);
            if (*buf == NULL)
                return -errno;

            unsigned short *sample_16 = *((unsigned short **) buf);

            rewind(infile);

            if (fgets(line, 80, infile) != NULL) {
                if (strncmp(line, "TEXT", 4) == 0) {
                    int n;
                    size = 0;
                    i = 0;
                    while (fgets(line, 80, infile)) {
                        ret = parse_wavefile_line(line, val, 8);
                        if (ret == 0)
                            continue;

                        for (j = 0; j < (unsigned int) rep; j++) {
                            for (n = 0; n < tx_channels; n++)
                                sample_16[i++] = convert(scale, val[n & (ret - 1)], offset);

                            size += tx_channels * 2;
                        }
                    }
                }
            }

            /* When we are in 1 TX mode it is possible that the number of bytes
             * is not a multiple of 8, but only a multiple of 4. In this case
             * we'll send the same buffer twice to make sure that it becomes a
             * multiple of 8. (default manager->alignment)
             */

            while ((size % manager->alignment) != 0) {
                memcpy(*buf + size, *buf, size);
                size += size;
            }

            fclose(infile);
            *count = size;

        } else {
            fclose(infile);
            ret = 0;
            /* Is it a MATLAB file?
             * http://na-wiki.csc.kth.se/mediawiki/index.php/MatIO
             */
            matfp = Mat_Open(file_name, MAT_ACC_RDONLY);
            if (matfp == NULL) {
                fprintf(stderr, "ERROR: Could not open %s as a matlab file\n", file_name);
                return WAVEFORM_MAT_INVALID_FORMAT;
            }

            bool complex_format = false;
            bool real_format = false;

            rep = 0;
            matvars =(matvar_t **) malloc(sizeof(matvar_t *) * tx_channels);

            while (rep < tx_channels && (matvars[rep] = Mat_VarReadNextInfo(matfp)) != NULL) {
                /* must be a vector */
                if (matvars[rep]->rank !=2 || (matvars[rep]->dims[0] > 1 && matvars[rep]->dims[1] > 1)) {
                    fprintf(stderr, "ERROR: Data inside the matlab file must be a vector\n");
                    free(matvars);
                    return WAVEFORM_MAT_INVALID_FORMAT;
                }
                /* should be a double */
                if (matvars[rep]->class_type != MAT_C_DOUBLE) {
                    fprintf(stderr, "ERROR: Data inside the matlab file must be of type double\n");
                    free(matvars);
                    return WAVEFORM_MAT_INVALID_FORMAT;
                }
                /*
    printf("%s : %s\n", __func__, matvars[rep]->name);
    printf("  rank %d\n", matvars[rep]->rank);
    printf("  dims %d x %d\n", matvars[rep]->dims[0], matvars[rep]->dims[1]);
    printf("  data %d\n", matvars[rep]->data_type);
    printf("  class %d\n", matvars[rep]->class_type);
*/
                Mat_VarReadDataAll(matfp, matvars[rep]);

                if (matvars[rep]->isComplex) {
                    mat_complex_split_t *complex_data =(mat_complex_split_t *) matvars[rep]->data;
                    double *re, *im;
                    re = (double *)complex_data->Re;
                    im = (double *)complex_data->Im;

                    for (j = 0; j < (unsigned int) matvars[rep]->dims[0] ; j++) {
                        if (fabs(re[j]) > max)
                            max = fabs(re[j]);
                        if (fabs(im[j]) > max)
                            max = fabs(im[j]);
                    }
                    complex_format = true;
                } else {
                    double re;

                    for (j = 0; j < (unsigned int) matvars[rep]->dims[0] ; j++) {
                        re = ((double *)matvars[rep]->data)[j];
                        if (fabs(re) > max)
                            max = fabs(re);
                    }
                    real_format = true;
                }
                rep++;
            }
            rep--;

            //	printf("read %i vars, length %i, max value %f\n", rep, matvars[rep]->dims[0], max);

            if (rep < 0) {
                fprintf(stderr, "ERROR: Could not find any valid data in %s\n", file_name);
                free(matvars);
                return WAVEFORM_MAT_INVALID_FORMAT;
            }

            if (max <= 1.0)
                max = 1.0;

            scale = 32767.0 * full_scale / max;

            size = matvars[0]->dims[0];

            for (i = 0; i <= (unsigned int) rep; i++) {
                if (size != (unsigned int) matvars[i]->dims[0]) {
                    fprintf(stderr, "ERROR: Vector dimensions in the matlab file don't match\n");
                    free(matvars);
                    return WAVEFORM_MAT_INVALID_FORMAT;
                }
            }

            if (complex_format && real_format) {
                fprintf(stderr, "ERROR: Both complex and real data formats in the same matlab file are not supported\n");
                free(matvars);
                return WAVEFORM_MAT_INVALID_FORMAT;
            }

            *buf =(char *) malloc((size + 1) * tx_channels * 2);

            if (*buf == NULL) {
                free(matvars);
                return -errno;
            }

            *count = size * tx_channels * 2;

            unsigned long long *sample = *((unsigned long long **) buf);
            unsigned int *sample_32 = *((unsigned int **) buf);
            unsigned short *sample_16 = *((unsigned short **) buf);

            struct _complex_ref tx_data[4] = {{NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}};
            mat_complex_split_t *complex_data[4];

            if (complex_format) {
                for (i = 0; i <= (unsigned int) rep; i++) {
                    complex_data[i] =(mat_complex_split_t *)matvars[i]->data;
                    tx_data[i].re =(double*) complex_data[i]->Re;
                    tx_data[i].im =(double*) complex_data[i]->Im;
                }
            } else if (real_format) {
                for (i = 0; i <= (unsigned int) rep; i++) {
                    if (i % 2)
                        tx_data[i / 2].im =(double*) matvars[i]->data;
                    else
                        tx_data[i / 2].re =(double*) matvars[i]->data;
                }
            }
            replicate_tx_data_channels(tx_data, tx_channels);

            switch (tx_channels) {
            case 1:
                for (i = 0 ; i < size; i++) {
                    sample_16[i] = convert(scale, tx_data[0].re[i], offset);
                }
                break;
            case 2:
                for (i = 0 ; i < size; i++) {
                    sample_32[i] = ((unsigned int) convert(scale, tx_data[0].im[i], offset) << 16) |
                            ((unsigned int) convert(scale, tx_data[0].re[i], offset) << 0);
                }
                break;
            case 4:
                for (i = 0 ; i < size; i++) {
                    sample[i] = ((unsigned long long) convert(scale, tx_data[1].im[i], offset) << 48) |
                            ((unsigned long long) convert(scale, tx_data[1].re[i], offset) << 32) |
                            ((unsigned long long) convert(scale, tx_data[0].im[i], offset) << 16) |
                            ((unsigned long long) convert(scale, tx_data[0].re[i], offset) << 0);
                }
                break;
            case 8:
                for (i = 0, j = 0; i < size; i++) {
                    sample[j++] = ((unsigned long long) convert(scale, tx_data[3].im[i], offset) << 48) |
                            ((unsigned long long) convert(scale, tx_data[3].re[i], offset) << 32) |
                            ((unsigned long long) convert(scale, tx_data[2].im[i], offset) << 16) |
                            ((unsigned long long) convert(scale, tx_data[2].re[i], offset) << 0);
                    sample[j++] = ((unsigned long long) convert(scale, tx_data[1].im[i], offset) << 48) |
                            ((unsigned long long) convert(scale, tx_data[1].re[i], offset) << 32) |
                            ((unsigned long long) convert(scale, tx_data[0].im[i], offset) << 16) |
                            ((unsigned long long) convert(scale, tx_data[0].re[i], offset) << 0);
                }
                break;
            }

            for (j = 0; j <= (unsigned int) rep; j++) {
                Mat_VarFree(matvars[j]);
            }
            free(matvars);
            Mat_Close(matfp);
            return ret;
        }
    } else {
        fclose(infile);
        return -EINVAL;
    }
    return 0;
}


#pragma endregion }

/**
 * @brief dacDataManager::scale_spin_button_output_cb
 * @param data
 * @return
 */
gboolean dacDataManager::scale_spin_button_output_cb(QDoubleSpinBox *spin,gpointer data)
{
    gchar *text;
    float value;

    value = spin->value();
    if (value > spin->minimum())
        text = data ? g_strdup_printf("%1.1f dB", value) :
                      g_strdup_printf("%d dB", (int) value);
    else
        text = g_strdup_printf("-Inf dB");

    //    spin->setValue(), text);

    g_free(text);

    return TRUE;
}

/**
 * @brief tx_enabled_channels_count
 * @param treeview
 * @param enabled_mask
 * @return
 */
int dacDataManager::tx_enabled_channels_count(QTreeWidget *treeview, unsigned *enabled_mask)
{
    int num_enabled = 0;
    int ch_pos = 0;

    if (enabled_mask)
        *enabled_mask = 0;

    for(int i=0;i<treeview->topLevelItemCount();i++)
    {
        if(treeview->topLevelItem(i)->checkState(0)==Qt::Checked)
        {
            num_enabled ++;

            if (enabled_mask)
                *enabled_mask |= 1 << ch_pos;
        }
        ch_pos++;
    }

    return num_enabled;
}

/**
 * @brief dacDataManager::db_full_scale_convert
 * @param value
 * @param inverse
 * @return
 */
double dacDataManager::db_full_scale_convert(double value, bool inverse)
{
    if (inverse) {
        if (value == 0)
            return -DBL_MAX;
        return (int)((20 * log10(value)) - 0.5);
    } else {
        if (value == SCALE_MINUS_INFINITE)
            return 0;
        return pow(10, value / 20.0);
    }
}

/**
 * @brief dacDataManager::enable_dds_channels
 * @param db
 */
void dacDataManager::enable_dds_channels(struct dac_buffer *db)
{
    QTreeWidget *treeview = db->tx_channels_view;
    gboolean enabled;
    gint ch_index = 0;

    for(int i=0;i<treeview->topLevelItemCount();i++)
    {
        enabled=treeview->topLevelItem(i)->checkState(0);

        struct iio_channel *channel = iio_device_get_channel(db->dac_with_scanelems, ch_index++);

        if (enabled)
            iio_channel_enable(channel);
        else
            iio_channel_disable(channel);
    }
}

/**
 * @brief dacDataManager::enable_dds
 * @param manager
 * @param on_off
 */
void dacDataManager::enable_dds(struct dac_data_manager *manager, bool on_off)
{
    struct iio_device *dac1 = NULL;
    struct iio_device *dac2 = NULL;
    int ret;

    if (on_off == manager->dds_activated && !manager->dds_disabled)
        return;
    manager->dds_activated = on_off;

    if (manager->dds_buffer) {
        iio_buffer_destroy(manager->dds_buffer);
        manager->dds_buffer = NULL;
    }

    dac1 = manager->dac1.iio_dac;
    if (manager->dacs_count == 2)
        dac2 = manager->dac2.iio_dac;

    ret = iio_channel_attr_write_bool(iio_device_find_channel(dac1, "altvoltage0", true), "raw", on_off);
    if (ret < 0) {
        fprintf(stderr, "Failed to toggle DDS: %d\n", ret);
        return;
    }
    if (dac2) {
        ret = iio_channel_attr_write_bool(iio_device_find_channel(dac2, "altvoltage0", true), "raw", on_off);
        if (ret < 0) {
            fprintf(stderr, "Failed to toggle DDS: %d\n", ret);
            return;
        }
    }
}

/**
 * @brief dacDataManager::process_dac_buffer_file
 * @param manager
 * @param file_name
 * @param stat_msg
 * @return
 */
int dacDataManager::process_dac_buffer_file (struct dac_data_manager *manager, const char *file_name, char **stat_msg)
{
    int ret, size = 0, s_size;
    double scale;
    /*
    struct stat st;
    */
    char *buf = NULL, *tmp;
    /*
    FILE *infile;
    */
    unsigned int buffer_channels = 0;

    if (manager->dds_buffer) {
        iio_buffer_destroy(manager->dds_buffer);
        manager->dds_buffer = NULL;
    }

    if (manager->is_local) {
#ifdef __linux__
        unsigned int major, minor;
        struct utsname uts;

        uname(&uts);
        sscanf(uts.release, "%u.%u", &major, &minor);
        if (major < 2 || (major == 3 && minor < 14)) {
            if (manager->dacs_count == 2)
                buffer_channels = 8;
            else if (manager->dac1.tx_count == 2)
                buffer_channels = 4;
            else
                buffer_channels = 2;
        } else {
            buffer_channels = tx_enabled_channels_count(manager->dac_buffer_module.tx_channels_view, NULL);
        }
#endif
    } else {
        buffer_channels = tx_enabled_channels_count(manager->dac_buffer_module.tx_channels_view, NULL);
    }


    if (g_str_has_suffix(file_name, ".bin")) {
        FILE *infile;
        struct stat st;

        /* Assume Binary format */
        stat(file_name, &st);
        buf = (char *)malloc(st.st_size);
        if (buf == NULL) {
            if (stat_msg)
                *stat_msg = g_strdup_printf("Internal memory allocation failed.");
            return -errno;
        }
        infile = fopen(file_name, "r");
        size = fread(buf, 1, st.st_size, infile);
        fclose(infile);
    } else {

        scale = db_full_scale_convert(manager->dac_buffer_module.scale->value(), false);
        ret = analyse_wavefile(manager, file_name, &buf, &size, buffer_channels, scale);
        if (ret < 0) {
            if (stat_msg)
                *stat_msg = g_strdup_printf("Error while parsing file: %s.", strerror(-ret));
            free(buf);
            return ret;
        } else if (ret > 0) {
            if (stat_msg)
                *stat_msg = g_strdup_printf("Invalid data format");
            free(buf);
            return -EINVAL;
        }
    }

    usleep(1000); /* FIXME: Temp Workaround needs some investigation */

    enable_dds(manager, false);
    enable_dds_channels(&manager->dac_buffer_module);

    struct iio_device *dac = manager->dac_buffer_module.dac_with_scanelems;

    s_size = iio_device_get_sample_size(dac);
    if (!s_size) {
        fprintf(stderr, "Unable to create buffer due to sample size");
        if (stat_msg)
            *stat_msg = g_strdup_printf("Unable to create buffer due to sample size");
        free(buf);
        return -EINVAL;
    }

    if (size % manager->alignment != 0 || size % s_size != 0) {
        fprintf(stderr, "Unable to create buffer due to sample size and number of samples");
        if (stat_msg)
            *stat_msg = g_strdup_printf("Unable to create buffer due to sample size and number of samples");
        free(buf);
        return -EINVAL;
    }

    //    size /= s_size;
    manager->dds_buffer = iio_device_create_buffer(dac, size / s_size, true);

    if (!manager->dds_buffer) {
        fprintf(stderr, "Unable to create buffer: %s\n", strerror(errno));
        if (stat_msg)
            *stat_msg = g_strdup_printf("Unable to create iio buffer: %s", strerror(errno));
        free(buf);
        return -errno;
    }

    //    qInfo()<<"Buff Size = "<<(int*)iio_buffer_end(manager->dds_buffer) - (int*)iio_buffer_start(manager->dds_buffer)<< " Correct size = "<<size;

    memcpy(iio_buffer_start(manager->dds_buffer), buf,size);
    //           ((int*)iio_buffer_end(manager->dds_buffer) - (int*)iio_buffer_start(manager->dds_buffer))*buffer_channels);

    iio_buffer_push(manager->dds_buffer);

    free(buf);

    tmp = strdup(file_name);

    if (manager->dac_buffer_module.dac_buf_filename)
        *manager->dac_buffer_module.dac_buf_filename="";

    *manager->dac_buffer_module.dac_buf_filename = tmp;

    if (stat_msg)
    {
        *stat_msg = g_strdup_printf("Waveform loaded successfully.");
    }

    return 0;
}

/**
 * @brief When user clicked on Load button
 * @param dbuf
 */
void dacDataManager::waveform_load_button_clicked_cb (struct dac_buffer *dbuf)
{
    const QByteArray filenameBytes = (*dbuf->dac_buf_filename).toLocal8Bit();
    const gchar *filename = filenameBytes.constData();
    gchar *status_msg;

    if(strcmp(filename,"")==0)
        return;

    if (!filename || g_str_has_suffix(filename, "(null)")) {
        status_msg = g_strdup_printf("No file selected.");
    } else if (!g_str_has_suffix(filename, ".txt") && !g_str_has_suffix(filename, ".mat") && !g_str_has_suffix(filename, ".bin")) {
        status_msg = g_strdup_printf("Invalid file type. Please select a .txt, .bin or .mat file.");
    } else if (!tx_channels_check_valid_setup(dbuf)) {
        status_msg = g_strdup_printf("Invalid channel selection.");
    } else {
        process_dac_buffer_file(dbuf->parent, (const char *)filename, &status_msg);
    }

    dbuf->load_status_buf->setPlainText(status_msg);
    g_free(status_msg);
}

/**
 * @brief When a file was selected then update dac_buffer properties
 * @param fileName
 * @param dbuf
 */
void dacDataManager::dac_buffer_config_file_set_cb(QString fileName, struct dac_buffer &dbuf)
{
    *dbuf.dac_buf_filename = fileName;
    dbuf.load_status_buf->setPlainText(fileName);
}

/**
 * @brief Create Dac Buffer GUI
 * @param d_buffer
 * @return
 */
QGroupBox *dacDataManager::gui_dac_buffer_create(struct dac_buffer *d_buffer)
{
    QGroupBox *dacbuf_frame;
    QGroupBox *fchooser_frame;
    QPushButton *fchooser_btn;
    QPushButton *fileload_btn;
    QTextEdit *load_status_txt;
    QLabel *scale;
    QGroupBox *tx_channels_frame;

    // Dac Buffer layout
    QVBoxLayout *vBox=new QVBoxLayout;

    dacbuf_frame = frame_with_table_create("DAC Buffer Settings");

    // File Selection layout
    QVBoxLayout *vBoxFileSelection=new QVBoxLayout;
    QHBoxLayout *hBoxFileSelection1=new QHBoxLayout;
    QHBoxLayout *hBoxFileSelection2=new QHBoxLayout;
    QHBoxLayout *hBoxFileSelection3=new QHBoxLayout;

    fchooser_btn = new QPushButton("Select a File");
    fileload_btn = new QPushButton("Load");
    load_status_txt = new QTextEdit("");
    fchooser_frame = frame_with_table_create("File Selection");
    load_status_txt->setReadOnly(true);
    load_status_txt->setEnabled(false);

    hBoxFileSelection1->addWidget(fchooser_btn);
    hBoxFileSelection1->addWidget(load_status_txt);
    vBoxFileSelection->addItem(hBoxFileSelection1);

    hBoxFileSelection2->addStretch();
    hBoxFileSelection2->addWidget(fileload_btn);
    vBoxFileSelection->addItem(hBoxFileSelection2);

    scale = new QLabel("Scale(dBFS):");

    d_buffer->scale = new QDoubleSpinBox();
    d_buffer->scale->setMinimumHeight(40);
    d_buffer->scale->setMaximumHeight(40);
    d_buffer->scale->setMinimum(-91.0);
    d_buffer->scale->setMaximum(0.0);
    d_buffer->scale->setSingleStep(1.0);
    d_buffer->scale->setDecimals(2);
    d_buffer->scale->setValue(0);

    hBoxFileSelection3->addWidget(scale);
    hBoxFileSelection3->addStretch(1);
    hBoxFileSelection3->addWidget(d_buffer->scale);

    vBoxFileSelection->addItem(hBoxFileSelection3);
    vBoxFileSelection->addStretch(1);

    fchooser_frame->setLayout(vBoxFileSelection);

    tx_channels_frame = frame_with_table_create("DAC Channels");

    QScrollArea *channels_scrolled_view = gui_dac_channels_tree_create(d_buffer);
    QVBoxLayout *vBoxDacChannels=new QVBoxLayout();

    vBoxDacChannels->addWidget(channels_scrolled_view);
    vBoxDacChannels->addStretch(1);
    tx_channels_frame->setLayout(vBoxDacChannels);

    vBox->addWidget(fchooser_frame);
    vBox->addWidget(tx_channels_frame);

    vBox->addStretch(1);

    dacbuf_frame->setLayout(vBox);

    d_buffer->frame = dacbuf_frame;
    d_buffer->load_status_buf = load_status_txt;
    d_buffer->dac_buf_filename = new QString("");
    //    d_buffer->tx_channels_view =channels_scrolled_view->widget();
    d_buffer->buffer_fchooser_btn = fchooser_btn;


    QObject::connect(fchooser_btn,&QPushButton::clicked,[=](){
        QString fileName= QFileDialog::getOpenFileName(nullptr,
                                                       tr("Find Files"), QDir::currentPath());

        dac_buffer_config_file_set_cb(fileName,*d_buffer);

        QDir::setCurrent(fileName.remove(QUrl(fileName).fileName()));

    });

    QObject::connect(fileload_btn,&QPushButton::clicked,[=](){
        waveform_load_button_clicked_cb(d_buffer);
    });

    QObject::connect(d_buffer->scale,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [=](){
        scale_spin_button_output_cb(d_buffer->scale,(void*) 1);
    });

    return dacbuf_frame;
}

#pragma endregion }

#pragma region Others {

/**
 * @brief freeChannel
 * @param ch_name
 */
void dacDataManager::freeChannel(char *ch_name){
    if (ch_name)
    {
        g_free(ch_name);
    }
}

/**
 * @brief dacDataManager::build_default_channel_name_from_index
 * @param ch_index
 * @return
 */
char *dacDataManager::build_default_channel_name_from_index(guint ch_index)
{
    guint tx_index = (ch_index / TX_NB_TONES) + 1; /* TX couting starts from 1 (not 0) */
    guint tone_index = (ch_index % 2) + 1; /* There are always 2 tones (I/Q). Count starts at 1 */
    const char i_q_type = (ch_index & 0x02) ? Q_CHANNEL : I_CHANNEL; /* First two indexes are I, next two are Q and so on*/

    return g_strdup_printf("TX%u_%c_F%u", tx_index, i_q_type, tone_index);
}

/**
 * @brief get_adi_part_code
 * @param device_name
 * @return
 */
const char * dacDataManager::get_adi_part_code(const char *device_name)
{
    const char *ad = NULL;

    if (!device_name)
        return NULL;
    ad = strstr(device_name, "ad");
    if (!ad || strlen(ad) < strlen("adxxxx"))
        return NULL;
    if (g_ascii_isdigit(ad[2]) &&
            g_ascii_isdigit(ad[3]) &&
            g_ascii_isdigit(ad[4]) &&
            g_ascii_isdigit(ad[5])) {
        return ad;
    }

    return NULL;
}

/**
 * @brief dma_valid_selection
 * @param device
 * @param mask
 * @param channel_count
 * @return
 */
bool dacDataManager::dma_valid_selection(const char *device, unsigned mask, unsigned channel_count)
{
    static const unsigned long eight_channel_masks[] = {
        0x01, 0x02, 0x04, 0x08, 0x03, 0x0C, /* 1 & 2 chan */
        0x10, 0x20, 0x40, 0x80, 0x30, 0xC0, /* 1 & 2 chan */
        0x33, 0xCC, 0xC3, 0x3C, 0x0F, 0xF0, /* 4 chan */
        0xFF,                               /* 8chan */
        0x00
    };
    static const unsigned long four_channel_masks[] = {
        0x01, 0x02, 0x04, 0x08, 0x03, 0x0C, 0x0F,
        0x00
    };
    bool ret = true;
    unsigned int i;

    device = get_adi_part_code(device);
    if (!device)
        return true;

    for (i = 0; i < DMA_DEVICES_COUNT; i++) {
        if (!strncmp(device, dma_devices[i], strlen(dma_devices[i])))
            break;
    }

    /* Skip validation for devices that are not in the list */
    if (i == DMA_DEVICES_COUNT)
        return true;

    if (channel_count == 8) {
        ret = false;
        for (i = 0;  i < sizeof(eight_channel_masks) / sizeof(eight_channel_masks[0]); i++)
            if (mask == eight_channel_masks[i])
                return true;
    } else if (channel_count == 4) {
        ret = false;
        for (i = 0;  i < sizeof(four_channel_masks) / sizeof(four_channel_masks[0]); i++)
            if (mask == four_channel_masks[i])
                return true;
    }

    return ret;
}

/**
 * @brief tx_channels_check_valid_setup
 * @param dbuf
 * @return
 */
bool dacDataManager::tx_channels_check_valid_setup(struct dac_buffer *dbuf)
{
    struct iio_device *dac = dbuf->dac_with_scanelems;
    int enabled_channels;
    unsigned mask;

    enabled_channels = tx_enabled_channels_count(dbuf->tx_channels_view,&mask);

    return (dma_valid_selection(iio_device_get_name(dac) ?: iio_device_get_id(dac),
                                mask, dbuf->scan_elements_count) && enabled_channels > 0);
}

/**
 * @brief convert
 * @param scale
 * @param val
 * @param offset
 * @return
 */
unsigned short dacDataManager::convert(double scale, float val, double offset)
{
    return (short) (val * scale + offset);
}

/**
 * @brief dacDataManager::compare_gain
 * @param a
 * @param b
 * @return
 */
int dacDataManager::compare_gain(const char *a, const char *b)
{
    double val_a, val_b;
    sscanf(a, "%lf", &val_a);
    sscanf(b, "%lf", &val_b);

    if (val_a < val_b)
        return -1;
    else if(val_a > val_b)
        return 1;
    else
        return 0;
}

/**
 * @brief dacDataManager::save_scale_widget_value
 * @param data
 */
void dacDataManager::save_scale_widget_value(void *data)
{
    struct dds_tone *tone =(struct dds_tone *) data;
    struct dds_channel *dds_ch = tone->parent;
    struct iio_widget *scale_w = &tone->iio_scale;
    struct iio_widget *scale_pair_w = (tone->number == 1) ? &dds_ch->t2.iio_scale : &dds_ch->t1.iio_scale;
    double old_val, val1, val2;

    val1 = db_full_scale_convert(((QDoubleSpinBox)(scale_w->widget)).value(), false);
    iio_channel_attr_read_double(scale_w->chn, scale_w->attr_name, &old_val);
    iio_channel_attr_read_double(scale_pair_w->chn, scale_pair_w->attr_name, &val2);

    if (val1 + val2 > 1)
        ((QDoubleSpinBox)scale_w->widget).setValue(db_full_scale_convert(old_val, true));

    scale_w->save(scale_w);
}

/**
 * @brief dacDataManager::dds_scale_set_value
 * @param scale
 * @param value
 */
void dacDataManager::dds_scale_set_value(QWidget *scale, gdouble value)
{
    QString className=scale->metaObject()->className();
    if (className=="QComboBox") {
        ((QComboBox*)(scale))->setCurrentIndex((gint)value);
    }
    if (className=="QDoubleSpinBox") {
        ((QDoubleSpinBox*)(scale))->setValue(value);
    }
}

/**
 * @brief dacDataManager::dds_scale_get_value
 * @param scale
 * @return
 */
double dacDataManager::dds_scale_get_value(QWidget *scale)
{
    QString className=scale->metaObject()->className();

    if (className=="QComboBox") {
        return ((gint)((QComboBox*)scale)->currentIndex());
    } else if (className=="QDoubleSpinBox") {
        return ((QDoubleSpinBox*)(scale))->value();
    }

    return 0;
}

/**
 * @brief dacDataManager::dds_locked_scale_cb
 * @param tx
 */
void dacDataManager::dds_locked_scale_cb(struct dds_tx *tx)
{
    struct dds_tone **tones = tx->dds_tones;

    if (tx->parent->tones_count == 2) /* No I-Q available */
        return;

    gdouble scale1 = dds_scale_get_value(tones[TX_T1_I]->scale);
    gdouble scale2 = dds_scale_get_value(tones[TX_T2_I]->scale);

    switch (((QComboBox*)(tx->dds_mode_widget))->currentIndex()) {
    case DDS_ONE_TONE:
        dds_scale_set_value(tones[TX_T1_I + 2]->scale, scale1);
        break;
    case DDS_TWO_TONE:
        dds_scale_set_value(tones[TX_T1_I + 2]->scale, scale1);
        dds_scale_set_value(tones[TX_T2_I + 2]->scale, scale2);
        break;
    default:
        break;
    }
}

/**
 * @brief dacDataManager::dds_locked_phase_cb
 * @param tx
 */
void dacDataManager::dds_locked_phase_cb(struct dds_tx *tx)
{
    struct dds_tone **tones = tx->dds_tones;

    if (tx->parent->tones_count == 2) /* No I-Q available */
        return;

    gdouble phase1 = ((QDoubleSpinBox*)(tones[TX_T1_I]->phase))->value();
    gdouble phase2 = ((QDoubleSpinBox*)(tones[TX_T2_I]->phase))->value();

    gdouble freq1 = ((QDoubleSpinBox*)(tones[TX_T1_I]->freq))->value();
    gdouble freq2 = ((QDoubleSpinBox*)(tones[TX_T2_I]->freq))->value();

    gdouble inc1, inc2;

    if (freq1 >= 0)
        inc1 = 90.0;
    else
        inc1 = 270;

    if ((phase1 - inc1) < 0)
        phase1 += 360;

    switch (((QComboBox*)(tx->dds_mode_widget))->currentIndex()) {
    case DDS_ONE_TONE:
        ((QDoubleSpinBox*)(tones[TX_T1_I + 2]->phase))->setValue(phase1 - inc1);
        break;
    case DDS_TWO_TONE:
        if (freq2 >= 0)
            inc2 = 90;
        else
            inc2 = 270;
        if ((phase2 - inc2) < 0)
            phase2 += 360;

        ((QDoubleSpinBox*)(tones[TX_T1_I + 2]->phase))->setValue(phase1 - inc1);
        ((QDoubleSpinBox*)(tones[TX_T2_I + 2]->phase))->setValue(phase2 - inc2);

        break;
    default:
        printf("%s: error\n", __func__);
        break;
    }
}

void dacDataManager::dds_locked_freq_cb(struct dds_tx *tx)
{
    struct dds_tone **tones = tx->dds_tones;

    if (tx->parent->tones_count == 2) /* No I-Q available */
        return;

    gdouble freq1 = ((QDoubleSpinBox*)(tones[TX_T1_I]->freq))->value();
    gdouble freq2 = ((QDoubleSpinBox*)(tones[TX_T2_I]->freq))->value();

    switch (((QComboBox*)(tx->dds_mode_widget))->currentIndex()) {
    case DDS_ONE_TONE:
        ((QDoubleSpinBox*)(tones[TX_T1_I + 2]->freq))->setValue(freq1);
        break;
    case DDS_TWO_TONE:
        ((QDoubleSpinBox*)(tones[TX_T1_I + 2]->freq))->setValue(freq1);
        ((QDoubleSpinBox*)(tones[TX_T2_I + 2]->freq))->setValue(freq2);
        break;
    default:
        //            printf("%s: error : %i\n", __func__,
        //                   ((QComboBox*)(tx->dds_mode_widget))->currentText());
        break;
    }

    dds_locked_phase_cb(tx);
}

void dacDataManager::manage_dds_mode(QComboBox *box,struct dds_tx *tx)
{
    struct dac_data_manager *manager;
    guint active;
    double min_scale;
    bool scale_available_mode;
    bool q_tone_exists;
    unsigned tones_count;
    unsigned i;

    if (!box)
        return;

    manager = tx->parent->parent;
    tones_count = manager->dac1.tones_count;
    q_tone_exists = (tones_count > 2);
    min_scale = manager->lowest_scale_point;
    scale_available_mode = manager->scale_available_mode;

    active = box->currentIndex();
    if (active != DDS_BUFFER) {
        struct dds_tx *txs = manager->dac1.txs;
        for (i = 0; i < manager->dac1.tx_count; i++) {
            QWidget *widget = txs[i].dds_mode_widget;
            if (((QComboBox*)(widget))->currentIndex() == DDS_BUFFER) {
                ((QComboBox*)(widget))->setCurrentIndex(active);
                manage_dds_mode((QComboBox*)(widget), &txs[i]);
            }
        }
        txs = manager->dac2.txs;
        for (i = 0; i < manager->dac2.tx_count; i++) {
            QComboBox *widget = (QComboBox *)txs[i].dds_mode_widget;
            if (((QComboBox*)(widget))->currentIndex() == DDS_BUFFER) {
                ((QComboBox*)(widget))->setCurrentIndex(active);
                manage_dds_mode((QComboBox*)(widget), &txs[i]);
            }
        }
    }

    struct dds_tone **tones = tx->dds_tones;

    switch (active) {
    case DDS_DISABLED:
    {
        for (i = TX_T1_I; i <= TX_T2_Q; i++) {
            if (i >= tones_count)
                break;
            struct dds_tone *tone = tones[i];
            QDoubleSpinBox *scale_w = tone->scale;

            if (dds_scale_get_value(scale_w) != min_scale) {
                tone->scale_state = dds_scale_get_value(scale_w);
                dds_scale_set_value(scale_w, min_scale);
            }
        }

        bool start_dds = false;
        GSList *node;

        for (node = manager->dds_tones; node; node = g_slist_next(node)) {
            struct dds_tone *tn =(struct dds_tone *)node->data;
            if (dds_scale_get_value(tn->scale) != min_scale) {
                start_dds = true;
                break;
            }
        }

        if (!manager->dds_activated && manager->dds_buffer) {
            iio_buffer_destroy(manager->dds_buffer);
            manager->dds_buffer = NULL;
        }
        manager->dds_disabled = true;
        enable_dds(manager, start_dds);

        tx->ch_i.frame->hide();
        if (q_tone_exists)
            tx->ch_q.frame->hide();
        manager->dac_buffer_module.frame->hide();
    }
        break;
    case DDS_ONE_TONE:
    {
        enable_dds(manager, true);
        tx->ch_i.frame->setTitle("Single Tone");
        tx->ch_i.frame->show();
        tx->ch_i.t2.frame->hide();
        if (q_tone_exists)
            tx->ch_q.frame->hide();
        manager->dac_buffer_module.frame->hide();

        if (dds_scale_get_value(tones[TX_T1_I]->scale) == min_scale) {
            dds_scale_set_value(tones[TX_T1_I]->scale, tones[TX_T1_I]->scale_state);
            if (q_tone_exists)
                dds_scale_set_value(tones[TX_T1_Q]->scale, tones[TX_T1_Q]->scale_state);
        }

        if (dds_scale_get_value(tones[TX_T2_I]->scale) != min_scale) {
            tones[TX_T2_I]->scale_state = dds_scale_get_value(tones[TX_T2_I]->scale);
            dds_scale_set_value(tones[TX_T2_I]->scale, min_scale);
            if (q_tone_exists) {
                tones[TX_T2_Q]->scale_state = dds_scale_get_value(tones[TX_T2_Q]->scale);
                dds_scale_set_value(tones[TX_T2_Q]->scale, min_scale);
            }
        }

        /* Connect the widgets that are showing */
        if (!tones[TX_T1_I]->dds_scale_hid) {
            if (scale_available_mode) {
                tones[TX_T1_I]->dds_scale_hid =1;
                QObject::connect(tones[TX_T1_I]->scale,
                                 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                 [=](){
                    dds_locked_scale_cb(tx);
                });
            } else {
                tones[TX_T1_I]->dds_scale_hid =1;
                QObject::connect(tones[TX_T1_I]->scale,
                                 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                 [=](){
                    dds_locked_scale_cb(tx);
                });
            }
        }

        //1
        if (!tones[TX_T1_I]->dds_freq_hid)
        {
            tones[TX_T1_I]->dds_freq_hid =1;

            QObject::connect(tones[TX_T1_I]->freq, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=]()
            {
                dds_locked_freq_cb(tx);
            });

        }

        if (!tones[TX_T1_I]->dds_phase_hid) {
            tones[TX_T1_I]->dds_phase_hid =1;
            QObject::connect(tones[TX_T1_I]->phase,
                             static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             [=](){
                dds_locked_phase_cb(tx);
            });
        }

        /* Disconnect the rest */
        if (tones[TX_T2_I]->dds_scale_hid) {
            //            QObject::disconnect();
            //            g_signal_handler_disconnect(tones[TX_T2_I]->scale, tones[TX_T2_I]->dds_scale_hid);
            tones[TX_T2_I]->dds_scale_hid = 0;
        }
        if (tones[TX_T2_I]->dds_freq_hid) {
            //            g_signal_handler_disconnect(tones[TX_T2_I]->freq, tones[TX_T2_I]->dds_freq_hid);
            tones[TX_T2_I]->dds_freq_hid = 0;
        }
        if (tones[TX_T2_I]->dds_phase_hid) {
            //            g_signal_handler_disconnect(tones[TX_T2_I]->phase, tones[TX_T2_I]->dds_phase_hid);
            tones[TX_T2_I]->dds_phase_hid = 0;
        }

        /* Force sync */
        dds_locked_scale_cb(tx);
        dds_locked_freq_cb(tx);
        dds_locked_phase_cb(tx);
    }
        break;
    case DDS_TWO_TONE:
    {
        enable_dds(manager, true);

        tx->ch_i.frame->setTitle("Two Tones");

        tx->ch_i.frame->show();
        tx->ch_i.t2.frame->show();

        if (q_tone_exists)
            tx->ch_q.frame->hide();

        manager->dac_buffer_module.frame->hide();

        for (i = TX_T1_I; i <= TX_T2_Q; i++) {
            if (i >= tones_count)
                break;
            if (dds_scale_get_value(tones[i]->scale) == min_scale) {
                dds_scale_set_value(tones[i]->scale, tones[i]->scale_state);
            }
        }

        for (i = TX_T1_I; i <= TX_T2_Q; i++) {
            if (i >= tones_count)
                break;
            if (!tones[i]->dds_scale_hid) {
                if (scale_available_mode) {
                    tones[i]->dds_scale_hid =1;
                    QObject::connect(tones[i]->scale,
                                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                     [=](){
                        dds_locked_scale_cb(tx);
                    });
                } else {
                    tones[i]->dds_scale_hid =1;
                    QObject::connect(tones[i]->scale,
                                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                     [=](){
                        dds_locked_scale_cb(tx);
                    });
                }
            }

            //2
            if (!tones[i]->dds_freq_hid)
            {
                tones[i]->dds_freq_hid =1;
                QObject::connect(tones[i]->freq, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=]()
                {
                    dds_locked_freq_cb(tx);
                });
            }

            if (!tones[i]->dds_phase_hid)
            {   tones[i]->dds_phase_hid =1;
                QObject::connect(tones[i]->phase,
                                 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                                 [=](){
                    dds_locked_phase_cb(tx);
                });
            }
        }

        /* Force sync */
        dds_locked_scale_cb(tx);
        dds_locked_freq_cb(tx);
        dds_locked_phase_cb(tx);
    }
        break;
    case DDS_INDEPDENT:
    {
        /* Independent/Individual control */
        enable_dds(manager, true);

        tx->ch_i.frame->setTitle("Channel I");

        tx->ch_i.frame->show();
        tx->ch_i.t2.frame->show();
        if (q_tone_exists)
            tx->ch_q.frame->show();
        manager->dac_buffer_module.frame->hide();

        for (i = TX_T1_I; i <= TX_T2_Q; i++) {
            if (i >= tones_count)
                break;
            if (dds_scale_get_value(tones[i]->scale) == min_scale)
                dds_scale_set_value(tones[i]->scale, tones[i]->scale_state);

            if (tones[i]->dds_scale_hid) {
                //                g_signal_handler_disconnect(tones[i]->scale, tones[i]->dds_scale_hid);
                tones[i]->dds_scale_hid = 0;
            }
            if (tones[i]->dds_freq_hid) {
                //                g_signal_handler_disconnect(tones[i]->freq, tones[i]->dds_freq_hid);
                tones[i]->dds_freq_hid = 0;
            }
            if (tones[i]->dds_phase_hid) {
                //                g_signal_handler_disconnect(tones[i]->phase, tones[i]->dds_phase_hid);
                tones[i]->dds_phase_hid = 0;
            }
        }

    }
        break;
    case DDS_BUFFER:
    {
        if ((manager->dds_activated || manager->dds_disabled) && *manager->dac_buffer_module.dac_buf_filename!="") {
            manager->dds_disabled = false;
        }

        tx->ch_i.frame->hide();
        if (q_tone_exists)
            tx->ch_q.frame->hide();
        manager->dac_buffer_module.frame->show();

        for (i = 0; i < manager->dac1.tx_count; i++)
            ((QComboBox*)(manager->dac1.txs[i].dds_mode_widget))->setCurrentIndex(DDS_BUFFER);
        for (i = 0; i < manager->dac2.tx_count; i++)
            ((QComboBox*)(manager->dac2.txs[i].dds_mode_widget))->setCurrentIndex(DDS_BUFFER);
    }
        break;
    default:
        break;
    }
}

void dacDataManager::tone_setup(struct dds_tone *tone)
{
    bool combobox_scales = tone->parent->parent->parent->parent->scale_available_mode;

    IIO_Widget iio_w;

    /* Bind the IIO Channel attributes to the GUI widgets */
    iio_w.iio_spin_button_s64_init(&tone->iio_freq,
                                   tone->iio_dac, tone->iio_ch, "frequency", tone->freq, &abs_mhz_scale);
    iio_w.iio_spin_button_add_progress(&tone->iio_freq);

    if (combobox_scales) {
        iio_w.iio_combo_box_init(&tone->iio_scale, tone->iio_dac, tone->iio_ch, "scale",
                                 "scale_available", tone->scale, compare_gain);
    } else {
        iio_w.iio_spin_button_init(&tone->iio_scale,
                                   tone->iio_dac, tone->iio_ch, "scale", tone->scale, NULL);
        iio_w.iio_spin_button_set_convert_function(&tone->iio_scale, db_full_scale_convert);
        iio_w.iio_spin_button_add_progress(&tone->iio_scale);
        iio_w.iio_spin_button_set_on_complete_function(&tone->iio_scale,
                                                       save_scale_widget_value, tone);
        iio_w.iio_spin_button_skip_save_on_complete(&tone->iio_scale, TRUE);
    }

    iio_w.iio_spin_button_init(&tone->iio_phase,
                               tone->iio_dac, tone->iio_ch, "phase", tone->phase, &khz_scale);
    iio_w.iio_spin_button_add_progress(&tone->iio_phase);

    /* Signals connect */
    iio_w.iio_spin_button_progress_activate(&tone->iio_freq);
    iio_w.iio_spin_button_progress_activate(&tone->iio_phase);
    if (combobox_scales) {
        //        g_signal_connect(tone->scale, "changed",
        //                         G_CALLBACK(save_widget_value), &tone->iio_scale);
    } else {
        //        g_signal_connect(tone->scale, "output",
        //                         G_CALLBACK(scale_spin_button_output_cb), NULL);
        iio_w.iio_spin_button_progress_activate(&tone->iio_scale);
    }

}

void dacDataManager::manager_iio_setup(dac_data_manager *manager)
{
    GSList *node;
    guint i;

    for (node = manager->dds_tones; node; node = g_slist_next(node))
        tone_setup((struct dds_tone *)node->data);

    for (i = 0; i < manager->dac1.tx_count; i++)
        QObject::connect((QComboBox*)manager->dac1.txs[i].dds_mode_widget,
                         static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                         [=](){
            manage_dds_mode((QComboBox*)manager->dac1.txs[i].dds_mode_widget,&manager->dac1.txs[i]);
        }
        );

    for (i = 0; i < manager->dac2.tx_count; i++)
        QObject::connect((QComboBox*)manager->dac2.txs[i].dds_mode_widget,
                         static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                         [=](){
            manage_dds_mode((QComboBox*)manager->dac2.txs[i].dds_mode_widget,&manager->dac2.txs[i]);
        }
        );

    for (node = manager->dds_tones; node; node = g_slist_next(node)) {
        struct dds_tone *tn =(struct dds_tone *) node->data;

        tn->scale_state = dds_scale_get_value(tn->scale);
    }

    struct dds_tone *tone = &manager->dac1.txs[0].ch_i.t1;

    if (manager->scale_available_mode) {
        QWidget *scale_cmb = tone->scale;
        gint active;

        active = (gint)tone->scale_state;
        while (dds_scale_get_value(scale_cmb) >= 0) {
            active++;
            dds_scale_set_value(scale_cmb, active);
        }
        manager->lowest_scale_point = active - 1;
        dds_scale_set_value(scale_cmb, tone->scale_state);

    } else {
        QDoubleSpinBox *scale_btn = (QDoubleSpinBox *)(manager->dac1.txs[0].ch_i.t1.scale);

        manager->lowest_scale_point = scale_btn->minimum();
    }
}

void dacDataManager::dac_data_manager_free(dac_data_manager *manager)
{
    if (manager) {
        free(manager->dac1.txs);
        free(manager->dac2.txs);
        if (manager->dds_buffer) {
            iio_buffer_destroy(manager->dds_buffer);
            manager->dds_buffer = NULL;
        }
        g_slist_free(manager->dds_tones);
        free(manager);
    }
}

void dacDataManager::dac_buffer_init(dac_data_manager *manager, dac_buffer *d_buffer)
{
    int count;
    struct iio_device *dac_with_scanelems;

    dac_with_scanelems = manager->dac1.iio_dac;
    count = device_scan_elements_count(dac_with_scanelems);
    if (manager->dacs_count == 2 && count == 0) {
        dac_with_scanelems = manager->dac2.iio_dac;
        count = device_scan_elements_count(dac_with_scanelems);
    }

    d_buffer->parent = manager;
    d_buffer->scan_elements_count = count;
    d_buffer->dac_with_scanelems = dac_with_scanelems;
}

int dacDataManager::device_scan_elements_count(iio_device *dev)
{
    unsigned int i;
    int count;

    for (i = 0, count = 0; i < iio_device_get_channels_count(dev); i++) {
        struct iio_channel *ch = iio_device_get_channel(dev, i);

        if (iio_channel_is_scan_element(ch))
            count++;
    }

    return count;
}

unsigned dacDataManager::get_iio_tones_count(iio_device *dev)
{
    unsigned int i, count;

    for (i = 0, count = 0; i < iio_device_get_channels_count(dev); i++) {
        struct iio_channel *chn = iio_device_get_channel(dev, i);
        char *name = get_tone_name(chn);

        if (name && strncmp(name, "TX", 2) == 0)
            count++;

        g_free(name);
    }

    return count;
}

void dacDataManager::dds_tx_init(dds_dac *ddac, dds_tx *tx, unsigned dds_index)
{
    tx->index = dds_index;
    tx->ch_i.type = I_CHANNEL;
    tx->ch_i.t1.number = 1;
    tx->ch_i.t2.number = 2;
    tx->ch_q.type = Q_CHANNEL;
    tx->ch_q.t1.number = 1;
    tx->ch_q.t2.number = 2;

    tx->parent = ddac;
    tx->ch_i.parent = tx;
    tx->ch_q.parent = tx;
    tx->ch_i.t1.parent = &tx->ch_i;
    tx->ch_i.t2.parent = &tx->ch_i;
    tx->ch_q.t1.parent = &tx->ch_q;
    tx->ch_q.t2.parent = &tx->ch_q;

    tx->dds_tones[0] = &tx->ch_i.t1;
    tx->dds_tones[1] = &tx->ch_i.t2;
    tx->dds_tones[2] = &tx->ch_q.t1;
    tx->dds_tones[3] = &tx->ch_q.t2;

    ddac->tx_count++;
}

void dacDataManager::dac_data_manager_set_buffer_size_alignment(dac_data_manager *manager, unsigned align)
{
    if (!manager)
        return;

    /*
         * This is just a backup in case the alignment was not reported by the
         * hardware itself. If it was reported by the hardware ignore this
         * value.
         */
    if (!manager->hw_reported_alignment)
        manager->alignment = align;
}

int dacDataManager::manager_channels_assign(dac_data_manager *manager)
{
    int ret;

    ret = dac_channels_assign(&manager->dac1);
    if (ret < 0)
        return ret;
    ret = dac_channels_assign(&manager->dac2);
    if (ret < 0)
        return ret;

    return 0;
}

void dacDataManager::dds_tone_iio_widgets_update(struct dds_tone *tone)
{
    tone->iio_freq.update(&tone->iio_freq);
    tone->iio_scale.update(&tone->iio_scale);
    tone->iio_phase.update(&tone->iio_phase);
}

void dacDataManager::dac_data_manager_update_iio_widgets(dac_data_manager *manager)
{
    GSList *node;
    unsigned i;

    if (!manager)
        return;

    for (node = manager->dds_tones; node; node = g_slist_next(node))
        dds_tone_iio_widgets_update((struct dds_tone *)node->data);

    for (i = 0; i < manager->dac1.tx_count; i++)
        manage_dds_mode((QComboBox*)(manager->dac1.txs[i].dds_mode_widget), &manager->dac1.txs[i]);
    for (i = 0; i < manager->dac2.tx_count; i++)
        manage_dds_mode((QComboBox*)(manager->dac2.txs[i].dds_mode_widget), &manager->dac2.txs[i]);
}

QHBoxLayout *dacDataManager::dac_data_manager_get_gui_container(dac_data_manager *manager)
{
    if (!manager)
        return NULL;

    return manager->container;
}

#pragma endregion }

#pragma region Interface {

/**
 * @brief load a file to card
 * @param fileName : file to load on card
 * @param scale : scale value
 */
QString dacDataManager::setFile(QString fileName,double scale)
{
//     Set dds Mode to Disable
        DisableDac();

//        // Set dds Mode to dac buffer output
//        manager->dac1.txs[0].dds_mode_widget->setCurrentIndex(4);

//        // Set scale value
//        manager->dac_buffer_module.scale->setValue(scale);

//        // Set all channel to checked state
//        QTreeWidget *treeview =manager->dac_buffer_module.tx_channels_view;
//        for(int i=0;i<treeview->topLevelItemCount();i++)
//            treeview->topLevelItem(i)->setCheckState(0,Qt::CheckState::Checked);

//        // Set current file name
//        dac_buffer_config_file_set_cb(fileName,manager->dac_buffer_module);

//        // Load file to card
//        waveform_load_button_clicked_cb(&manager->dac_buffer_module);

//        return manager->dac_buffer_module.load_status_buf->toPlainText();

    return "";
}

/**
 * @brief dacDataManager::DisableDac
 */
void dacDataManager::DisableDac()
{
    // Set dds Mode to Disable
    //    manager->dac1.txs[0].dds_mode_widget->setCurrentIndex(0);
    //    manager->dac1.txs[1].dds_mode_widget->setCurrentIndex(0);

}

void dacDataManager::getData(bool val)
{
    int i{};

}

#pragma endregion }
