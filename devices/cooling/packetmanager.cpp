/**
 * @file packetmanager.cpp
 * @brief Subsystem module for cooling in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "packetmanager.h"
//#include "tcpclient.h"


uint8_t packetManager::LT[50]={};
packetManager::CoolingBoard_t packetManager::CoolingBoard={};

/**
 * @brief Constructs a new packetManager instance and initializes subsystem components and UI state.
 */
packetManager::packetManager()
{

}

/**
 * @brief Executes the packet received procedure for packetManager.
 * @param dataReceiced Input parameter specifying dataReceiced (QByteArray).
 */
void packetManager::packetReceived(QByteArray dataReceiced)
{
    dataReceiced = dataReceiced.toHex();

    QByteArray item;
    uint8_t data[100];
    bool ok;
    int index=0;

    if(dataReceiced.length()<10) return;

    for(int i=0;i<dataReceiced.size();i+=2)
    {
        item.append(dataReceiced[i]).append(dataReceiced[i+1]);
        data[index++]=(item.toInt(&ok, 16));
        item.clear();
    }

//    qDebug()<<"Data In  :   "<<dataReceiced.length();
//    qDebug()<<"Data In  :   "<<dataReceiced;
    checkInputPacket(data);
}


void packetManager::checkInputPacket(uint8_t *inPack)
{
    uint8_t dataType_;
    dataType_=validationInputPacket(inPack);
    if(dataType_!=0 && dataType_ == FC_ReadMultReg )
    {
        FC_ReadMultReg_Fun(&inPack[8]);
    }
}

/**
 * @brief Executes the validation input packet procedure for packetManager.
 * @param inPack Input parameter specifying inPack (uint8_t).
 * @return Output value of type uint8_t.
 */
uint8_t packetManager::validationInputPacket(uint8_t *inPack)
{
    uint8_t result=0;
    packet_t inputPacketTemp;
    uint16_t footer2;
    inputPacketTemp.hader=(inPack[0]*256)+inPack[1];
    inputPacketTemp.boardId=(inPack[2]*256)+inPack[3];
    inputPacketTemp.packetLen=(inPack[4]*256)+inPack[5];

    inputPacketTemp.dataType=inPack[6];
    inputPacketTemp.dataLen=inPack[7];
    inputPacketTemp.footer=(inPack[inputPacketTemp.dataLen+8]*256)+inPack[inputPacketTemp.dataLen+9];
    footer2=(inPack[inputPacketTemp.packetLen-2]*256)+inPack[inputPacketTemp.packetLen-1];

    if(inputPacketTemp.hader==_HEADER_ && inputPacketTemp.footer==_FOOTER_ && inputPacketTemp.footer==footer2)
    {
         result=inputPacketTemp.dataType;
    }

    return result;
}


/**
 * @brief Executes the f c  read mult reg  fun procedure for packetManager.
 * @param data Input parameter specifying data (uint8_t).
 */
void packetManager::FC_ReadMultReg_Fun(uint8_t *data)
{
    uint8_t DatapackLen=data[0];
    uint8_t start=data[1];
    uint8_t len=data[2];
    uint8_t InIndex=3;

    for(uint8_t i=0;i<DatapackLen;i++)
    {
        for(uint8_t j=start;j<start+len;j++)
        {
            LT[j]=data[InIndex++];
        }

        if(DatapackLen>1)
        {
            start=data[InIndex++];
            len=data[InIndex++];
        }
    }

    fillModel();
}

/**
 * @brief Executes the fill model procedure for packetManager.
 */
void packetManager::fillModel()
{
    int index=3;
    if(LT[index]==ON) CoolingBoard.COMPRESSOR.State=ON;
    else if(LT[index]==OFF) CoolingBoard.COMPRESSOR.State=OFF;

    index++;

    if(LT[index]==ON) CoolingBoard.FAN.State=ON;
    else if(LT[index]==OFF) CoolingBoard.FAN.State=OFF;

    index++;

    if(LT[index]==ON) CoolingBoard.MOTOR.State=ON;
    else if(LT[index]==OFF) CoolingBoard.MOTOR.State=OFF;

    index++;

    CoolingBoard.COMPRESSOR.Error=(ERROR_t)LT[index++];
    CoolingBoard.FAN.Error=(ERROR_t)LT[index++];
    CoolingBoard.MOTOR.Error=(ERROR_t)LT[index++];

    CoolingBoard.COMPRESSOR.Current=(LT[index]*256)+LT[index+1]; index+=2;
    CoolingBoard.FAN.Current=(LT[index]*256)+LT[index+1];        index+=2;

    if(CoolingBoard.FAN.State==ON) CoolingBoard.FAN.Current+=QRandomGenerator::global()->bounded(98,110);
    CoolingBoard.MOTOR.Current=(LT[index]*256)+LT[index+1];      index+=2;
    if(CoolingBoard.MOTOR.State==ON) CoolingBoard.MOTOR.Current+=QRandomGenerator::global()->bounded(195,220);

    CoolingBoard.LevelMeter.valuePersent=LT[index];  index++;


    if(LT[index]==ON) CoolingBoard.COMPRESSOR.DefState=ON;
    else if(LT[index]==OFF) CoolingBoard.COMPRESSOR.DefState=OFF;
    index++;
    if(LT[index]==ON) CoolingBoard.FAN.DefState=ON;
    else if(LT[index]==OFF) CoolingBoard.FAN.DefState=OFF;
    index++;
    if(LT[index]==ON) CoolingBoard.MOTOR.DefState=ON;
    else if(LT[index]==OFF) CoolingBoard.MOTOR.DefState=OFF;
    index++;

    CoolingBoard.COMPRESSOR.MaxCurrent=(LT[index]*256)+LT[index+1]; index+=2;
    CoolingBoard.FAN.MaxCurrent=(LT[index]*256)+LT[index+1];        index+=2;
    CoolingBoard.MOTOR.MaxCurrent=(LT[index]*256)+LT[index+1];      index+=2;

    CoolingBoard.protectionMode=(protectionMode_t)LT[index];        index++;
    CoolingBoard.dataSendMode=(DataSendMode_t)LT[index];            index++;


    //////////////////////////////////////////////////////////////////////////   Calibration Value

    CoolingBoard.COMPRESSOR.CurrentADCValue=(LT[index]*256)+LT[index+1]; index+=2;
    CoolingBoard.FAN.CurrentADCValue=(LT[index]*256)+LT[index+1]; index+=2;

    CoolingBoard.MOTOR.CurrentADCValue=(LT[index]*256)+LT[index+1]; index+=2;

    CoolingBoard.LevelMeter.AdcValue=(LT[index]*256)+LT[index+1];

    ///////////////////////////////////////////////////////////////////////////////////////////////
    emit newDataUpdate(CoolingBoard);
}
