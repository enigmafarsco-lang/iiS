#include "soliddialog.h"
#include "ui_soliddialog.h"

SolidDialog::SolidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolidDialog)
{
    ui->setupUi(this);

    initLedsIcon();

    initBlinkingTimer();

    connections();
}

SolidDialog::~SolidDialog()
{
    delete ui;
}

void SolidDialog::on_DeviceResponseSlot(QByteArray response)
{
    QString packet;
    QStringList frame;

    packet = response;
    frame = packet.split('@');

    for (int i=0;i<frame.length();i++)
    {
        if( (!frame[i].endsWith("$")) )
            continue;

        frame[i]= frame[i].replace("$", "");

        handle_info(frame[i]);
    }

}

void SolidDialog::handle_info(QString response)
{
    QString cmd = "";

    bool solidHealthyFlag = true;

    QStringList frame;

    //==================
    frame = response.split(',');
    //============

    if(frame[0]=="JamSolidInfo")
    {
        if(frame.count() != 9)
            return;
        //=================
        cmd = "SET,";
        QString status="";
        //============
        {
            ui->lblForwardPowerValue->setText(frame[1]);
            cmd = cmd + frame[1] + ",";
            ui->lblReversePowerValue->setText(frame[2]);
            ui->lblCurrentValue->setText(frame[3]);
            ui->lblVoltageValue->setText(frame[4]);
            ui->lblAmplifireTemperatureValue->setText(frame[5]);
            cmd = cmd + frame[5]+ ",";
            ui->lblDriverTemperatureValue->setText(frame[6]);
            ui->lblInternalTemperatureValue->setText(frame[7]);
            cmd = cmd + frame[7]+ ",";

            status = frame[8];
        }
        //=================
        QByteArray byteArray = status.toLatin1();

        bool ok = true;
        quint16 s = byteArray.toInt(&ok, 16);

        std::string status_bits = std::bitset<16>(s).to_string();

        QString result;

        int index = status_bits.length()-1;
        //========================
        if(status_bits[index] == '0')
        {
            linkStatus = 2;
            ui->ledLinkErrorLight->setColor(Qt::red);
            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '1')
        {
            linkStatus = 1;
            ui->ledLinkErrorLight->setColor(Qt::green);
        }
        index--;
        //================================

        if(status_bits[index] == '1' && status_bits[index-1] == '0')
        {
            gateLevelStatus = 3;
            ui->ledGateLevelErrorLight->setColor(Qt::green);
            gate_blinking = true;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '1')
        {
            gateLevelStatus = 2;
            ui->ledGateLevelErrorLight->setColor(Qt::red);
            gate_blinking = false;

            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '0')
        {
            gateLevelStatus = 1;
            ui->ledGateLevelErrorLight->setColor(Qt::green);
            gate_blinking = false;
        }
        index-=2;
        //======================================

        if(status_bits[index] == '1' && status_bits[index-1] == '1')
        {
            vswrStatus = 2;
            ui->ledVswrErrorLight->setColor(Qt::yellow);
            vswr_blinking = false;
        }
        else if(status_bits[index] == '1' && status_bits[index-1] == '0')
        {
            vswrStatus = 4;
            ui->ledVswrErrorLight->setColor(Qt::green);
            vswr_blinking = true;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '1')
        {
            vswrStatus = 3;
            ui-> ledVswrErrorLight->setColor(Qt::red);
            vswr_blinking = false;

            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '0')
        {
            vswrStatus = 1;
            ui->ledVswrErrorLight->setColor(Qt::green);
            vswr_blinking = false;
        }
        index-=2;
        //============================
        if(status_bits[index] == '1' && status_bits[index-1] == '0')
        {
            dcSupplyStatus = 3;
            ui->ledDcSupplyErrorLight->setColor(Qt::green);
            dcSupply_blinking = true;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '1')
        {
            dcSupplyStatus = 2;
            ui->ledDcSupplyErrorLight->setColor(Qt::red);
            dcSupply_blinking = false;

            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '0')
        {
            dcSupplyStatus = 1;
            ui->ledDcSupplyErrorLight->setColor(Qt::green);
            dcSupply_blinking = false;
        }
        index-=2;
        //================================
        if(status_bits[index] == '1')
        {
            internalLockStatus = 2;
            ui->ledInternalLockErrorLight->setColor(Qt::red);


            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0')
        {
            internalLockStatus = 1;
            ui->ledInternalLockErrorLight->setColor(Qt::green);
        }
        index--;
        //======================
        if(status_bits[index] == '1' && status_bits[index-1] == '1')
        {
            ui->ledTemperatureErrorLight->setColor(Qt::yellow);
            temp_blinking = false;
        }
        else if(status_bits[index] == '1' && status_bits[index-1] == '0')
        {
            ui->ledTemperatureErrorLight->setColor(Qt::green);
            temp_blinking = true;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '1')
        {
            ui-> ledTemperatureErrorLight->setColor(Qt::red);
            temp_blinking = false;

            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '0')
        {
            ui->ledTemperatureErrorLight->setColor(Qt::green);
            temp_blinking = false;
        }
        index-=2;
        //====================
        if(status_bits[index] == '1' && status_bits[index-1] == '1')
        {
            cmd = cmd + "H,";
            //ui->rdbHigh->setChecked(true);
        }
        else if(status_bits[index] == '1' && status_bits[index-1] == '0')
        {
            cmd = cmd + "L,";
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '1')
        {
            cmd = cmd + "M,";
        }
        else if(status_bits[index] == '0' && status_bits[index-1] == '0')
        {
            cmd = cmd + "U,";//undefined state
        }
        index-=2;
        //===============================
        if(status_bits[index] == '0')
        {
            cmd = cmd +"0,";
            //ui->chkAlcMode->setChecked(false);
        }
        else if(status_bits[index] == '1')
        {
            cmd = cmd + "1,";
            //ui->chkAlcMode->setChecked(true);
        }
        index--;
        //==========================
        if(status_bits[index] == '1')
        {
            fanStatus = 2;
            ui-> ledFanErrorLight->setColor(Qt::yellow);

            solidHealthyFlag = false;
        }
        else if(status_bits[index] == '0')
        {
            fanStatus = 1;
            ui->ledFanErrorLight->setColor(Qt::green);
        }
        index--;
        //==========================

        if(status_bits[index] == '1')
        {
            solidActive = true;
            ui->ledActivateLight->setColor(Qt::green);

            cmd = cmd + "1";
        }
        else if(status_bits[index] == '0')
        {
            solidActive = false;
            ui-> ledActivateLight->setColor(Qt::gray);

            cmd = cmd + "0";
        }

        //====================================
        emit(this->sendCommandToOtherObjectSignal(cmd));

        emit(this->sendCommandToOtherObjectSignal("SOLID_HEALTHY,"+QString::number(solidHealthyFlag)));
    }
    else if(frame[0].contains("ErrLatch"))
    {
        cmd = "SET_ERROR_LATCH,";
        if(response.contains("Enabled"))
        {
            cmd = cmd + "Enabled";
            emit(this->sendCommandToOtherObjectSignal(cmd));
        }
        else
        {
            cmd = cmd + "Disabled";
            emit(this->sendCommandToOtherObjectSignal(cmd));
        }
    }
    //    else if(frame[0].contains("EnableOk") || frame[0].contains("IsEnable")
    //            )
    else if(frame[0].contains("StartOk"))
    {
        ui->btnStartReport->setChecked(true);
        ui->btnStopReport->setChecked(false);
    }
    else if(frame[0].contains("StopOk"))
    {
        ui->btnStartReport->setChecked(false);
        ui->btnStopReport->setChecked(true);
    }

    //============
    int temp = ui->pgbSolid->value();
    temp = temp +5;
    if(temp> ui->pgbSolid->maximum())
    {
        temp = 0;
    }
    ui->pgbSolid->setValue(temp);
}
void SolidDialog::receiveCommandFromOtherObjectSlot(QString cmd)
{
    QStringList frame;
    //=================
    frame = cmd.split(',');
    //========
    if(frame[0] == "SOLID_OPEN_DIALOG")
    {
        this->show();
        //this->exec();
    }
    else if(frame[0] == "SOLID DIALOG ARE YOU OPEN")
    {
        emit(sendCommandToOtherObjectSignal("SOLID DIALOG IS,"+QString::number(this->isVisible())));
    }
}

void SolidDialog::joshanStatusDataSlot()
{

    QJsonObject solidVal;
    solidVal["solid_amplifire_temperature"] =  ui->lblAmplifireTemperatureValue->text().toDouble();
    solidVal["solid_internal_temperature"]  =  ui->lblInternalTemperatureValue->text().toDouble();
    solidVal["solid_driver_temperature"]    =  ui->lblDriverTemperatureValue->text().toDouble();
    solidVal["solid_forward_power"]         =  ui->lblForwardPowerValue->text().toDouble();
    solidVal["solid_reverse_power"]         =  ui->lblReversePowerValue->text().toDouble();
    solidVal["solid_current"]               =  ui->lblCurrentValue->text().toDouble();
    solidVal["solid_voltage"]               =  ui->lblVoltageValue->text().toDouble();
    solidVal["solid_internal_lock"]         =  internalLockStatus;
    solidVal["solid_gate_level"]            =  gateLevelStatus;
    solidVal["solid_dc_supply"]             =  dcSupplyStatus;
    solidVal["solid_vswr"]                  =  vswrStatus;
    solidVal["solid_serial_link"]           =  linkStatus;
    solidVal["solid_fan"]                   =  fanStatus;
    solidVal["solid_active"]                =  solidActive;
    solidVal["solid_lan_status"]            =  isDeviceConnected;


    emit sendStatusDataToJoshanSignal(solidVal);

}

void SolidDialog::isDeviceConnectedSlot(bool val)
{
    isDeviceConnected = val;
}



void SolidDialog::on_blinkingTimer_timeOut()
{
    if(vswr_blinking)
    {
        ui->ledVswrErrorLight->setVisible( (!ui->ledVswrErrorLight->isVisible()) );
    }

    if(dcSupply_blinking)
    {
        ui->ledDcSupplyErrorLight->setVisible( (!ui->ledDcSupplyErrorLight->isVisible()) );
    }

    if(gate_blinking)
    {
        ui->ledGateLevelErrorLight->setVisible( (!ui->ledGateLevelErrorLight->isVisible()) );
    }
    if(temp_blinking)
    {
        ui->ledTemperatureErrorLight->setVisible( (!ui->ledTemperatureErrorLight->isVisible()) );
    }
}

void SolidDialog::initBlinkingTimer()
{
    blinking_timer = new QTimer(this);
    blinking_timer->setInterval(500);
    blinking_timer->start();
}

void SolidDialog::connections()
{
    connect(blinking_timer, &QTimer::timeout, this, &SolidDialog::on_blinkingTimer_timeOut);
}

void SolidDialog::initLedsIcon()
{
    ui->ledActivateLight->setColor(Qt::yellow);
    ui->ledFanErrorLight->setColor(Qt::yellow);
    ui->ledLinkErrorLight->setColor(Qt::yellow);
    ui->ledVswrErrorLight->setColor(Qt::yellow);
    ui->ledDcSupplyErrorLight->setColor(Qt::yellow);
    ui->ledGateLevelErrorLight->setColor(Qt::yellow);
    ui->ledTemperatureErrorLight->setColor(Qt::yellow);
    ui->ledInternalLockErrorLight->setColor(Qt::yellow);
}



void SolidDialog::on_btnStartReport_clicked()
{
    emit sendToMainSig("SOLID_STOP_REPORT,FALSE");
}

void SolidDialog::on_btnStopReport_clicked()
{
    emit sendToMainSig("SOLID_STOP_REPORT,TRUE");
}

void SolidDialog::on_SolidDialog_finished(int result)
{
    emit(sendCommandToOtherObjectSignal("SOLID DIALOG IS,"+QString::number(false)));///*this->isVisible()*/
}

