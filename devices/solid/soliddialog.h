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
    explicit SolidDialog(QWidget *parent = nullptr);
    ~SolidDialog();

public slots:
    void on_DeviceResponseSlot(QByteArray);

signals:
    void sendCommandToDeviceSignal(QByteArray);
    void sendToMainSig(QString);
    void sendStatusDataToJoshanSignal(QJsonObject);


public:
    void handle_info(QString);

signals:
    void sendCommandToOtherObjectSignal(QString);
//    void

public slots:
    void receiveCommandFromOtherObjectSlot(QString);
    void joshanStatusDataSlot();
    void isDeviceConnectedSlot(bool);

private slots:
    void  on_blinkingTimer_timeOut();

    void on_btnStartReport_clicked();

    void on_btnStopReport_clicked();

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
