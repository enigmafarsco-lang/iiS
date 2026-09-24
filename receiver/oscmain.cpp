#include "oscmain.h"
#include "ui_oscmain.h"

#include <receiver/plugins/ad9371.h>
#include <receiver/plugins/ad9371_adv.h>
#include <receiver/plugins/controlunitadrv9009.h>

#pragma region Properties {

//static unsigned int num_devices = 0;
static GList *plot_list = NULL;
GSList *plugin_list = NULL;
//static gboolean stop_capture;
static GSList *dplugin_list = NULL;
static struct osc_plugin *spect_analyzer_plugin = NULL;
static OscPreferences *osc_preferences = NULL;

libini2 libini2;

#pragma endregion }

#pragma region Constructor {


OSCMain::OSCMain(QApplication *app,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OSCMain)
{
    ui->setupUi(this);


    mainApp=app;

    ClearTabContainer();
}

OSCMain::~OSCMain()
{
    delete ui;
}

#pragma endregion }

#pragma region Events {

void OSCMain::closeEvent(QCloseEvent *event)
{
    globals::status=false;
    application_quit();
}

void OSCMain::application_quit()
{
    mainApp->quit();
}

#pragma endregion }

#pragma region Menu {

void OSCMain::on_actionQuit_triggered()
{
    QApplication::quit();
}

void OSCMain::on_actionConnect_triggered()
{
    connectDialog *connect_dialog=new connectDialog();
    connect_dialog->exec();
}

void OSCMain::on_actionNew_Plot_triggered()
{
    Plot * plot = new Plot(mainApp);
    plot->osc_plot_new();
    plot->show();
}

void OSCMain::on_actionLoad_Save_Profile_triggered()
{
    //    FileDialog* fileDialog=new FileDialog(this);
    //    fileDialog->show();

    // getExistingDirectory
    //    QString dir=QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home",
    //                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QString dir=QFileDialog::getOpenFileName(this,tr("Open File"),"/home",
                                             tr("Document (*.xml *.pdf)"));
}

#pragma endregion }

#pragma region Plot {

Plot * OSCMain::new_plot_cb()
{
    Plot * new_plot;

    Plot * oscPlot=new Plot(mainApp);

    if (osc_preferences && osc_preferences->plot_preferences)
        new_plot = oscPlot->osc_plot_new_with_pref(osc_preferences->plot_preferences);
    else
        new_plot = oscPlot->osc_plot_new();

    oscPlot->osc_plot_set_visible(new_plot, true);

    plot_init(new_plot,"");

    //    on_actionNew_Plot_triggered();

    return new_plot;

}

void OSCMain::plot_init(Plot *plot, QString mode)
{
    plot_list = g_list_append(plot_list, plot);

    connect(plot,&Plot::HoppingSignal,[=](bool status,double min,double max,double step,int delay){
        emit hoppingSignal(status,min,max,step,delay);
    });


    // this connnection is for sending a message to adrv in order to activating smart noise
//    connect(_adrv9009controlUnit, &ControlUnitADRV9009::sendFileToDacSignal, [&]{ _adrv9009->setFile(spotPath,0);});


    //saeid raziani
    if(mode == FRQ_DOMAIN)
    {
        //these connections are for emitting when frq domain and seekin button is clicked in order to ploting them
        connect(plot, &Plot::frqDomainIsSelectedSignal, this,[&]{emit frqDomainIsSelectedSignal();});
        connect(plot, &Plot::seekIsSelectedSignal, this,[&]{
//            _adrv9009adv->chk_trigger_mode->setChecked(true);
            emit seekIsSelectedSignal();});


        connect(_adrv9009,&adrv9009::frqValueChangedSignal, plot, &Plot:: frqValueChangedSlot);
        _adrv9009->tx_lo_freq->setValue(_adrv9009->tx_lo_freq->value()+1);
        _adrv9009->tx_lo_freq->setValue(_adrv9009->tx_lo_freq->value()-1);
    }
    //    g_signal_connect(plot, "osc-capture-event", G_CALLBACK(start), NULL);
    //    g_signal_connect(plot, "osc-destroy-event", G_CALLBACK(plot_destroyed_cb), NULL);
    //    g_signal_connect(plot, "osc-newplot-event", G_CALLBACK(new_plot_created_cb), NULL);
    //    osc_plot_set_quit_callback(OSC_PLOT(plot), (void (*)(void *))application_quit, NULL);
//    plot->show();
}

void OSCMain::create_default_plot(void)
{
    if (globals::ctx && !!iio_context_get_devices_count(globals::ctx) &&
            g_list_length(plot_list) == 0) {
        new_plot_cb();
    }
}

#pragma endregion }

#pragma region Profile {

int OSCMain::load_profile(const char *filename, bool load_plugins)
{
    int ret = 0;

    GSList *node;
    gint x_pos = 0, y_pos = 0;
    char *value;

    value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, "remote_ip_addr");
    /* IP addresses specified on the command line via the -c option
         * override profile settings.
         */
    if (value && !(globals::ctx && !strcmp(iio_context_get_name(globals::ctx), "network"))) {
        struct iio_context *new_ctx = iio_create_network_context(value);
        //        if (new_ctx) {
        //            application_reload(new_ctx, false);
        //        } else {
        //            fprintf(stderr, "Failed connecting to remote device: %s\n", value);
        //            /* Abort parsing the rest of the profile as there is
        //                 * probably a lot of device specific stuff in it.
        //                 */
        //            free(value);
        //            return 0;
        //        }
        free(value);
    }

    value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, "uri");
    /* URI addresses specified on the command line via the -u option
         * override profile settings
         */
    if (value && !(globals::ctx && (!strcmp(iio_context_get_name(globals::ctx), "uri")))) {
        struct iio_context *new_ctx;
        struct iio_scan_context *ctxs = iio_create_scan_context(NULL, 0);
        struct iio_context_info **info;
        char *pid_vid = value;
        char *serial = strchr(value, ' ');
        const char *tmp;
        int i;

        if (!serial)
            goto nope;
        //usb_set_serialnumber(value);

        pid_vid[serial - pid_vid] = 0;
        serial++;

        if (!ctxs)
            goto nope;
        ret = iio_scan_context_get_info_list(ctxs, &info);
        if (ret < 0)
            goto nope_ctxs;
        if (!ret)
            goto nope_list;

        for (i = 0; i < ret; i++) {
            tmp = iio_context_info_get_description(info[i]);
            /* find the correct PID/VID plus serial number*/
            if (strstr(tmp, pid_vid) && strstr(tmp, serial)) {
                new_ctx = iio_create_context_from_uri(
                            iio_context_info_get_uri(info[i]));
                //                if (new_ctx) {
                //                    application_reload(new_ctx, false);
                //                    break;
                //                } else {
                //                    fprintf(stderr, "Failed connecting to uri: %s\n", value);
                //                    free(value);
                //                    return 0;
                //                }
            }

        }

nope_list:
        iio_context_info_list_free(info);
nope_ctxs:
        iio_scan_context_destroy(ctxs);
nope:
        free(value);
        ret = 0;
    }

    value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, "test");
    if (value) {
        free(value);
        return 0;
    }

    value = libini2.read_token_from_ini(filename,
                                        OSC_INI_SECTION, "tooltips_enable");

    if (value) {
        //        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(tooltips_en),
        //                !!atoi(value));
        free(value);
    }

    value = libini2.read_token_from_ini(filename,
                                        OSC_INI_SECTION, "startup_version_check");

    if (value) {
        //        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(versioncheck_en),
        //                !!atoi(value));
        free(value);
    }

    value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, "window_x_pos");
    if (value) {
        x_pos = atoi(value);
        free(value);
    }

    value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, "window_y_pos");
    if (value) {
        y_pos = atoi(value);
        free(value);
    }

    for (node = plugin_list; node; node = g_slist_next(node)) {
        struct osc_plugin *plugin =(osc_plugin*) node->data;
        char buf[1024];

        if (load_plugins && plugin->load_profile)
            plugin->load_profile(plugin, filename);

        snprintf(buf, sizeof(buf), "plugin.%s.detached", plugin->name);
        value = libini2.read_token_from_ini(filename, OSC_INI_SECTION, buf);
        if (!value)
            continue;


        //plugin_restore_ini_state(plugin->name, "detached", !!atoi(value));
        free(value);
    }

    return ret;
}

int OSCMain::load_default_profile(char *filename, bool load_plugins)
{
    int ret = 0;

    /* Don't load anything */
    if (filename && !strcmp(filename, "-"))
        return 0;

    if (filename && check_inifile(filename)) {
        ret = load_profile(filename, load_plugins);
    } else {
        gchar *path = get_default_profile_name();

        /* if this is bad, we don't load anything and
                 * return success, so we still run */
        if (check_inifile(path))
            ret = load_profile(path, load_plugins);

        g_free(path);
    }

    return ret;
}

gchar *OSCMain::get_default_profile_name()
{
    return g_build_filename(
                getenv("HOME") ?: getenv("LOCALAPPDATA"),
                DEFAULT_PROFILE_NAME, NULL);
}

#pragma endregion }

#pragma region Plugins {

void OSCMain::ClearTabContainer(void)
{
    ui->tab_container->clear();
}

struct osc_plugin * OSCMain::get_plugin_from_name(const char *name)
{
    GSList *node;

    for (node = plugin_list; node; node = g_slist_next(node)) {
        struct osc_plugin *plugin = (osc_plugin *)node->data;
    }

    return NULL;
}

OscPreferences *OSCMain::aggregate_osc_preferences_from_plugins(GSList *plist)
{
    /* TO DO: implement this function to combine preferences from all
        plugins. If two preferences are conflicting, signal that there is an
        error in the design of the plugins and return NULL, otherwise return
        the merged preferences from all plugins. For now we just get the first
        preference that we encoutner. */

    GSList *node;
    struct osc_plugin *p;

    for (node = plist; node; node = g_slist_next(node)) {
        p =(osc_plugin*) node->data;
        if (p->get_preferences_for_osc) {
            return p->get_preferences_for_osc(p);
        }
    }

    return NULL;
}

void OSCMain::AddPlugin(QString plugin)
{

    if(plugin=="9009")
    {
        //saeid raziani
        _adrv9009 = new class adrv9009(mainApp);\

        ui->tab_container->addTab(_adrv9009,"ADRV9009");
        _adrv9009->init();
        connect(this,     &OSCMain::hoppingSignal,             _adrv9009,&adrv9009::startHopping);
        connect(_adrv9009,&adrv9009::fileLoadIsCompleteSignal, this,&OSCMain::adrv9009Signal);
        //saeid raziani



        _adrv9009adv = new adrv9009_adv();
        ui->tab_container->addTab(_adrv9009adv,"ADRV9009 Advance");
        _adrv9009adv->init();
        _adrv9009controlUnit = new ControlUnitADRV9009();
        ui->tab_container->addTab(_adrv9009controlUnit,"ADRV9009 Control Unit");


        //saeid raziani ==> this connection emits when frq box is focused and awars receivermain class
        //        connect(_adrv9009, &adrv9009::frqHasFocusedSignal,this, &OSCMain::frqHasFocusedSignal);


        //----------------------------------------------------------------------
        //        adrv9009 *adrv9009=new class adrv9009(mainApp);
        //        ui->tab_container->addTab(adrv9009,"ADRV9009");
        //        adrv9009->init();

        //        connect(this,&OSCMain::hoppingSignal,adrv9009,&adrv9009::startHopping);

        //        adrv9009_adv *adrv9009adv=new adrv9009_adv();
        //        ui->tab_container->addTab(adrv9009adv,"ADRV9009 Advance");
        //        adrv9009adv->init();

        //        ControlUnitADRV9009 *adrv9009controlUnit=new ControlUnitADRV9009();
        //        ui->tab_container->addTab(adrv9009controlUnit,"ADRV9009 Control Unit");
    }

    else if(plugin=="9371")
    {
        AD9371 *ad9371=new class AD9371(mainApp);
        ui->tab_container->addTab(ad9371,"AD9371");
        ad9371->init();

        ad9371_adv *ad9371adv=new ad9371_adv();
        ui->tab_container->addTab(ad9371adv,"AD9371 Advance");
        ad9371adv->init();
    }

}

void OSCMain::plugin_restore_ini_state(const char *plugin_name, const char *attribute, int value)
{
    struct detachable_plugin *dplugin;
    GSList *found_plugin;
    QWindow *plugin_window;
    QWidget *button;

    found_plugin = g_slist_find_custom(dplugin_list,
                                       (gconstpointer)plugin_name, plugin_names_cmp);
    if (found_plugin == NULL)
        return;

    dplugin =(detachable_plugin*) found_plugin->data;
    button = dplugin->detach_attach_button;

    if (!strcmp(attribute, "detached")) {
        //        if ((dplugin->detached_state) ^ (value))
        //            g_signal_emit_by_name(button, "clicked", dplugin);
    } else if (!strcmp(attribute, "x_pos")) {
        //plugin_window = GTK_WINDOW(gtk_widget_get_toplevel(button));
        if (dplugin->detached_state == true) {
            dplugin->xpos = value;
            //move_gtk_window_on_screen(plugin_window, dplugin->xpos, dplugin->ypos);
        }
    } else if (!strcmp(attribute, "y_pos")) {
        //plugin_window = GTK_WINDOW(gtk_widget_get_toplevel(button));
        if (dplugin->detached_state == true) {
            dplugin->ypos = value;
            //  move_gtk_window_on_screen(plugin_window, dplugin->xpos, dplugin->ypos);
        }
    }

}

gint OSCMain::plugin_names_cmp(gconstpointer a, gconstpointer b)
{
    struct detachable_plugin *p = (struct detachable_plugin *)a;
    char *key = (char *)b;

    return strcmp(p->plugin->name, key);
}

void OSCMain::osc_plugin_context_free_resources(osc_plugin_context *ctx)
{
    if (ctx ->plugin_name)
        g_free(ctx->plugin_name);
    if (ctx->required_devices)
        g_list_free_full(ctx->required_devices, (GDestroyNotify)g_free);
}

void OSCMain::load_plugins()
{
    ClearTabContainer();
    foreach (QString device, Devices) {

        if(device.contains("adrv9009-phy"))
        {
            AddPlugin("9009");
        }
        if(device.contains("9371-phy"))
        {
            AddPlugin("9371");
        }

    }

}

bool OSCMain::force_plugin(const char *name)
{
    const char *force_plugin = getenv("OSC_FORCE_PLUGIN");
    const char *pos;

    if (!force_plugin)
        return false;

    if (strcmp(force_plugin, "all") == 0)
        return true;

#ifdef __GLIBC__
    pos = strcasestr(force_plugin, name);
#else
    pos = strstr(force_plugin, name);
#endif
    if (pos) {
        switch (*(pos + strlen(name))) {
        case ' ':
        case '\0':
            return true;
        default:
            break;
        }
    }

    return false;
}

void OSCMain::load_plugin_finish(osc_plugin *plugin)
{
    struct detachable_plugin *d_plugin;
    gint page = 0;

    if (plugin->update_active_page) {
        plugin->update_active_page(plugin, page, FALSE);
    }

    d_plugin =(detachable_plugin *) malloc(sizeof(*d_plugin));
    d_plugin->plugin = plugin;
    dplugin_list = g_slist_append(dplugin_list, (gpointer) d_plugin);
    plugin_make_detachable(d_plugin);
}

void OSCMain::plugin_make_detachable(detachable_plugin *d_plugin)
{
    QWidget *page;
    int num_pages = 0;

    //        num_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    //        page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), num_pages - 1);

    d_plugin->window = NULL;
    d_plugin->detached_state = FALSE;
    //d_plugin->detach_attach_button = plugin_tab_add_detach_btn(page, d_plugin);
}

/* Before we really start, let's load the last saved profile */
bool OSCMain::check_inifile(const char *filepath)
{
    struct stat sts;
    FILE *fd;
    char buf[1024];
    size_t i;

    buf[1023] = '\0';

    if (stat(filepath, &sts) == -1)
        return FALSE;

    if (!S_ISREG(sts.st_mode))
        return FALSE;

    fd = fopen(filepath, "r");
    if (!fd)
        return FALSE;

    i = fread(buf, 1, sizeof(buf) - 1, fd);
    fclose(fd);

    if (i == 0 )
        return FALSE;

    if (!strstr(buf, "[" OSC_INI_SECTION "]"))
        return FALSE;

    return TRUE;
}

void OSCMain::plugins_get_preferred_size(GSList *plist, int *width, int *height)
{
    GSList *node;
    struct osc_plugin *p;
    int w, h, max_w = -1, max_h = -1;

    for (node = plist; node; node = g_slist_next(node)) {
        p = (osc_plugin*)node->data;
        if (p->get_preferred_size) {
            p->get_preferred_size(p, &w, &h);
            if (w > max_w)
                max_w = w;
            if (h > max_h)
                max_h = h;
        }
    }
    *width = max_w;
    *height = max_h;
}

#pragma endregion }

#pragma region Initialize {

void OSCMain::do_init()
{
    if(!globals::ctx)
    {
        qCritical() << "OSCMain::do_init(): IIO context is null";
        return;
    }

    init_device_list(globals::ctx);

    load_plugins();

    osc_preferences = aggregate_osc_preferences_from_plugins(plugin_list);

    int width = -1, height = -1;
    plugins_get_preferred_size(plugin_list, &width, &height);
    spect_analyzer_plugin = get_plugin_from_name("Spectrum Analyzer");
}

void OSCMain::init_device_list(iio_context*ctx)
{
    unsigned int i, j;
    Devices.clear();

    if(!ctx)
    {
        qCritical() << "OSCMain::init_device_list(): IIO context is null";
        num_devices = 0;
        return;
    }

    num_devices = iio_context_get_devices_count(ctx);

    for (i = 0; i < num_devices; i++) {
        iio_device *dev = iio_context_get_device(ctx, i);
        unsigned int nb_channels = iio_device_get_channels_count(dev);
        struct extra_dev_info *dev_info =reinterpret_cast<struct extra_dev_info *>(calloc(1, sizeof(*dev_info)));
        iio_device_set_data(dev, dev_info);
        dev_info->input_device = is_input_device(dev);

        for (j = 0; j < nb_channels; j++) {
            struct iio_channel *ch = iio_device_get_channel(dev, j);
            struct extra_info *info =(extra_info *)(calloc(10,sizeof(*info)));
            info->dev = dev;
            iio_channel_set_data(ch, info);
        }

        Devices.append(iio_device_get_name(dev));

        //        rx_update_device_sampling_freq(iio_device_get_name(dev) ?:
        //                                                                 iio_device_get_id(dev), USE_INTERN_SAMPLING_FREQ);
        rx_update_device_sampling_freq(ctx,iio_device_get_name(dev) ?:
                                                                     iio_device_get_id(dev), USE_INTERN_SAMPLING_FREQ);
    }
}

bool OSCMain::rx_update_device_sampling_freq(iio_context* ctx,const char *device, double freq)
{
    struct iio_device *dev;
    struct extra_dev_info *info;

    g_return_val_if_fail(device, false);

    dev = iio_context_find_device(ctx, device);
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

    // TODO: Add into osc plot
    //        for (node = plot_list; node; node = g_list_next(node))
    //            osc_plot_update_rx_lbl(OSC_PLOT(node->data), NORMAL_UPDATE);

    return true;
}

bool OSCMain::is_input_device(const iio_device *dev)
{
    return device_type_get(dev, 1);
}

bool OSCMain::device_type_get(const iio_device *dev, int type)
{
    struct iio_channel *ch;
    int nb_channels, i;

    if (!dev)
        return false;

    nb_channels = iio_device_get_channels_count(dev);
    for (i = 0; i < nb_channels; i++)
    {
        ch = iio_device_get_channel(dev, i);
        if (iio_channel_is_scan_element(ch) &&
                (type ? !iio_channel_is_output(ch) : iio_channel_is_output(ch)))
            return true;
    }

    return false;
}

double OSCMain::read_sampling_frequency(const iio_device *dev)
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

int OSCMain::osc_iio_device_get_trigger(const iio_device *dev, const iio_device **trigger)
{
    int ret;

    ret = iio_device_get_trigger(dev, trigger);
    if (ret == -EIO) {
        ret = 0;
        *trigger = NULL;
    }
    return ret;
}

//saeid raziani
Plot * OSCMain::createMultiPlot(QString mode)
{
    Plot * plot = new Plot(mainApp,mode);
    plot->osc_plot_new();
    plot_init(plot,mode);
    //    plot->on_btn_capture_clicked();

    return plot;
}

#pragma endregion }
