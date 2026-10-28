/**
 * @file joshan.h
 * @brief Subsystem module for joshan in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef JOSHAN_H
#define JOSHAN_H

#include <QWidget>
#include "QJsonDocument"
#include "Communications/appconfiguration.h"
#include "constants/project_constans.h"
#include "Communications/tcplan.h"
//devices
#include "devices/joshan/datastruct.h"



namespace Ui {
/**
 * @class Joshan
 * @brief High-level controller class for Joshan.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Joshan;
}

/**
 * @class Joshan
 * @brief High-level controller class for Joshan.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Joshan : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new Joshan instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget *).
     */
    explicit Joshan(QWidget     * parent      = nullptr);




    /**
     * @brief Retrieves the current oshan stop setter value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isJoshanStopSetter(bool);

    /**
     * @brief Destructor for Joshan, releasing allocated hardware resources, timers, and buffers.
     */
    ~Joshan();

public slots:
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray);
    /**
     * @brief Retrieves the current func data joshan slot value.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void getFuncDataJoshanSlot(QJsonObject);
    /**
     * @brief Retrieves the current status data joshan slot value.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void getStatusDataJoshanSlot(QJsonObject);
    /**
     * @brief Executes the joshan is connected procedure for Joshan.
     * @param bool Input parameter specifying bool (bool).
     */
    void joshanIsConnected(bool);

private slots:

    //    void gettingData(QString,QString);

private:
    AppConfiguration * mAppConfig;
    BTcpLan * mJoshan;

    QTimer * joshanFunctionalTimer;
    QTimer * joshanStatusTimer;


    /**
     * @brief Initializes and configures the init for the Joshan subsystem.
     */
    void init();
    /**
     * @brief Executes the connections procedure for Joshan.
     */
    void connections();
    /**
     * @brief Executes the handle info procedure for Joshan.
     * @param QString Input parameter specifying QString (QString).
     */
    void handle_info(QString);
    int cntFunc{};
    int cntStatus{};
    bool isJoshanStop{false};
    QString lnaStatus{};
    QString receivedDeviceValue;
    QJsonObject deviceFuncData;
    QJsonObject deviceStatusData;
//    QJsonArray /*deviceDataArr*/;
//    QJsonDocument val(deviceData);

    Ui::Joshan *ui;

signals:
    void servoData   (QString);
    /**
     * @brief Executes the solid data procedure for Joshan.
     * @param QString Input parameter specifying QString (QString).
     */
    void solidData   (QString);
    /**
     * @brief Executes the exiter data procedure for Joshan.
     * @param QString Input parameter specifying QString (QString).
     */
    void exiterData  (QString);
    /**
     * @brief Executes the cooling data procedure for Joshan.
     * @param QString Input parameter specifying QString (QString).
     */
    void coolingData (QString);
    /**
     * @brief Executes the rff data procedure for Joshan.
     * @param QString Input parameter specifying QString (QString).
     */
    void rffData     (QString);
    /**
     * @brief Transmits send func data to joshan signal command/packet over the communication bus.
     */
    void sendFuncDataToJoshanSignal();
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     */
    void sendStatusDataToJoshanSignal();
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);

    /**
     * @brief Executes the joshan data to rff signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToRffSignal           (QJsonObject);
    /**
     * @brief Executes the joshan data to solid signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToSolidSignal         (QJsonObject);
    /**
     * @brief Executes the joshan data to servo signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToServoSignal         (QJsonObject);
    /**
     * @brief Executes the joshan data to cooling signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToCoolingSignal       (QJsonObject);
    /**
     * @brief Executes the joshan data to exciter set signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToExciterSetSignal    (QJsonObject);
    /**
     * @brief Executes the joshan data to exciter enable signal procedure for Joshan.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanDataToExciterEnableSignal (QJsonObject);

//    void connectionStatusSignal(bool);
    void structFuncDataSignal(joshanFuncStruct*);
    void structStatusDataSignal(joshanStatusStruct*);


};

#endif // JOSHAN_H
