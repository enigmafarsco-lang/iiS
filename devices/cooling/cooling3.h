/**
 * @file cooling3.h
 * @brief Subsystem module for cooling in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef COOLING3_H
#define COOLING3_H

#include <QWidget>
#include "usertype.h"
#include "packetmanager.h"
#include <QTimer>
#include "constants/project_constans.h"
#include "devices/joshan/datastruct.h"


#define _HEADER_ 0xAABB
#define _FOOTER_ 0xCCDD


namespace Ui {
/**
 * @class Cooling3
 * @brief High-level controller class for Cooling3.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Cooling3;
}

/**
 * @class Cooling3
 * @brief High-level controller class for Cooling3.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Cooling3 : public QWidget
{
    Q_OBJECT


public:
    /**
     * @brief Constructs a new Cooling3 instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit Cooling3(QWidget *parent = nullptr);

    /**
     * @brief Destructor for Cooling3, releasing allocated hardware resources, timers, and buffers.
     */
    ~Cooling3();

private:

    int c{};
    bool autoMode{true};
    typedef enum _FC_
    {
        FC_ACK= 0,
        FC_ReadMultReg=2,
        FC_WriteMultReg=4,
        FC_ReadSetting,
        FC_WriteSetting ,
        FC_ReadBroadcast ,
        FC_WriteBroadcast ,
        FC_WriteCalibXY  ,
        FC_ReadCalibXY  ,
        FC_RestartModual
    }FC_t;

    enum on_off_status
    {
        Cooler_On=0,
        Cooler_Off=1
    };

    on_off_status status_mode;
    /**
     * @brief Executes the creat packet procedure for Cooling3.
     * @param slaveNumber Input parameter specifying slaveNumber (uint8_t).
     * @param data Input parameter specifying data (uint8_t).
     * @param Packetlen Input parameter specifying Packetlen (uint8_t).
     * @param fc Input parameter specifying fc (FC_t).
     * @return Output value of type bool.
     */
    bool creatPacket(uint8_t slaveNumber,uint8_t *data,uint8_t Packetlen,FC_t fc);
    packetManager pManager ;
    /**
     * @brief Executes the reloade default gui procedure for Cooling3.
     */
    void reloadeDefaultGui();
    QTimer timerSendCoolerData;

    Ui::Cooling3 *ui;

private:
    int count {};
    bool isCoolerConnected{false};

private slots:
    /**
     * @brief Slot handler responding to on  btn auto clicked event.
     */
    void on_BtnAuto_clicked();
    /**
     * @brief Slot handler responding to on  btn manual clicked event.
     */
    void on_BtnManual_clicked();
    /**
     * @brief Slot handler responding to on  btn turn on cooling clicked event.
     */
    void on_BtnTurnOnCooling_clicked();
    /**
     * @brief Slot handler responding to on  btn turn off cooling clicked event.
     */
    void on_BtnTurnOffCooling_clicked();
    /**
     * @brief Executes the gui update procedure for Cooling3.
     * @param packetManager::CoolingBoard_t Input parameter specifying packetManager::CoolingBoard_t (packetManager::CoolingBoard_t).
     */
    void GuiUpdate(packetManager::CoolingBoard_t);


public slots:
    /**
     * @brief Retrieves the current evice connected slot value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isDeviceConnectedSlot(bool);
    /**
     * @brief Executes the joshan func data slot procedure for Cooling3.
     */
    void joshanFuncDataSlot();
    /**
     * @brief Executes the joshan status data slot procedure for Cooling3.
     */
    void joshanStatusDataSlot();
    /**
     * @brief Executes the joshan control data slot procedure for Cooling3.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanControlDataSlot(QJsonObject);
    /**
     * @brief Retrieves the current solid temp slot value.
     * @param int Input parameter specifying int (int).
     */
    void getSolidTempSlot(int);



signals:
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
    /**
     * @brief Executes the new packet received signal procedure for Cooling3.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void newPacketReceivedSignal(QByteArray);
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send func data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendFuncDataToJoshanSignal(QJsonObject);

};

#endif // COOLING3_H
