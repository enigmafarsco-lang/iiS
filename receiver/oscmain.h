#ifndef OSCMAIN_H
#define OSCMAIN_H

#pragma region Include {

#include <QMainWindow>
#include <QWidget>

#include <iio.h>
#include <receiver/oscpreferences.h>
#include <receiver/plot.h>
#include <receiver/config.h>
#include <QTabWidget>
#include <QCoreApplication>
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
#include <QApplication>
#include <receiver/connectdialog.h>
#include <receiver/plugins/adrv9009.h>
#include <receiver/plugins/adrv9009_adv.h>
#include <receiver/plugins/controlunitadrv9009.h>
#include <receiver/utils/filedialog.h>
#include <QFileDialog>
#include <QMessageBox>

#pragma endregion }

#define DEFAULT_PROFILE_NAME ".osc_profile.ini"
#define OSC_INI_SECTION "IIO Oscilloscope"
#define CAPTURE_INI_SECTION OSC_INI_SECTION " - Capture Window"

//#define TIME_PLOT 0
//#define FFT_PLOT 1

struct osc_plugin;

namespace Ui {
class OSCMain;
}

class OSCMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit OSCMain(QApplication *app=nullptr,QWidget *parent = nullptr);



    bool ctx_destroyed_by_do_quit=false;

    int load_default_profile(char *filename, bool load_plugins);
    void do_init();
    void create_default_plot(void);
    Plot* new_plot_cb();
    void plot_init(Plot *plot, QString mode);
    void application_quit();
    void init_device_list(iio_context *ctx);

    //saeid raziani
    adrv9009            * _adrv9009;
    adrv9009_adv        * _adrv9009adv;
    ControlUnitADRV9009 * _adrv9009controlUnit;

    //
    bool rx_update_device_sampling_freq(iio_context *ctx,const char *device, double freq);
    bool is_input_device(const struct iio_device *dev);
    bool device_type_get(const struct iio_device *dev, int type);

    void osc_plot_destroy (OscPlot *plot);
    //saeid raziani

    static double read_sampling_frequency(const struct iio_device *dev);
    static inline int osc_iio_device_get_trigger(const struct iio_device *dev,
                                                 const struct iio_device **trigger);


    Plot * createMultiPlot(QString);


    ~OSCMain();

public slots:
    void on_actionNew_Plot_triggered();

private slots:
    void on_actionQuit_triggered();
    void on_actionConnect_triggered();
//    void on_actionNew_Plot_triggered();
    void on_actionLoad_Save_Profile_triggered();

private:
    Ui::OSCMain *ui;
    QApplication *mainApp;
    QStringList Devices;


    static int load_profile_sequential(const char *filename);
    static int capture_profile_handler(int line, const char *section,
                                       const char *name, const char *value);
    bool check_inifile(const char *filepath);
    static int load_profile(const char *filename, bool load_plugins);
    static void plugin_restore_ini_state(const char *plugin_name,const char *attribute, int value);
    void load_plugins();
    static OscPreferences *aggregate_osc_preferences_from_plugins(GSList *plist);
    static void plugins_get_preferred_size(GSList *plist, int *width, int *height);
    static gchar *get_default_profile_name(void);
    static gint plugin_names_cmp(gconstpointer a, gconstpointer b);
    void osc_plugin_context_free_resources(struct osc_plugin_context *ctx);
    static bool force_plugin(const char *name);
    static void load_plugin_finish(struct osc_plugin *plugin);
    static void plugin_make_detachable(struct detachable_plugin *d_plugin);

    void gfunc_destroy_plot(gpointer data, gpointer user_data);
    static void gfunc_close_plot(gpointer data, gpointer user_data);
    osc_plugin *get_plugin_from_name(const char *name);

    int load_profile_sequential_handler(int line, const char *section, const char *name, const char *value);

    int handle_osc_param(int line, const char *name, const char *value);
    void ClearTabContainer(void);
    void AddPlugin(QString plugin);

signals:
    void quitApp();
    void hoppingSignal(bool,double min,double max,double step,int delay);
    void frqHasFocusedSignal();
    void adrv9009Signal();
    //saeid raziani
    void frqDomainIsSelectedSignal();
    void seekIsSelectedSignal();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // OSCMAIN_H
