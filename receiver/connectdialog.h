/**
 * @file connectdialog.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class connectDialog
 * @brief High-level controller class for connectDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class connectDialog;
}

/**
 * @class connectDialog
 * @brief High-level controller class for connectDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class connectDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new connectDialog instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QDialog).
     */
    explicit connectDialog(QDialog *parent = nullptr);
    /**
     * @brief Destructor for connectDialog, releasing allocated hardware resources, timers, and buffers.
     */
    ~connectDialog();

    /**
     * @brief Executes the initialize procedure for connectDialog.
     * @return Output value of type bool.
     */
    bool Initialize();

    // TX bandwidth (100/200/400) and ORx bandwidth selected in the
    // first-start form.  The startup profile assertion composes the
    // Talise profile from these two choices (general + orx + tx + </profile>).
    int selectedTxBw() const;
    int selectedOrxBw() const;

private slots:

    /**
     * @brief Slot handler responding to btn cancel clicked event.
     */
    void btn_cancel_clicked();
    /**
     * @brief Slot handler responding to btn refresh clicked event.
     */
    void btn_refresh_clicked();
    /**
     * @brief Slot handler responding to btn connect clicked event.
     */
    void btn_connect_clicked();
    /**
     * @brief Executes the tx bw toggled procedure for connectDialog.
     * @param checked Input parameter specifying checked (bool).
     */
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
