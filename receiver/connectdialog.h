#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <receiver/globals.h>
#include <QDialog>
#include <QDebug>
#include <glib.h>
#include <glib-object.h>
#include <iio.h>
#include <receiver/settings.h>
#include <QFuture>
#include <QThread>

namespace Ui {
class connectDialog;
}

#define NO_DEVICES "No Devices"
static gchar *usb_pids[128];
static int active_pid = -1;

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectDialog(QDialog *parent = nullptr);
    ~connectDialog();    

    bool Initialize();

private slots:

    void rad_scan_toggled(bool checked);
    void rad_manual_toggled(bool checked);
    void btn_cancel_clicked();
    void btn_refresh_clicked();
    void btn_connect_clicked();
    void filter_ip_toggled(bool checked);
    void filter_usb_toggled(bool checked);
    void on_cmb_connect_usbd_currentIndexChanged(int index);

private:
    Ui::connectDialog *ui;        
    gint ret;
    const gchar *ip_addr;

    Settings setting;

    void RefreshScanMode();
    void ClearDialog();
    struct iio_context * GetContext();
    bool ReloadConnectDialog();

signals:
    void connectSignal();
};

#endif // CONNECTDIALOG_H
