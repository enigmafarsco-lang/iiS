// ============================================================================
// File: servo.h
// Subsystem: eLynxSDR servo
// Description: Subsystem implementation and interface guidance for servo.h.
// ============================================================================

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

    // Transmits send pan tilt to commander command/packet over communication interface.
    void sendPanTiltToCommander(double, double);
    // Sets til pan speed value and updates internal subsystem configuration.
    void setTilPanSpeed(int,int, QString);
    // Executes pan tilt stop earlier routine.
    void panTiltStopEarlier();
    // Initializes init for Servo.
    void init();

    // Executes coordinator to angle routine.
    double coordinatorToAngle(double,double,double,double);
    // Executes degree to radian routine.
    double degreeToRadian(double);
    // Executes radian to degree routine.
    double radianToDegree(double);

    // Sets tilt position value and updates internal subsystem configuration.
    bool setTiltPosition(const double&);
    // Executes compare double routine.
    bool compareDouble(double, double);
    // Sets pan pos value and updates internal subsystem configuration.
    bool setPanPos(double&,  double&);
    bool isDeviceConnected{false};
    bool isPanStart{false};
    bool isTileStart{false};

    // Executes handle wild character routine.
    QByteArray handleWildCharacter(const QByteArray&);
    // Executes creating speed cmd routine.
    QByteArray creatingSpeedCmd(int, int, QString);
    // Retrieves current byte set tilt state.
    QByteArray getByteSetTilt(const double&);
    // Executes creating cmd routine.
    QByteArray creatingCmd(double,QString);
    // Retrieves current cmd set pan pos state.
    QByteArray getCmdSetPanPos(double);
    // Executes creating stop cmd routine.
    QByteArray creatingStopCmd();

    // Retrieves current check sum state.
    quint8 getCheckSum(const QByteArray&);
    // Executes show message routine.
    int showMessage(QString, int);
    Ui::Servo *ui;


public:
    // Constructor: Initializes Servo subsystem components, default parameters, and state.
    explicit Servo(QWidget *parent = nullptr);

    // Destructor: Cleans up Servo resources, timers, and allocated memory.
    ~Servo();


public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray response);
    // Reads and parses incoming receive command from object slot data.
    void receiveCommandFromObjectSlot(QString);
    // Executes joshan func data slot routine.
    void joshanFuncDataSlot();
    // Executes joshan status data slot routine.
    void joshanStatusDataSlot();
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);
    // Executes joshan control data slot routine.
    void joshanControlDataSlot(QJsonObject);


private slots:
    // Event/Signal handler for on chb checkoffset state changed.
    void on_chbCheckoffset_stateChanged(int arg1);
    // Event/Signal handler for on chb checkoffset clicked.
    void on_chbCheckoffset_clicked(bool checked);
    // Event/Signal handler for on chb auto set clicked.
    void on_chbAutoSet_clicked(bool checked);
    // Event/Signal handler for on btn set tilt speed clicked.
    void on_btnSetTiltSpeed_clicked();
    // Event/Signal handler for on btn set pan speed clicked.
    void on_btnSetPanSpeed_clicked();
    // Event/Signal handler for on btn jog right released.
    void on_btnJogRight_released();
    // Event/Signal handler for on btn jog down released.
    void on_btnJogDown_released();
    // Event/Signal handler for on btn jog left released.
    void on_btnJogLeft_released();
    // Event/Signal handler for on btn jog right pressed.
    void on_btnJogRight_pressed();
    // Event/Signal handler for on btn jog down pressed.
    void on_btnJogDown_pressed();
    // Event/Signal handler for on btn jog left pressed.
    void on_btnJogLeft_pressed();
    // Event/Signal handler for on btn set tilt clicked.
    void on_btnSetTilt_clicked();
    // Event/Signal handler for on btn auto set clicked.
    void on_btnAutoSet_clicked();
    // Event/Signal handler for on btn set pan clicked.
    void on_btnSetPan_clicked();
    // Event/Signal handler for on btn jog up released.
    void on_btnJogUp_released();
    // Event/Signal handler for on btn jog up pressed.
    void on_btnJogUp_pressed();
    // Event/Signal handler for on btn park clicked.
    void on_btnPark_clicked();
    // Event/Signal handler for on btn stop clicked.
    void on_btnStop_clicked();
    // Executes checking pan in park routine.
    void checkingPanInPark();
    // Updates and synchronizes update auto set data stream.
    void updateAutoSet();


signals:
    // Transmits send udp command to target detection signal command/packet over communication interface.
    void sendUdpCommandToTargetDetectionSignal(QByteArray);
    // Transmits send tcp command to target detection signal command/packet over communication interface.
    void sendTcpCommandToTargetDetectionSignal(QByteArray);
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
    // Transmits send command to object signal command/packet over communication interface.
    void sendCommandToObjectSignal(QString);
    // Transmits send func data to joshan signal command/packet over communication interface.
    void sendFuncDataToJoshanSignal(QJsonObject);
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal(QJsonObject);




};

#endif // SERVO_H
