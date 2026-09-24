#include "solid.h"
#include "ui_solid.h"

Solid::Solid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Solid)
{
    ui->setupUi(this);

    {
        QFile file(":/resources/qss/mystylesheet.qss");
        file.open(QFile::ReadOnly);
        m_defaultStyleSheet = QString::fromLatin1(file.readAll());

    }

    {
        movie = new QMovie(":/resources/Images/Icons/Lamp.gif");
        ui->lblAlarm->setMovie(movie);
        movie->start();
        movie->stop();
    }
    blinkingTimer.setInterval(1000);
    blinkingTimer.setTimerType(Qt::PreciseTimer);
    connect(&blinkingTimer, &QTimer::timeout, this, &Solid::timeout);
}

Solid::~Solid()
{
    delete movie;
    delete ui;
}

void Solid::on_DeviceResponseSlot(QByteArray)
{

}

void Solid::joshanFuncDataSlot()
{
    QJsonObject solidData;
    solidData["solid_enable"] = ui->btnEnable->isChecked() ? "on" : "off";
    solidData["device"] = SOLID_DEVICE_NAME;
    emit sendFuncDataToJoshanSignal(solidData);
}

void Solid::joshanStatusDataSlot()
{
    QJsonObject solidData;
    solidData["solid_enable"] = ui->btnEnable->isChecked() ? "on" : "off";
    solidData["solid_forward_power"] = ui->lblForwardPowerValue->text();
    solidData["solid_reverse_power"] = ui->lblForwardPowerValue->text();
    solidData["device"] = SOLID_DEVICE_NAME;
    emit sendFuncDataToJoshanSignal(solidData);
}



void Solid::receiveCommandFromOtherObjectSlot(QString response)
{
    QStringList frame;

    //==================
    frame = response.split(',');



    if (frame[0]==("SET") && frame.length()==7)
    {
        ui->lblForwardPowerValue->setText(frame[1]);

        if(frame[2].toInt() <=25 && (!m_defaultStyleSheet.isEmpty()) )
        {

            ui->lblAmplifireTemperatureValue->setStyleSheet(m_defaultStyleSheet);
            ui->lblAmplifireTemperatureValue->setStyleSheet(m_defaultStyleSheet);
        }
        else if(frame[2].toInt()<=30)
        {
            //            ui->lblAmplifireTemperatureValue->setStyleSheet("color: black");
            ui->lblAmplifireTemperatureValue->setStyleSheet(warningStyle);
        }
        else
        {
            //            ui->lblAmplifireTemperatureValue->setStyleSheet("");
            ui->lblAmplifireTemperatureValue->setStyleSheet(dangerStyle);
        }
        ui->lblAmplifireTemperatureValue->setText(frame[2] + " " + QChar(0x00B0)+" <sup>C</sup>");
        emit(sendStatusTempSignal(frame[2].toInt()));

        //====
        if(frame[3].toInt() <=25 && (!m_defaultStyleSheet.isEmpty()) )
        {
            //            ui->lblInternalTemperatureValue->setStyleSheet(m_defaultStyleSheet);
        }
        else if(frame[3].toInt()<=30)
        {
            //            ui->lblInternalTemperatureValue->setStyleSheet("");
            ui->lblInternalTemperatureValue->setStyleSheet(warningStyle);
        }
        else
        {
            //            ui->lblInternalTemperatureValue->setStyleSheet("color: balck");
            ui->lblInternalTemperatureValue->setStyleSheet(dangerStyle);
        }
        ui->lblInternalTemperatureValue->setText(frame[3] + " " + QChar(0x00B0)+" <sup>C</sup>");

        if(frame[4]== "L")
            ui->rdbLow->setChecked(true);
        else  if(frame[4]== "M")
            ui->rdbMedium->setChecked(true);
        else if(frame[4]== "H")
            ui->rdbHigh->setChecked(true);
        else //if(frame[4]== "U")//Undefined
        {
            if(ui->rdbLow->isChecked())
                on_rdbLow_clicked();
            if(ui->rdbMedium->isChecked())
                on_rdbMedium_clicked();
            if(ui->rdbHigh->isChecked())
                on_rdbHigh_clicked();
        }

        //==================
        if(frame[5]=="0")
            ui->chkAlcMode->setChecked(false);
        else
            ui->chkAlcMode->setChecked(true);
        //==========================
        if(frame[6]=="0")
        {
            m_state = "OFF";
            ui->btnEnable->setChecked(false);
            ui->btnDisable->setChecked(true);
            ui->lblSolidStatus->setText("Solid is disabled");
            //            ui->lblSolidStatus->setStyleSheet("color:"+QString(BLACK));
            ui->btnEnable->setStyleSheet("background-color:"+QString(BLACK));
            ui->btnDisable->setStyleSheet("background-color:"+QString(RED));
            //            ui->btnEnable->setStyleSheet()

            //========
            movie->stop();
        }
        else if(frame[6]=="1")
        {
            m_state = "ON";
            emit(sendCommandToOtherObjectSignal("SOLID_STATUS,"+m_state));
            ui->btnEnable->setChecked(true);
            ui->btnDisable->setChecked(false);
            ui->lblSolidStatus->setText("Solid is enabled");
            //            ui->lblSolidStatus->setStyleSheet("color:"+QString(BLACK));
            ui->btnEnable->setStyleSheet("color:"+QString(BLACK)+";background-color:"+QString(YELLOW_GREEN));
            ui->btnDisable->setStyleSheet("background-color:"+QString(BLACK));
            //========
            emit(sendCommandToOtherObjectSignal("EXCITER_STATUS_FOR_ENABLE_MOVIE,?"));
        }

        //======send power to outerNode=====
        QJsonObject report;
        report["power"] = ui->lblForwardPowerValue->text().toDouble();

        emit(sendStatusToOuterNodeSignal(report));
    }
    else if(frame[0]=="SOLID_HEALTHY")
    {
        if(frame[1] == "1")
        {
            if(blinkingTimer.isActive())
            {
                blinkingTimer.stop();

                ui->btnSolidStatus->setIcon(QIcon());

                //==========
                emit(sendCommandToOtherObjectSignal("SOLID DIALOG ARE YOU OPEN,?"));

            }
        }
        else
        {
            if( (!blinkingTimer.isActive()) )
            {
                blinkingTimer.start();
                ui->btnSolidStatus->setIcon(QIcon(":/resources/rc/close-pressed.png"));
            }
        }
    }
    else if(frame[0]=="SOLID DIALOG IS")
    {
        if(frame[1]=="1")
        {
            ui->btnSolidStatus->setChecked(true);
        }
        else
        {
            ui->btnSolidStatus->setChecked(false);
        }
    }
    else if(frame[0] == "SET_ERROR_LATCH" && frame.length()==2)
    {
        if(frame[1]== "Enabled")
            ui->chkErrLacthMode->setChecked(true);
        else
            ui->chkErrLacthMode->setChecked(false);
    }

    else if(frame[0].compare("SOLID_STATUS",Qt::CaseInsensitive)==0)
    {
        emit(sendCommandToOtherObjectSignal("SOLID_STATUS,"+m_state));
    }
    else if(frame[0].compare("SOLID",Qt::CaseInsensitive)==0)
    {
        if(frame[1].compare("DISABLE",Qt::CaseInsensitive)==0)
        {
            on_btnDisable_clicked();
        }
    }
    else if(frame[0].compare("POWER_STATUS_SOLID",Qt::CaseInsensitive)==0)
    {
        if(frame[1].compare("TRANSMIT",Qt::CaseInsensitive)==0)
        {
            m_commandString = "@JamSolidEnable$";
            m_commandBytes = m_commandString.toLatin1();

            emit(sendCommandToDeviceSignal(m_commandBytes));
        }
        else if(frame[1].compare("RECEIVE")==0)
        {
            QMessageBox::critical(this->parentWidget(),"Error","Just in *Transmit Mode* Can Enable Solid.",QMessageBox::Ok);
        }
        else if(frame[1].compare("UNKNOWN")==0)
        {
            QString textErro = "<h2>System May be Damaged....</h2>";

            textErro = textErro +  "<br>Power Control Board Does Not Report State Or Invalid Report. Are You Sure That Power Control Board Is in **Transmit Mode**?";

            int ret = QMessageBox::critical(this->parentWidget(),"Error",textErro,QMessageBox::Yes |QMessageBox::No,QMessageBox::No);

            switch (ret) {
            case QMessageBox::Yes:
                m_commandString = "@JamSolidEnable$";
                m_commandBytes = m_commandString.toLatin1();

                emit(sendCommandToDeviceSignal(m_commandBytes));
                break;
            }
        }
    }
    else if(frame[0]=="EXCITER_STATUS_FOR_ENABLE_MOVIE")
    {
        if(frame[1]=="ON")
        {
            movie->start();
        }
        else
            movie->stop();
    }
}

void Solid::isDeviceConnectedSlot(bool conncted)
{
    isDeviceConnected = conncted;
    if( (!conncted) )
    {
        m_state = "UNKNOWN";
    }
}

void Solid::joshanControlDataSlot(QJsonObject val)
{

    QJsonObject value = val["parameters"].toObject();

    if(!value["enable"].isNull())
    {
        if(value["enable"].toString() == "on") on_btnEnable_clicked();
        else  on_btnDisable_clicked();
    }

    if(!value["level"].isNull())
    {
        if(value["level"].toString() == "low") on_rdbLow_clicked();
        else if(value["level"].toString() == "medium") on_rdbMedium_clicked();
        else  on_rdbHigh_clicked();
    }

    if(!value["alc"].isNull())
    {
        if(value["alc"].toBool() == true) on_chkAlcMode_clicked(true);
        else  on_chkAlcMode_clicked(false);
    }

    if(!value["error_latch"].isNull())
    {
        if(value["error_latch"].toBool() == true) on_chkErrLacthMode_clicked(true);
        else  on_chkErrLacthMode_clicked(false);
    }

    if(!value["reset"].isNull())
    {
        if(value["reset"].toBool() == true) on_btnReset_clicked();
    }
}

void Solid::timeout()
{
    ui->btnSolidStatus->setChecked( (!ui->btnSolidStatus->isChecked()) );
}

void Solid::on_chkAlcMode_clicked(bool checked)
{
    if(checked)
    {
        m_commandString =  "@JamSolidAlc,1$";
    }
    else
    {
        m_commandString = "@JamSolidAlc,0$";
    }
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_chkErrLacthMode_clicked(bool checked)
{
    if(checked)
    {
        m_commandString =  "@JamSolidErrLatch,1$";
    }
    else
    {
        m_commandString = "@JamSolidErrLatch,0$";
    }
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_btnEnable_clicked()
{
    //    ui->btnEnable->setChecked(false);

    //    emit(sendCommandToOtherObjectSignal("POWER_STATUS_SOLID,?"));
    ui->btnDisable->setChecked(false);

    m_commandString = "@JamSolidEnable$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_btnDisable_clicked()
{
    ui->btnDisable->setChecked(false);

    m_commandString = "@JamSolidDisable$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_btnReset_clicked()
{
    m_commandString = "@JamSolidReset$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_btnSolidStatus_clicked()
{
    ui->btnSolidStatus->setChecked(true);

    m_commandString = "SOLID_OPEN_DIALOG";
    emit(sendCommandToOtherObjectSignal(m_commandString));
}

void Solid::on_rdbHigh_clicked()
{
    m_commandString = "@JamSolidPowerLvl,H$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_rdbMedium_clicked()
{
    m_commandString = "@JamSolidPowerLvl,M$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

void Solid::on_rdbLow_clicked()
{
    m_commandString = "@JamSolidPowerLvl,L$";
    m_commandBytes = m_commandString.toLatin1();

    emit(sendCommandToDeviceSignal(m_commandBytes));
}

//:/resources/Images/Icons/alert_animated.gif



