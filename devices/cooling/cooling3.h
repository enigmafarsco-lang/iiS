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
    explicit Cooling3(QWidget *parent = nullptr);

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
    bool creatPacket(uint8_t slaveNumber,uint8_t *data,uint8_t Packetlen,FC_t fc);
    packetManager pManager ;
    void reloadeDefaultGui();
    QTimer timerSendCoolerData;

    Ui::Cooling3 *ui;

private:
    int count {};
    bool isCoolerConnected{false};

private slots:
    void on_BtnAuto_clicked();
    void on_BtnManual_clicked();
    void on_BtnTurnOnCooling_clicked();
    void on_BtnTurnOffCooling_clicked();
    void GuiUpdate(packetManager::CoolingBoard_t);


public slots:
    void isDeviceConnectedSlot(bool);
    void joshanFuncDataSlot();
    void joshanStatusDataSlot();
    void joshanControlDataSlot(QJsonObject);
    void getSolidTempSlot(int);



signals:
    void sendCommandToDeviceSignal(QByteArray);
    void newPacketReceivedSignal(QByteArray);
    void sendStatusDataToJoshanSignal(QJsonObject);
    void sendFuncDataToJoshanSignal(QJsonObject);

};

#endif // COOLING3_H
