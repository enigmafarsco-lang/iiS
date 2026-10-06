#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <receiver/globals.h>
#include <QDialog>
#include <QDebug>
#include <glib.h>
#include <glib-object.h>
#include <iio.h>
#include <receiver/settings.h>

namespace Ui {
class connectDialog;
}

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectDialog(QDialog *parent = nullptr);
    ~connectDialog();

    bool Initialize();

    // TX bandwidth (100/200/400) and ORx bandwidth selected in the
    // first-start form.  The startup profile assertion composes the
    // Talise profile from these two choices (general + orx + tx + </profile>).
    int selectedTxBw() const;
    int selectedOrxBw() const;

private slots:

    void btn_cancel_clicked();
    void btn_refresh_clicked();
    void btn_connect_clicked();
    void txBwToggled(bool checked);

private:
    Ui::connectDialog *ui;

    Settings setting;

    // Board serial number read from the target (empty until known).
    QString boardSerial;

    // Keep the ORx choices consistent with the TX choice:
    // TX 100 -> ORx 100; TX 200 -> ORx 100/200; TX 400 -> ORx 200/400.
    void syncOrxChoices();

    // Create/reuse the IIO context from the URI currently in the form
    // (manual entry only - the discover/scan section was removed).
    struct iio_context * GetContext();

    // Refresh the IIO Context Information panels (Board Serial, Context
    // Description, FRU Info, IIO Devices, Context Attributes) from the
    // entered URI.
    bool ReloadConnectDialog();

    // Read the board serial number into the form (it gates the ORx 400
    // option: SN001 boards have ORx 100/200 only).
    void updateBoardSerial();

signals:
    void connectSignal();
};

#endif // CONNECTDIALOG_H
