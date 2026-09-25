#include "cooling3.h"
#include "ui_cooling3.h"

Cooling3::Cooling3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cooling3)
{
    ui->setupUi(this);

    connect(this,SIGNAL(newPacketReceivedSignal(QByteArray)),&pManager,SLOT(packetReceived(QByteArray)));
    connect(&pManager,&packetManager::newDataUpdate,this,&Cooling3::GuiUpdate);

    //first time we want to run it in auto mode
    if(autoMode)
    {
//        on_BtnAuto_clicked();
    }

    else
    {
//        on_BtnManual_clicked();
    }

//    ui->BtnAuto->setEnabled(false);
//    ui->BtnManual->setEnabled(false);

    on_BtnManual_clicked();
    on_BtnTurnOffCooling_clicked();


//    connect();

}

Cooling3::~Cooling3()
{
    delete ui;
}



void Cooling3::isDeviceConnectedSlot(bool val)
{

    isCoolerConnected = val;

    if(status_mode == Cooler_Off and isCoolerConnected)
    {
//        on_BtnTurnOffCooling_clicked();
    }

    else if(status_mode == Cooler_On and isCoolerConnected)
    {
//        on_BtnTurnOnCooling_clicked();
    }

    ui->BtnManual->setEnabled(val);
    ui->BtnAuto->setEnabled(val);

    ui->BtnTurnOnCooling->setEnabled(val);
    ui->BtnTurnOffCooling->setEnabled(val);
    //turn on motor
//    uint8_t data[]={1,0,1,packetManager::ON};
//    creatPacket(1,data,4,FC_WriteMultReg);
    uint8_t data[]={1,2,1,packetManager::ON};
    creatPacket(1,data,4,FC_WriteMultReg);
}

void Cooling3::joshanFuncDataSlot()
{
    QJsonObject coolingData;
    coolingData["cooling_mode"] = autoMode ? "auto" : "manual";
    status_mode== Cooler_On ?coolingData["cooling_enable"] = "on" :  coolingData["cooling_enable"] = "off";
    coolingData["device"] = COOLING_DEVICE_NAME;
    emit sendFuncDataToJoshanSignal(coolingData);

}

void Cooling3::joshanStatusDataSlot()
{

    //    val->cooling_compressor =
    //    val->cooling_fan_currenrt =
    //    val->cooling_motor_current =

    QJsonObject coolingData;
    coolingData["cooling_compressor"]    =   ui->lblComp_2->text().toDouble();
    coolingData["cooling_fan_current"]   =   ui->lblFan_2->text().toDouble();
    coolingData["cooling_motor_current"] =   ui->lblMotor_2->text().toDouble();
    coolingData["cooling_water_current"] =    ui->progWaterTemp->value();
    coolingData["cooling_lan_status"]    =   isCoolerConnected;

    emit sendStatusDataToJoshanSignal(coolingData);
}

void Cooling3::joshanControlDataSlot(QJsonObject val)
{

    QJsonObject value = val["parameters"].toObject();

    if(!value["mode"].isNull())
    {
        if(value["mode"].toString() == "manual")
            on_BtnManual_clicked();

        else on_BtnAuto_clicked();
    }

    if(!value["enable"].isNull())
    {

        if(value["enable"].toString() == "on")
            on_BtnTurnOnCooling_clicked();

        else  on_BtnTurnOffCooling_clicked();
    }


}

void Cooling3::getSolidTempSlot(int temp)
{
    if (!autoMode) return;


    if(temp < 19) on_BtnTurnOffCooling_clicked();
    else if (temp > 25) on_BtnTurnOnCooling_clicked();

}


void Cooling3::on_BtnAuto_clicked()
{
    autoMode = true;
    ui->BtnAuto->setStyleSheet("color:"+QString(BLACK)+";background-color:"+QString(YELLOW_GREEN));
    ui->BtnManual->setStyleSheet("background-color: "+QString(BLACK));
    ui->BtnTurnOnCooling->setVisible(false);
    ui->BtnTurnOffCooling->setVisible(false);
    //emit sendCommandToDeviceSignal("");
}

void Cooling3::on_BtnManual_clicked()
{
    autoMode = false;
    ui->BtnTurnOnCooling->setVisible(true);
    ui->BtnTurnOffCooling->setVisible(true);
    ui->BtnAuto->setStyleSheet("background-color:"+QString(BLACK));
    ui->BtnManual->setStyleSheet("color:"+QString(BLACK)+";background-color: "+QString(YELLOW_GREEN));

    //for the first time that user clicks on manual, we want to off fan and motor
    if(isCoolerConnected)
    {
        //        on_BtnTurnOffCooling_clicked();
    }

}

void Cooling3::on_BtnTurnOnCooling_clicked()
{
    status_mode = Cooler_On;
    uint8_t data[]={1,0,3,(uint8_t)packetManager::ON,(uint8_t)packetManager::ON,(uint8_t)packetManager::NONE};
    creatPacket(1,data,6,FC_WriteMultReg);

    ui->BtnTurnOffCooling->setStyleSheet("background-color:"+QString(BLACK));
    ui->BtnTurnOnCooling->setStyleSheet("color:"+QString(BLACK)+";background-color:"+QString(YELLOW_GREEN));
}


bool Cooling3::creatPacket(uint8_t slaveNumber,uint8_t *data,uint8_t dataLen,FC_t fc)
{
    // QByteArray::operator[] past the current size only warns (QByteRef) and
    // discards the byte, so the old dOut[index++] form sent EMPTY packets and
    // spammed "Using QByteRef with an index pointing outside the valid range".
    QByteArray dOut;

    dOut.append((char)(uint8_t)(_HEADER_>>8));
    dOut.append((char)(uint8_t)(_HEADER_));

    dOut.append((char)(uint8_t)(slaveNumber>>8));
    dOut.append((char)(uint8_t)(slaveNumber));

    dOut.append((char)(uint8_t)((dataLen+10)>>8)); // 10 = FixByte
    dOut.append((char)(uint8_t)(dataLen+10));

    dOut.append((char)fc);
    dOut.append((char)dataLen);

    for (int i = 0; i < dataLen; ++i)
    {
        dOut.append((char)data[i]);
    }
    dOut.append((char)(uint8_t)(_FOOTER_>>8));
    dOut.append((char)(uint8_t)(_FOOTER_));

    emit sendCommandToDeviceSignal(dOut);
    return true;
}


void Cooling3::on_BtnTurnOffCooling_clicked()
{
    status_mode = Cooler_Off;
    uint8_t data[]={1,0,3,(uint8_t)packetManager::OFF,(uint8_t)packetManager::OFF,(uint8_t)packetManager::NONE};
    creatPacket(1,data,6,FC_WriteMultReg);

    ui->BtnTurnOnCooling->setStyleSheet("background-color:"+QString(BLACK));
    ui->BtnTurnOffCooling->setStyleSheet("background-color:"+QString(RED));
}



void Cooling3::GuiUpdate(packetManager::CoolingBoard_t CoolingBoard)
{
    // ui->lblComp_2->setText(QString::number(0));

    if(CoolingBoard.COMPRESSOR.State==packetManager::ON)
    {
        ui->lblComp_2->setText(QString::number(CoolingBoard.COMPRESSOR.Current));
    }

    else if(CoolingBoard.COMPRESSOR.State==packetManager::OFF)
    {
        ui->lblComp_2->setText(QString::number(0));
    }

    if(CoolingBoard.MOTOR.State==packetManager::ON)
    {
        ui->lblMotor_2->setText(QString::number(CoolingBoard.MOTOR.Current));
    }

    else if(CoolingBoard.MOTOR.State==packetManager::OFF)
    {
        ui->lblMotor_2->setText(QString::number(0));
    }

    if(CoolingBoard.FAN.State==packetManager::ON)
    {
        ui->lblFan_2->setText(QString::number(CoolingBoard.FAN.Current));
    }

    else if(CoolingBoard.FAN.State==packetManager::OFF)
    {
        ui->lblFan_2->setText(QString::number(0));
    }




    if(CoolingBoard.COMPRESSOR.Error==packetManager::ER_OverCurrent)
    {
        ui->lblCompError->setText("Over Current");
    }
    else ui->lblCompError->setText(" ");

    if(CoolingBoard.FAN.Error==packetManager::ER_OverCurrent) ui->lblFanError->setText("Over Current");
    else ui->lblFanError->setText(" ");

    if(CoolingBoard.MOTOR.Error==packetManager::ER_OverCurrent) ui->lblMotorError->setText("Over Current");
    else ui->lblMotorError->setText(" ");




    QString danger = "QProgressBar::chunk: horizontal {border-radius: 3px; background: QLinearGradient(X1:0, y1:0.966136, x2:0, y2:0, stop:0.609721 rgba(242, 53, 53, 255), stop:0.691923 rgba(240, 151, 141, 252));border: .px solid black;}";

    ui->progWaterTemp->setStyleSheet(danger);
    ui->progWaterTemp->setValue(CoolingBoard.LevelMeter.valuePersent);
//    ui->lblWaterTemp->setText(QString::number(CoolingBoard.LevelMeter.valuePersent));
}


