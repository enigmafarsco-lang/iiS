#include "statuswidget.h"
#include "ui_statuswidget.h"

StatusWidget::StatusWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusWidget)
{
    ui->setupUi(this);

    connect(ui->chbJoshanAllowed, &QCheckBox::toggled, [&](bool checked)
    {
        emit isJoshanStop(checked);
    });
}

StatusWidget::~StatusWidget()
{
    delete ui;
}

void StatusWidget::On_DeviceResponseSlot(QByteArray)
{
}

void StatusWidget::connectionStatusSlot(bool connected)
{
    //    qInfo()<<"-----------------------> " << connected;
    BTcpLan *tcp = qobject_cast<BTcpLan*>(sender());
    if(tcp != nullptr)
    {

        if(tcp->getDeviceName() == SERVO_DEVICE_NAME)
        {
            if(connected)
                ui->btnServoConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnServoConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }

        else if(tcp->getDeviceName() == RFF_DEVICE_NAME)
        {
            if(connected)
                ui->btnRffBoxConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnRffBoxConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }

        else if(tcp->getDeviceName()==COOLING_DEVICE_NAME)
        {
            if(connected)
                ui->btnCoolingConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnCoolingConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }

        else if(tcp->getDeviceName()==TUNER_DEVICE_NAME)
        {
            if(connected)
                ui->btnTunerConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnTunerConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }

        else if(tcp->getDeviceName()==JOSHAN_DEVICE_NAME)
        {
            if(connected)
                ui->btnJoshanConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnJoshanConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }

        else if(tcp->getDeviceName()==COLIBR_DEVICE_NAME)
        {
            if(connected)
                ui->btnCalibrConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnCalibrConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }


        return;
    }

    TcpServer *tcpServer = qobject_cast<TcpServer*>(sender());
    if(tcpServer != nullptr)
    {
        if(connected)
            ui->btnOuterNodeConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
        else
            ui->btnOuterNodeConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));

        return;
    }
    UdpLan *udp = qobject_cast<UdpLan*>(sender());
    if(udp != nullptr)
    {
        if(udp->getDeviceName()==SOLID_DEVICE_NAME)
        {
            if(connected)
                ui->btnSolidConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
            else
                ui->btnSolidConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
        }
    }

}

void StatusWidget::receiveCommandFromOtherObjectSlot(QString)
{

}

void StatusWidget::exciterConnectionSlot(bool connected)
{
    if(connected)
        ui->btnExciterConnectionStatus->setStyleSheet("color:black;background-color:"+QString(YELLOW_GREEN));
    else
        ui->btnExciterConnectionStatus->setStyleSheet("color:white;background-color:"+QString(RED));
}



void StatusWidget::on_btnExciterConnectionStatus_clicked(bool checked)
{

}

void StatusWidget::on_btnSolidConnectionStatus_clicked(bool checked)
{
    ui->btnSolidConnectionStatus->setChecked((!checked));
}

void StatusWidget::on_btnServoConnectionStatus_clicked(bool checked)
{
    ui->btnServoConnectionStatus->setChecked((!checked));
}

void StatusWidget::on_btnPowerControlBoardConnectionStatus_clicked(bool checked)
{
}

void StatusWidget::on_btnGpsCompassConnectionStatus_clicked(bool checked)
{
}

void StatusWidget::on_btnOuterNodeConnectionStatus_clicked(bool checked)
{
    ui->btnOuterNodeConnectionStatus->setChecked((!checked));
}

void StatusWidget::on_btnRffBoxConnectionStatus_clicked()
{

}


void StatusWidget::on_checkBox_stateChanged(int arg1)
{
    if (ui->chbJoshanAllowed->isChecked())
    {

    }
    else {

    }

}

void StatusWidget::on_chbJoshanAllowed_toggled(bool checked)
{

}
