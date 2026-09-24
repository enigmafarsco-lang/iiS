#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H


#include "tcpclient.h"
#include <QRandomGenerator>
#include <cstdint>

#define _HEADER_ 0xAABB
#define _FOOTER_ 0xCCDD


class packetManager : public QObject
{
    Q_OBJECT
public:


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

    typedef enum _STATUS_
    {
        NONE=0,
        ON=100,
        OFF=200
    }status_t;

    typedef enum _PRTMODE_
    {
        None=0,
        Manual=1,
        Auto=2
    }protectionMode_t;


    typedef enum _DATASEND_
    {
        Request=1,
        SelfSend=2
    }DataSendMode_t;

    typedef enum _ERROR_
    {
        ER_NoError=0,
        ER_OverCurrent,
        ER_LowCurrent,   // no load
        ER_ShortCircuit,
        ER_HighVoltage,
        ER_LowVoltage,
        ER_CopleVoltage,
        ER_SourceOverVoltage,
        ER_SourceLowVoltage
    }ERROR_t;


    typedef struct _PACKET_
    {
        uint16_t hader;
        uint16_t boardId;
        uint16_t packetLen;
        uint8_t dataType;
        uint8_t dataLen;
        uint16_t footer;
        //uint16_t crc;
        uint8_t *data;
    }packet_t;


    typedef struct _OUTPUT_
    {
        status_t State;
        status_t Monitor;
        uint16_t Current;
        uint16_t CurrentADCValue;
        ERROR_t Error;

        uint16_t MaxCurrent;
        status_t DefState;
        double calibX;
        double calibY;

    }output_t;


    typedef struct _L_METER_
    {
        uint8_t valuePersent;
        uint16_t AdcValue;

        double calib_X;
        double calib_Y;
    }levelMeter_t;

    typedef struct _COOLING_BOARD_
    {
        output_t COMPRESSOR;
        output_t FAN;
        output_t MOTOR;
        protectionMode_t protectionMode;
        DataSendMode_t dataSendMode;
        levelMeter_t LevelMeter;
    }CoolingBoard_t;


    packetManager();
    static uint8_t LT[50];
    static CoolingBoard_t CoolingBoard;


private:
//    TCPClient tcpClient;
    bool creatPacket(uint8_t slaveNumber,uint8_t *data,uint8_t Packetlen,FC_t fc);
    bool FC_WriteMultReg_Fun(uint8_t data[]);
    uint16_t CRC16_Calculate( uint8_t *buf,uint16_t len );
    void fillModel();
    void checkInputPacket(uint8_t *inPack);
    uint8_t validationInputPacket(uint8_t *inPack);
    void FC_ReadMultReg_Fun(uint8_t *data);


public slots:
    void packetReceived(QByteArray);
signals:
    void newDataUpdate(CoolingBoard_t);

};

#endif // PACKETMANAGER_H
