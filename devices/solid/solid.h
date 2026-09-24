#ifndef SOLID_H
#define SOLID_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QMovie>
#include <QJsonObject>
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"


namespace Ui {
class Solid;
}

class Solid : public QWidget
{
    Q_OBJECT

public:
    explicit Solid(QWidget *parent = nullptr);
    ~Solid();

public slots:
    void on_DeviceResponseSlot(QByteArray);
    void joshanFuncDataSlot();
    void joshanStatusDataSlot();


signals:
    void sendCommandToDeviceSignal(QByteArray);

public:
    void handle_info(QByteArray);


signals:
    void sendCommandToOtherObjectSignal(QString);

public slots:
    void receiveCommandFromOtherObjectSlot(QString);
    void isDeviceConnectedSlot(bool);
    void joshanControlDataSlot(QJsonObject);

    void timeout();

private slots:
    void on_chkAlcMode_clicked(bool checked);

    void on_btnEnable_clicked();

    void on_btnDisable_clicked();

    void on_btnReset_clicked();

    void on_btnSolidStatus_clicked();

    void on_rdbHigh_clicked();

    void on_rdbMedium_clicked();

    void on_rdbLow_clicked();


    void on_chkErrLacthMode_clicked(bool checked);

signals:
    void sendStatusToOuterNodeSignal(QJsonObject);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);
    void sendStatusTempSignal(int);

private:
    Ui::Solid *ui;

    QString m_commandString = "";
    QByteArray m_commandBytes;

    //============
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

    QString silver_color = "background-color: silver";
    QString white_color = "background-color: white; color: blue";
    QString warningStyle ="background-color:yellow;color: black";
    QString dangerStyle ="background-color:"+QString(CRIMSON)+";color: "+QString(DARK_OLIVE_GREEN );
    QTimer blinkingTimer;

    QString m_state = "UNKNOWN";

    QString m_defaultStyleSheet = "";
    bool isDeviceConnected{false};

    QMovie *movie;
};

#endif // SOLID_H
