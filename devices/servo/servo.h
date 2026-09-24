#ifndef SERVO_H
#define SERVO_H

#include <QWidget>
#include <QMessageBox>
#include <bitset>
#include <iostream>
#include <QThread>
#include <QTimer>
#include <math.h>
#include <cmath>
#include <QFile>

#include "../../constants/project_constans.h"
#include "../../utils.h"
#include "../../usertype.h"
#include "devices/targetDetector/targetdetector.h"
#include "devices/joshan/datastruct.h"
#include "QShortcut"


namespace Ui {
class Servo;
}

class Servo : public QWidget
{
    Q_OBJECT

private:

    int i{};
    double panDestination    {1000};
    double tiltDestination   {1000};
    double lastTarget        {0};
    double offsetNorth       {0};
    double northOffset       {0};
    double panPosDeg         {0};
    double rotatorNorthOffset{0};
    double tiltPosDeg        {0};

    bool checkOffsetFlag = false;
    bool panInRange      = false;
    bool rotatorIsStop   = true;
    bool ok;

    QTimer timerAutoSet;
    QTimer timerPark;

    enum communicationType{ unasigned = 0, lan = 1, serial = 2 } comeType;


private:

    void sendPanTiltToCommander(double, double);
    void setTilPanSpeed(int,int, QString);
    void panTiltStopEarlier();
    void init();

    double coordinatorToAngle(double,double,double,double);
    double degreeToRadian(double);
    double radianToDegree(double);

    bool setTiltPosition(const double&);
    bool compareDouble(double, double);
    bool setPanPos(double&,  double&);
    bool isDeviceConnected{false};
    bool isPanStart{false};
    bool isTileStart{false};

    QByteArray handleWildCharacter(const QByteArray&);
    QByteArray creatingSpeedCmd(int, int, QString);
    QByteArray getByteSetTilt(const double&);
    QByteArray creatingCmd(double,QString);
    QByteArray getCmdSetPanPos(double);
    QByteArray creatingStopCmd();

    quint8 getCheckSum(const QByteArray&);
    int showMessage(QString, int);
    Ui::Servo *ui;


public:
    explicit Servo(QWidget *parent = nullptr);

    ~Servo();


public slots:
    void on_DeviceResponseSlot(QByteArray response);
    void receiveCommandFromObjectSlot(QString);
    void joshanFuncDataSlot();
    void joshanStatusDataSlot();
    void isDeviceConnectedSlot(bool);
    void joshanControlDataSlot(QJsonObject);


private slots:
    void on_chbCheckoffset_stateChanged(int arg1);
    void on_chbCheckoffset_clicked(bool checked);
    void on_chbAutoSet_clicked(bool checked);
    void on_btnSetTiltSpeed_clicked();
    void on_btnSetPanSpeed_clicked();
    void on_btnJogRight_released();
    void on_btnJogDown_released();
    void on_btnJogLeft_released();
    void on_btnJogRight_pressed();
    void on_btnJogDown_pressed();
    void on_btnJogLeft_pressed();
    void on_btnSetTilt_clicked();
    void on_btnAutoSet_clicked();
    void on_btnSetPan_clicked();
    void on_btnJogUp_released();
    void on_btnJogUp_pressed();
    void on_btnPark_clicked();
    void on_btnStop_clicked();
    void checkingPanInPark();
    void updateAutoSet();


signals:
    void sendUdpCommandToTargetDetectionSignal(QByteArray);
    void sendTcpCommandToTargetDetectionSignal(QByteArray);
    void sendCommandToDeviceSignal(QByteArray);
    void sendCommandToObjectSignal(QString);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);




};

#endif // SERVO_H
