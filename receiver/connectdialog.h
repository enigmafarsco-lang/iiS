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
    void btn_connect_clicked();
    void txBwToggled(bool checked);

private:
    Ui::connectDialog *ui;

    Settings setting;

    // Keep the ORx choices consistent with the TX choice:
    // TX 100 -> ORx 100; TX 200 -> ORx 100/200; TX 400 -> ORx 200/400.
    void syncOrxChoices();

signals:
    void connectSignal();
};

#endif // CONNECTDIALOG_H
