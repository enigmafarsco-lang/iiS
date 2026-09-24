#include "rff.h"
#include "ui_rff.h"

RFF::RFF(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RFF)
{
    ui->setupUi(this);

    //this timer send a command to the RFF in order to get data from RFF
    sendStartTimer = new QTimer(0);
    connect(sendStartTimer, &QTimer::timeout, this, &RFF::sendStartSlot);
    sendStartTimer->start(750);

    ui->groupBox->setVisible(false);
}

RFF::~RFF()
{
    delete ui;
}


void RFF::on_BtnLnaOn_clicked()
{
    QString m_commandString = "<LnaOn:1>";
    QByteArray m_commandBytes = m_commandString.toLatin1();
    emit(sendCommandToDeviceSignal(m_commandBytes));
    QThread::currentThread()->msleep(50);
}

void RFF::on_BtnLnaOff_clicked()
{
    QString m_commandString = "<LnaOn:0>";
    QByteArray m_commandBytes = m_commandString.toLatin1();
    emit(sendCommandToDeviceSignal(m_commandBytes));
    QThread::currentThread()->msleep(50);
}



void RFF::on_RdbDbm_clicked()
{
    m_powerUnitDbm = true;
}

void RFF::sendStartSlot()
{
    emit sendStartSignal("<BIT:?>");
}

void RFF::joshanFuncDataSlot()
{
    QJsonObject rffData;
    lnaStatus==1 ? rffData["lna_enable"] = "on": rffData["lna_enable"] =  "off";
    rffData["device"] = RFF_DEVICE_NAME;
    emit sendFuncDataToJoshanSignal(rffData);
}

void RFF::joshanStatusDataSlot()
{
    QJsonObject rffData;

//    lnaStatus==1 ? rffData["lna_enable"] = "on": rffData["lna_enable"] =  "off";
    rffData["rff_temp"]  = ui->progTemp->value();
    rffData["rff_humid"] = ui->progHum->value();
    rffData["rff_lan_status"] = isDeviceConnected;
    emit sendStatusDataToJoshanSignal(rffData);
}

void RFF::isDeviceConnectedSlot(bool val)
{
    isDeviceConnected = val;
}

void RFF::joshanControlDataSlot(QJsonObject val)
{
    QJsonObject value = val["parameters"].toObject();

    if(!value["enable"].isNull())
    {
        if(value["enable"].toString() == "on")
            on_BtnLnaOn_clicked();
        else {
            on_BtnLnaOff_clicked();
        }

    }
}

//void RFF::joshanStatusDataSlot(joshanStatusStruct * val)
//{
//    val->lna_status =lnaStatus;
//}


void RFF::on_RdbWatt_clicked()
{
    m_powerUnitDbm = false;
}





void RFF::on_DeviceResponseSlot(QByteArray response)
{
    QString packet;

    QStringList frame;
    //clearing data from with space
    packet = response.simplified();
    packet =packet.replace(" ", "");


    if( !(packet[0] == "<")     ||
            ! packet.contains(">")  ||
            packet.length() < 3)
    {

        return;
    }

    frame = packet.split('>');


    for (int i=0;i < frame.length();i++)
    {
        handle_info(frame[i]);
    }
}

void RFF::handle_info(QString response)
{
    if(!response.isEmpty() && !response.isNull() && response.at(0) == "<" )
    {
        response = response.replace("<", "").toLower();
        QStringList frame;
        frame = response.split(',');

        for (int i=0;i < frame.length();i++)
        {
            //            std::cout  << i << " ==++++++++++++++++++++++++++++++++> "<< frame[i].toStdString()<< std::endl;
            QStringList temp = frame[i].split(':');
            if(temp[0].isEmpty() || temp[1].isEmpty()) return;
            QString key = temp[0];
            QString value = temp[1];

            if (key.compare("GpsLat",Qt::CaseInsensitive)==0)
            {
                //                std::cout  <<" GpsLat: "<< key.toUInt() << std::endl;
                ui->EdtLat->setText(value);
                UserType::gisLAt = value.toDouble();
            }

            if (key.compare("GpsLong",Qt::CaseInsensitive)==0)
            {
                //                std::cout  <<" GpsLong: "<< key.toUInt() << std::endl;
                ui->EdtLong->setText(value);
                UserType::gisLong = value.toDouble();
            }

            if (key.compare("CmpAngle",Qt::CaseInsensitive)==0)
            {
                //                std::cout  <<" CmpAngle: "<< key.toUInt() << std::endl;
                emit(sendCommandToObjectSignal("CMPANGLE,"+value));

                double ang = value.toDouble() + UserType::compassPanOffset;
                ui->EdtPan->setText(QString::number(ang,'f',2));
            }

            if (key.compare("CmpPitch",Qt::CaseInsensitive)==0)
            {
                //                std::cout  <<" CmpPitch: "<< value.toUInt() << std::endl;
                double ang = value.toDouble() + UserType::compassPitchOffset;
                ui->EdtPitch->setText(QString::number(ang,'f',2));
            }

            if (key.compare("PowTx",Qt::CaseInsensitive)==0)
            {
                onPowTx(value);
            }

            if (key.compare("HumInGun",Qt::CaseInsensitive)==0)
            {
                double hum = value.toDouble();
                //                std::cout <<"==================> " <<" hum: "<< hum << std::endl;
                //                ui->gauge->setValue(value.toDouble());
                if (hum <= HUMMID_WARNING_THRESHOLD)
                {
                    ui->progHum->setStyleSheet("QProgressBar::chunk{background-color:"+QString(DARK_OLIVE_GREEN)+"}");
                }
                else if (hum > HUMMID_WARNING_THRESHOLD && hum <= HUMMID_CRITICAL_THRESHOLD)
                {
                    ui->progHum->setStyleSheet("QProgressBar::chunk{background-color:yellow} QProgressBar {color:black;}");
                    // ui->progHum->setStyleSheet("QProgressBar::Text{color:black;}");

                }
                else if (hum > HUMMID_CRITICAL_THRESHOLD )
                {
                    ui->progHum->setStyleSheet("QProgressBar::chunk{background-color:red}");
                }
                ui->progHum->setValue(hum);
                QString s = QString::number(hum, 'f', 1) + " <strong>%</strong>";
                ui->lblHum->setText(s);

            }

            if (key.compare("TmpInGun",Qt::CaseInsensitive)==0)
            {
                double tempreture = value.toDouble();

                if (tempreture <= TEMP_WARNING_THRESHOLD)
                {
                    ui->progTemp->setStyleSheet("QProgressBar::chunk{background-color:#2efe2e}");
                }
                else if (tempreture > TEMP_WARNING_THRESHOLD && tempreture <= TEMP_CRITICAL_THRESHOLD)
                {
                    ui->progTemp->setStyleSheet("QProgressBar::chunk{background-color:yellow} QProgressBar {color:black;}");

                }
                else if (tempreture > TEMP_CRITICAL_THRESHOLD )
                {
                    ui->progTemp->setStyleSheet("QProgressBar::chunk{background-color:red}");
                }
                ui->progTemp->setValue(tempreture);
                QString s = QString::number(tempreture,'f',1) +QChar(0x00B0) + "<sup>C</sup>";

                ui->lblTemp->setText(s);

            }

            if (key.compare("LnaOn",Qt::CaseInsensitive)==0)
            {
                //                std::cout  <<" =============>LanOn: "<< key.toUInt() << std::endl;
                if (value.toUInt() == 1)
                {
                    ui->BtnLnaOn->setStyleSheet("color:"+QString(BLACK)+";background-color:"+QString(YELLOW_GREEN));
                    ui->BtnLnaOff->setStyleSheet("background-color:"+QString(BLACK));
                    //                    ui->LED->setColor(Qt::green);
                }
                else if (value.toUInt() == 0) {
                    ui->BtnLnaOn->setStyleSheet("background-color:"+QString(BLACK));
                    ui->BtnLnaOff->setStyleSheet("background-color:"+QString(RED));
                }
                lnaStatus = value.toUInt();
            }
        }
    }
}

void RFF::onPowTx(QString val)
{
    if(ui->RdbWatt->isChecked()) //watt
    {
        ui->EdtPOwer->setText(dbmToWatt(val));
    }

    else                        //dbm
    {
        ui->EdtPOwer->setText(val);
    }
}

QString RFF::dbmToWatt(QString val)
{
    double temp = val.toDouble();
    temp = temp/ 10.0;
    temp = pow(10.0,temp );
    temp = temp / 1000.0;
    return QString::number(temp, 'f', 2);
}

QString RFF::wattToDbm(QString val)
{
    if(!(val == "0"))
    {
        double temp = val.toDouble();
        temp = 10.0 * log10(temp * 1000.0);
        return QString::number(temp, 'f', 2);
    }
    else
        return "0";

}
