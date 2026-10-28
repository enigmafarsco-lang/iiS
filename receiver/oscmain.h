// ============================================================================
// File: oscmain.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for oscmain.h.
// ============================================================================

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
    // Constructor: Initializes OSCMain subsystem components, default parameters, and state.
    explicit OSCMain(QApplication *app=nullptr,QWidget *parent = nullptr);



    bool ctx_destroyed_by_do_quit=false;

    // Executes load default profile routine.
    int load_default_profile(char *filename, bool load_plugins);
    // Executes do init routine.
    void do_init();
    // Executes create default plot routine.
    void create_default_plot(void);
    // Executes new plot cb routine.
    Plot* new_plot_cb();
    // Executes plot init routine.
    void plot_init(Plot *plot, QString mode);
    // Executes application quit routine.
    void application_quit();
    // Initializes init device list for OSCMain.
    void init_device_list(iio_context *ctx);

    //saeid raziani
    adrv9009            * _adrv9009;
    adrv9009_adv        * _adrv9009adv;
    ControlUnitADRV9009 * _adrv9009controlUnit;

    //
    bool rx_update_device_sampling_freq(iio_context *ctx,const char *device, double freq);
    // Retrieves current input_device state.
    bool is_input_device(const struct iio_device *dev);
    // Executes device type get routine.
    bool device_type_get(const struct iio_device *dev, int type);

    // Executes osc plot destroy routine.
    void osc_plot_destroy (OscPlot *plot);
    //saeid raziani

    // Reads and parses incoming read_sampling_frequency data.
    static double read_sampling_frequency(const struct iio_device *dev);
    static inline int osc_iio_device_get_trigger(const struct iio_device *dev,
                                                 const struct iio_device **trigger);


    // Executes create multi plot routine.
    Plot * createMultiPlot(QString);


    // Destructor: Cleans up OSCMain resources, timers, and allocated memory.
    ~OSCMain();

public slots:
    // Event/Signal handler for on action new  plot triggered.
    void on_actionNew_Plot_triggered();

private slots:
    // Event/Signal handler for on action quit triggered.
    void on_actionQuit_triggered();
    // Event/Signal handler for on action connect triggered.
    void on_actionConnect_triggered();
//    void on_actionNew_Plot_triggered();
    void on_actionLoad_Save_Profile_triggered();

private:
    Ui::OSCMain *ui;
    QApplication *mainApp;
    QStringList Devices;


    // Executes load profile sequential routine.
    static int load_profile_sequential(const char *filename);
    static int capture_profile_handler(int line, const char *section,
                                       const char *name, const char *value);
    // Executes check inifile routine.
    bool check_inifile(const char *filepath);
    // Executes load profile routine.
    static int load_profile(const char *filename, bool load_plugins);
    // Executes plugin restore ini state routine.
    static void plugin_restore_ini_state(const char *plugin_name,const char *attribute, int value);
    // Executes load plugins routine.
    void load_plugins();
    static OscPreferences *aggregate_osc_preferences_from_plugins(GSList *plist);
    // Executes plugins get preferred size routine.
    static void plugins_get_preferred_size(GSList *plist, int *width, int *height);
    static gchar *get_default_profile_name(void);
    // Executes plugin names cmp routine.
    static gint plugin_names_cmp(gconstpointer a, gconstpointer b);
    // Executes osc plugin context free resources routine.
    void osc_plugin_context_free_resources(struct osc_plugin_context *ctx);
    // Executes force plugin routine.
    static bool force_plugin(const char *name);
    // Executes load plugin finish routine.
    static void load_plugin_finish(struct osc_plugin *plugin);
    // Executes plugin make detachable routine.
    static void plugin_make_detachable(struct detachable_plugin *d_plugin);

    // Executes gfunc destroy plot routine.
    void gfunc_destroy_plot(gpointer data, gpointer user_data);
    // Executes gfunc close plot routine.
    static void gfunc_close_plot(gpointer data, gpointer user_data);
    osc_plugin *get_plugin_from_name(const char *name);

    // Executes load profile sequential handler routine.
    int load_profile_sequential_handler(int line, const char *section, const char *name, const char *value);

    // Executes handle osc param routine.
    int handle_osc_param(int line, const char *name, const char *value);
    // Executes clear tab container routine.
    void ClearTabContainer(void);
    // Executes add plugin routine.
    void AddPlugin(QString plugin);

signals:
    // Executes quit app routine.
    void quitApp();
    // Executes hopping signal routine.
    void hoppingSignal(bool,double min,double max,double step,int delay);
    // Executes frq has focused signal routine.
    void frqHasFocusedSignal();
    // Executes adrv9009 signal routine.
    void adrv9009Signal();
    //saeid raziani
    void frqDomainIsSelectedSignal();
    // Executes seek is selected signal routine.
    void seekIsSelectedSignal();

    // QWidget interface
protected:
    // Executes close event routine.
    void closeEvent(QCloseEvent *event);
};

#endif // OSCMAIN_H
