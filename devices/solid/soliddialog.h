/**
 * @file soliddialog.h
 * @brief Subsystem module for solid in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef SOLIDDIALOG_H
#define SOLIDDIALOG_H

#include <QDialog>
#include <iostream>
#include <bitset>
#include <QTimer>
#include <QJsonObject>
#include "devices/joshan/datastruct.h"

using namespace std;
namespace Ui {
/**
 * @class SolidDialog
 * @brief High-level controller class for SolidDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class SolidDialog;
}

/**
 * @class SolidDialog
 * @brief High-level controller class for SolidDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class SolidDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new SolidDialog instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit SolidDialog(QWidget *parent = nullptr);
    /**
     * @brief Destructor for SolidDialog, releasing allocated hardware resources, timers, and buffers.
     */
    ~SolidDialog();

public slots:
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray);

signals:
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
    /**
     * @brief Transmits send to main sig command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendToMainSig(QString);
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusDataToJoshanSignal(QJsonObject);


public:
    /**
     * @brief Executes the handle info procedure for SolidDialog.
     * @param QString Input parameter specifying QString (QString).
     */
    void handle_info(QString);

signals:
    /**
     * @brief Transmits send command to other object signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendCommandToOtherObjectSignal(QString);
//    void

public slots:
    void receiveCommandFromOtherObjectSlot(QString);
    void joshanStatusDataSlot();
    void isDeviceConnectedSlot(bool);

private slots:
    /**
     * @brief Slot handler responding to on blinking timer time out event.
     */
    void  on_blinkingTimer_timeOut();

    /**
     * @brief Slot handler responding to on btn start report clicked event.
     */
    void on_btnStartReport_clicked();

    /**
     * @brief Slot handler responding to on btn stop report clicked event.
     */
    void on_btnStopReport_clicked();

    /**
     * @brief Slot handler responding to on  solid dialog finished event.
     * @param result Input parameter specifying result (int).
     */
    void on_SolidDialog_finished(int result);

private:
    //holding statuses for passing to joshan
    bool solidActive{};
    quint8 vswrStatus{};
    quint8 dcSupplyStatus{};
    quint8 linkStatus{};
    quint8 fanStatus{};
    quint8 internalLockStatus{};
    quint8 gateLevelStatus{};
    bool isDeviceConnected{false};
    //-----------------------------------

    void initBlinkingTimer();
    void connections();
    void initLedsIcon();
private:
    Ui::SolidDialog *ui;

    QString m_commandString = "";
    QByteArray m_commandBytes;

    bool dcSupply_blinking = false;
    bool vswr_blinking = false;
    bool gate_blinking = false;
    bool temp_blinking = false;

    QTimer* blinking_timer;

    //====================
    //    - White (text): #eff0f1
    //    - Dark black (main bg): #272a2d
    //    - Lighter dark black (menu bar): #373b3f
    //    - Dark blue: #18465d
    //    - Gray: #76797C
    //    - Red (accents): #db1c49
    //    -Purpel : rgb(85, 85, 127)
    //======================
    QString orginal_color;

    QString purpel_color = "background-color: rgb(85, 85, 127)";
    QString gray_color = "background-color: #76797C";
    QString yellow_color = "background-color: yellow";
    QString red_color = "background-color: #db1c49";
    QString green_color = "background-color:#186a3b";

    QString silver_color = "background-color: silver";
    QString white_color = "background-color: white; color: blue";
};

#endif // SOLIDDIALOG_H
