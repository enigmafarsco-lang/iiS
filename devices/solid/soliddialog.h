// ============================================================================
// File: soliddialog.h
// Subsystem: eLynxSDR solid
// Description: Subsystem implementation and interface guidance for soliddialog.h.
// ============================================================================

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
class SolidDialog;
}

class SolidDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor: Initializes SolidDialog subsystem components, default parameters, and state.
    explicit SolidDialog(QWidget *parent = nullptr);
    // Destructor: Cleans up SolidDialog resources, timers, and allocated memory.
    ~SolidDialog();

public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray);

signals:
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
    // Transmits send to main sig command/packet over communication interface.
    void sendToMainSig(QString);
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal(QJsonObject);


public:
    // Executes handle info routine.
    void handle_info(QString);

signals:
    // Transmits send command to other object signal command/packet over communication interface.
    void sendCommandToOtherObjectSignal(QString);
//    void

public slots:
    // Reads and parses incoming receive command from other object slot data.
    void receiveCommandFromOtherObjectSlot(QString);
    // Executes joshan status data slot routine.
    void joshanStatusDataSlot();
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);

private slots:
    // Event/Signal handler for on blinking timer time out.
    void  on_blinkingTimer_timeOut();

    // Event/Signal handler for on btn start report clicked.
    void on_btnStartReport_clicked();

    // Event/Signal handler for on btn stop report clicked.
    void on_btnStopReport_clicked();

    // Event/Signal handler for on  solid dialog finished.
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

    // Initializes init blinking timer for SolidDialog.
    void initBlinkingTimer();
    // Executes connections routine.
    void connections();
    // Initializes init leds icon for SolidDialog.
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
