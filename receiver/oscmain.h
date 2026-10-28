/**
 * @file oscmain.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class OSCMain
 * @brief High-level controller class for OSCMain.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class OSCMain;
}

/**
 * @class OSCMain
 * @brief High-level controller class for OSCMain.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class OSCMain : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new OSCMain instance and initializes subsystem components and UI state.
     * @param app Input parameter specifying app (QApplication).
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit OSCMain(QApplication *app=nullptr,QWidget *parent = nullptr);



    bool ctx_destroyed_by_do_quit=false;

    /**
     * @brief Executes the load default profile procedure for OSCMain.
     * @param filename Input parameter specifying filename (char).
     * @param load_plugins Input parameter specifying load_plugins (bool).
     * @return Output value of type int.
     */
    int load_default_profile(char *filename, bool load_plugins);
    /**
     * @brief Executes the do init procedure for OSCMain.
     */
    void do_init();
    /**
     * @brief Executes the create default plot procedure for OSCMain.
     */
    void create_default_plot(void);
    /**
     * @brief Executes the new plot cb procedure for OSCMain.
     * @return Output value of type Plot*.
     */
    Plot* new_plot_cb();
    /**
     * @brief Executes the plot init procedure for OSCMain.
     * @param plot Input parameter specifying plot (Plot).
     * @param mode Input parameter specifying mode (QString).
     */
    void plot_init(Plot *plot, QString mode);
    /**
     * @brief Executes the application quit procedure for OSCMain.
     */
    void application_quit();
    /**
     * @brief Initializes and configures the init device list for the OSCMain subsystem.
     * @param ctx Input parameter specifying ctx (iio_context).
     */
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


    /**
     * @brief Executes the create multi plot procedure for OSCMain.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type Plot *.
     */
    Plot * createMultiPlot(QString);


    /**
     * @brief Destructor for OSCMain, releasing allocated hardware resources, timers, and buffers.
     */
    ~OSCMain();

public slots:
    /**
     * @brief Slot handler responding to on action new  plot triggered event.
     */
    void on_actionNew_Plot_triggered();

private slots:
    /**
     * @brief Slot handler responding to on action quit triggered event.
     */
    void on_actionQuit_triggered();
    /**
     * @brief Slot handler responding to on action connect triggered event.
     */
    void on_actionConnect_triggered();
//    void on_actionNew_Plot_triggered();
    void on_actionLoad_Save_Profile_triggered();

private:
    Ui::OSCMain *ui;
    QApplication *mainApp;
    QStringList Devices;


    /**
     * @brief Executes the load profile sequential procedure for OSCMain.
     * @param filename Input parameter specifying filename (const char).
     * @return Output value of type static int.
     */
    static int load_profile_sequential(const char *filename);
    static int capture_profile_handler(int line, const char *section,
                                       const char *name, const char *value);
    /**
     * @brief Executes the check inifile procedure for OSCMain.
     * @param filepath Input parameter specifying filepath (const char).
     * @return Output value of type bool.
     */
    bool check_inifile(const char *filepath);
    /**
     * @brief Executes the load profile procedure for OSCMain.
     * @param filename Input parameter specifying filename (const char).
     * @param load_plugins Input parameter specifying load_plugins (bool).
     * @return Output value of type static int.
     */
    static int load_profile(const char *filename, bool load_plugins);
    /**
     * @brief Executes the plugin restore ini state procedure for OSCMain.
     * @param plugin_name Input parameter specifying plugin_name (const char).
     * @param attribute Input parameter specifying attribute (const char).
     * @param value Input parameter specifying value (int).
     * @return Output value of type static void.
     */
    static void plugin_restore_ini_state(const char *plugin_name,const char *attribute, int value);
    /**
     * @brief Executes the load plugins procedure for OSCMain.
     */
    void load_plugins();
    static OscPreferences *aggregate_osc_preferences_from_plugins(GSList *plist);
    /**
     * @brief Executes the plugins get preferred size procedure for OSCMain.
     * @param plist Input parameter specifying plist (GSList).
     * @param width Input parameter specifying width (int).
     * @param height Input parameter specifying height (int).
     * @return Output value of type static void.
     */
    static void plugins_get_preferred_size(GSList *plist, int *width, int *height);
    static gchar *get_default_profile_name(void);
    /**
     * @brief Executes the plugin names cmp procedure for OSCMain.
     * @param a Input parameter specifying a (gconstpointer).
     * @param b Input parameter specifying b (gconstpointer).
     * @return Output value of type static gint.
     */
    static gint plugin_names_cmp(gconstpointer a, gconstpointer b);
    /**
     * @brief Executes the osc plugin context free resources procedure for OSCMain.
     * @param ctx Input parameter specifying ctx (struct osc_plugin_context).
     */
    void osc_plugin_context_free_resources(struct osc_plugin_context *ctx);
    /**
     * @brief Executes the force plugin procedure for OSCMain.
     * @param name Input parameter specifying name (const char).
     * @return Output value of type static bool.
     */
    static bool force_plugin(const char *name);
    /**
     * @brief Executes the load plugin finish procedure for OSCMain.
     * @param plugin Input parameter specifying plugin (struct osc_plugin).
     * @return Output value of type static void.
     */
    static void load_plugin_finish(struct osc_plugin *plugin);
    /**
     * @brief Executes the plugin make detachable procedure for OSCMain.
     * @param d_plugin Input parameter specifying d_plugin (struct detachable_plugin).
     * @return Output value of type static void.
     */
    static void plugin_make_detachable(struct detachable_plugin *d_plugin);

    /**
     * @brief Executes the gfunc destroy plot procedure for OSCMain.
     * @param data Input parameter specifying data (gpointer).
     * @param user_data Input parameter specifying user_data (gpointer).
     */
    void gfunc_destroy_plot(gpointer data, gpointer user_data);
    /**
     * @brief Executes the gfunc close plot procedure for OSCMain.
     * @param data Input parameter specifying data (gpointer).
     * @param user_data Input parameter specifying user_data (gpointer).
     * @return Output value of type static void.
     */
    static void gfunc_close_plot(gpointer data, gpointer user_data);
    osc_plugin *get_plugin_from_name(const char *name);

    /**
     * @brief Executes the load profile sequential handler procedure for OSCMain.
     * @param line Input parameter specifying line (int).
     * @param section Input parameter specifying section (const char).
     * @param name Input parameter specifying name (const char).
     * @param value Input parameter specifying value (const char).
     * @return Output value of type int.
     */
    int load_profile_sequential_handler(int line, const char *section, const char *name, const char *value);

    /**
     * @brief Executes the handle osc param procedure for OSCMain.
     * @param line Input parameter specifying line (int).
     * @param name Input parameter specifying name (const char).
     * @param value Input parameter specifying value (const char).
     * @return Output value of type int.
     */
    int handle_osc_param(int line, const char *name, const char *value);
    /**
     * @brief Executes the clear tab container procedure for OSCMain.
     */
    void ClearTabContainer(void);
    /**
     * @brief Executes the add plugin procedure for OSCMain.
     * @param plugin Input parameter specifying plugin (QString).
     */
    void AddPlugin(QString plugin);

signals:
    /**
     * @brief Executes the quit app procedure for OSCMain.
     */
    void quitApp();
    /**
     * @brief Executes the hopping signal procedure for OSCMain.
     * @param bool Input parameter specifying bool (bool).
     * @param min Input parameter specifying min (double).
     * @param max Input parameter specifying max (double).
     * @param step Input parameter specifying step (double).
     * @param delay Input parameter specifying delay (int).
     */
    void hoppingSignal(bool,double min,double max,double step,int delay);
    /**
     * @brief Executes the frq has focused signal procedure for OSCMain.
     */
    void frqHasFocusedSignal();
    /**
     * @brief Executes the adrv9009 signal procedure for OSCMain.
     */
    void adrv9009Signal();
    //saeid raziani
    void frqDomainIsSelectedSignal();
    void seekIsSelectedSignal();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // OSCMAIN_H
