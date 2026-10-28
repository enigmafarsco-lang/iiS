/**
 * @file servo.h
 * @brief Subsystem module for servo in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class Servo
 * @brief High-level controller class for Servo.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Servo;
}

/**
 * @class Servo
 * @brief High-level controller class for Servo.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
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

    /**
     * @brief Transmits send pan tilt to commander command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     */
    void sendPanTiltToCommander(double, double);
    /**
     * @brief Sets the til pan speed parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     * @param int Input parameter specifying int (int).
     * @param QString Input parameter specifying QString (QString).
     */
    void setTilPanSpeed(int,int, QString);
    /**
     * @brief Executes the pan tilt stop earlier procedure for Servo.
     */
    void panTiltStopEarlier();
    /**
     * @brief Initializes and configures the init for the Servo subsystem.
     */
    void init();

    /**
     * @brief Executes the coordinator to angle procedure for Servo.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     * @return Output value of type double.
     */
    double coordinatorToAngle(double,double,double,double);
    /**
     * @brief Executes the degree to radian procedure for Servo.
     * @param double Input parameter specifying double (double).
     * @return Output value of type double.
     */
    double degreeToRadian(double);
    /**
     * @brief Executes the radian to degree procedure for Servo.
     * @param double Input parameter specifying double (double).
     * @return Output value of type double.
     */
    double radianToDegree(double);

    /**
     * @brief Sets the tilt position parameter and updates internal state.
     * @param double& Input parameter specifying double& (const).
     * @return Output value of type bool.
     */
    bool setTiltPosition(const double&);
    /**
     * @brief Executes the compare double procedure for Servo.
     * @param double Input parameter specifying double (double).
     * @param double Input parameter specifying double (double).
     * @return Output value of type bool.
     */
    bool compareDouble(double, double);
    /**
     * @brief Sets the pan pos parameter and updates internal state.
     * @param double& Input parameter specifying double& (double&).
     * @param double& Input parameter specifying double& (double&).
     * @return Output value of type bool.
     */
    bool setPanPos(double&,  double&);
    bool isDeviceConnected{false};
    bool isPanStart{false};
    bool isTileStart{false};

    /**
     * @brief Executes the handle wild character procedure for Servo.
     * @param QByteArray& Input parameter specifying QByteArray& (const).
     * @return Output value of type QByteArray.
     */
    QByteArray handleWildCharacter(const QByteArray&);
    /**
     * @brief Executes the creating speed cmd procedure for Servo.
     * @param int Input parameter specifying int (int).
     * @param int Input parameter specifying int (int).
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type QByteArray.
     */
    QByteArray creatingSpeedCmd(int, int, QString);
    /**
     * @brief Retrieves the current byte set tilt value.
     * @param double& Input parameter specifying double& (const).
     * @return Output value of type QByteArray.
     */
    QByteArray getByteSetTilt(const double&);
    /**
     * @brief Executes the creating cmd procedure for Servo.
     * @param double Input parameter specifying double (double).
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type QByteArray.
     */
    QByteArray creatingCmd(double,QString);
    /**
     * @brief Retrieves the current cmd set pan pos value.
     * @param double Input parameter specifying double (double).
     * @return Output value of type QByteArray.
     */
    QByteArray getCmdSetPanPos(double);
    /**
     * @brief Executes the creating stop cmd procedure for Servo.
     * @return Output value of type QByteArray.
     */
    QByteArray creatingStopCmd();

    /**
     * @brief Retrieves the current check sum value.
     * @param QByteArray& Input parameter specifying QByteArray& (const).
     * @return Output value of type quint8.
     */
    quint8 getCheckSum(const QByteArray&);
    /**
     * @brief Executes the show message procedure for Servo.
     * @param QString Input parameter specifying QString (QString).
     * @param int Input parameter specifying int (int).
     * @return Output value of type int.
     */
    int showMessage(QString, int);
    Ui::Servo *ui;


public:
    /**
     * @brief Constructs a new Servo instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit Servo(QWidget *parent = nullptr);

    /**
     * @brief Destructor for Servo, releasing allocated hardware resources, timers, and buffers.
     */
    ~Servo();


public slots:
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param response Input parameter specifying response (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray response);
    /**
     * @brief Reads, parses, and decodes incoming receive command from object slot data buffer.
     * @param QString Input parameter specifying QString (QString).
     */
    void receiveCommandFromObjectSlot(QString);
    /**
     * @brief Executes the joshan func data slot procedure for Servo.
     */
    void joshanFuncDataSlot();
    /**
     * @brief Executes the joshan status data slot procedure for Servo.
     */
    void joshanStatusDataSlot();
    /**
     * @brief Retrieves the current evice connected slot value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isDeviceConnectedSlot(bool);
    /**
     * @brief Executes the joshan control data slot procedure for Servo.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanControlDataSlot(QJsonObject);


private slots:
    /**
     * @brief Slot handler responding to on chb checkoffset state changed event.
     * @param arg1 Input parameter specifying arg1 (int).
     */
    void on_chbCheckoffset_stateChanged(int arg1);
    /**
     * @brief Slot handler responding to on chb checkoffset clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chbCheckoffset_clicked(bool checked);
    /**
     * @brief Slot handler responding to on chb auto set clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chbAutoSet_clicked(bool checked);
    /**
     * @brief Slot handler responding to on btn set tilt speed clicked event.
     */
    void on_btnSetTiltSpeed_clicked();
    /**
     * @brief Slot handler responding to on btn set pan speed clicked event.
     */
    void on_btnSetPanSpeed_clicked();
    /**
     * @brief Slot handler responding to on btn jog right released event.
     */
    void on_btnJogRight_released();
    /**
     * @brief Slot handler responding to on btn jog down released event.
     */
    void on_btnJogDown_released();
    /**
     * @brief Slot handler responding to on btn jog left released event.
     */
    void on_btnJogLeft_released();
    /**
     * @brief Slot handler responding to on btn jog right pressed event.
     */
    void on_btnJogRight_pressed();
    /**
     * @brief Slot handler responding to on btn jog down pressed event.
     */
    void on_btnJogDown_pressed();
    /**
     * @brief Slot handler responding to on btn jog left pressed event.
     */
    void on_btnJogLeft_pressed();
    /**
     * @brief Slot handler responding to on btn set tilt clicked event.
     */
    void on_btnSetTilt_clicked();
    /**
     * @brief Slot handler responding to on btn auto set clicked event.
     */
    void on_btnAutoSet_clicked();
    /**
     * @brief Slot handler responding to on btn set pan clicked event.
     */
    void on_btnSetPan_clicked();
    /**
     * @brief Slot handler responding to on btn jog up released event.
     */
    void on_btnJogUp_released();
    /**
     * @brief Slot handler responding to on btn jog up pressed event.
     */
    void on_btnJogUp_pressed();
    /**
     * @brief Slot handler responding to on btn park clicked event.
     */
    void on_btnPark_clicked();
    /**
     * @brief Slot handler responding to on btn stop clicked event.
     */
    void on_btnStop_clicked();
    /**
     * @brief Executes the checking pan in park procedure for Servo.
     */
    void checkingPanInPark();
    /**
     * @brief Updates and synchronizes the update auto set data stream and visual indicators.
     */
    void updateAutoSet();


signals:
    /**
     * @brief Transmits send udp command to target detection signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendUdpCommandToTargetDetectionSignal(QByteArray);
    /**
     * @brief Transmits send tcp command to target detection signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendTcpCommandToTargetDetectionSignal(QByteArray);
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
    /**
     * @brief Transmits send command to object signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendCommandToObjectSignal(QString);
    /**
     * @brief Transmits send func data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendFuncDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusDataToJoshanSignal(QJsonObject);




};

#endif // SERVO_H
