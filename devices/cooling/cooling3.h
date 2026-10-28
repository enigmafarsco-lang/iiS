// ============================================================================
// File: cooling3.h
// Subsystem: eLynxSDR cooling
// Description: Subsystem implementation and interface guidance for cooling3.h.
// ============================================================================

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
class Cooling3;
}

class Cooling3 : public QWidget
{
    Q_OBJECT


public:
    // Constructor: Initializes Cooling3 subsystem components, default parameters, and state.
    explicit Cooling3(QWidget *parent = nullptr);

    // Destructor: Cleans up Cooling3 resources, timers, and allocated memory.
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
    // Executes creat packet routine.
    bool creatPacket(uint8_t slaveNumber,uint8_t *data,uint8_t Packetlen,FC_t fc);
    packetManager pManager ;
    // Executes reloade default gui routine.
    void reloadeDefaultGui();
    QTimer timerSendCoolerData;

    Ui::Cooling3 *ui;

private:
    int count {};
    bool isCoolerConnected{false};

private slots:
    // Event/Signal handler for on  btn auto clicked.
    void on_BtnAuto_clicked();
    // Event/Signal handler for on  btn manual clicked.
    void on_BtnManual_clicked();
    // Event/Signal handler for on  btn turn on cooling clicked.
    void on_BtnTurnOnCooling_clicked();
    // Event/Signal handler for on  btn turn off cooling clicked.
    void on_BtnTurnOffCooling_clicked();
    // Executes gui update routine.
    void GuiUpdate(packetManager::CoolingBoard_t);


public slots:
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);
    // Executes joshan func data slot routine.
    void joshanFuncDataSlot();
    // Executes joshan status data slot routine.
    void joshanStatusDataSlot();
    // Executes joshan control data slot routine.
    void joshanControlDataSlot(QJsonObject);
    // Retrieves current solid temp slot state.
    void getSolidTempSlot(int);



signals:
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
    // Executes new packet received signal routine.
    void newPacketReceivedSignal(QByteArray);
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal(QJsonObject);
    // Transmits send func data to joshan signal command/packet over communication interface.
    void sendFuncDataToJoshanSignal(QJsonObject);

};

#endif // COOLING3_H
