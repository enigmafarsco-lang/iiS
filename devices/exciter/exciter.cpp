
#include "exciter.h"
#include "ui_exciter.h"
#include <QRegularExpression>

Exciter::Exciter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::exciter)
{
    ui->setupUi(this);
    // Phase 5: let the user type arbitrary spot bandwidths (e.g. "40"),
    // not just the pre-filled combo entries.
    ui->cmbBW->setEditable(true);
    //    QPushButton * getBtn[] = {ui->btnGetCW,ui->btnGetWB,ui->btnDisableSpot,ui->btnStopSweep,ui->btnDisImpulse};
    QPushButton * setBtn[] = {ui->btnSetCW,ui->btnSetWB, ui->btnLoadSpot, ui->btnStartSweep, ui->btnLoadImpulse};

    ui->grpDacBuffer->setVisible(false);

    for (int i{};i < 5; i++)
    {
//        setBtn[i]->setEnabled(false);
        connect(setBtn[i], SIGNAL(clicked()),this,SLOT(setDataSlot()));
    }

    //    ui->tabWidget->setStyleSheet("QTabBar::tab:selected {background-color:" +QString(DARK_OLIVE_GREEN)+ ";}");

    //    connect(&exciterConnection, &QTimer::timeout, this, &Exciter::connectToDevice);

    //public some elements in order to access them and set their value with cart data
    this->spnCWPower =   ui->spnCWPower;
    spnSweepPower    =   ui->spnSweepPower;
    spnimpulsePower  =   ui->spnImpulsePower;
    spnWidePower     =   ui->spnWBPower;
    spnSpotPower     =   ui->spnSpotPower;

    spnCWFrq      = ui->spnCWFrq;
    spnSweepStep  = ui->spnSweepStep;
    spnSweepStop  = ui->spnSweepStopFrq;
    spnSweepStart = ui->spnSweepStartFrq;

    spnimpulseFrq  = ui->spnImpulseFrq;
    spnWideFrq     = ui->spnCWFrq;
    spnSpotFrq     = ui->spnSpotFrq;
    powerOn        = ui->chbPower;
    spnDelay       = ui->spnDelay;

    spnImpPw = ui->spnImpulsePulseWidth;
    spnImpPri = ui-> spnImpulsePRI;

    startBtn       = ui->btnStartSweep;
    stopBtn        = ui->btnStopSweep;

    btnDisableSpot  = ui->btnDisableSpot;
    btnSetCW        = ui->btnSetCW;
    btnSetWB        = ui->btnSetWB;
    btnLoadSpot     = ui->btnLoadSpot;
    btnDisableCW    = ui->btnDisableCW;
    btnDisableWB    = ui->btnDisableWB;
    btnStopSweep    = ui->btnStopSweep;
    btnDisImpulse   = ui->btnDisImpulse;
    btnStartSweep   = ui->btnStartSweep;
    btnLoadImpulse  = ui->btnLoadImpulse;


    connect(ui->spnCWFrq, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->lblCurrentFrq->setNum(ui->spnCWFrq->value());});
    connect(ui->spnCWPower, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->lblCurrentPower->setNum(ui->spnCWPower->value());});

    connect(ui->chbPower, &QCheckBox::stateChanged, [&](int val){

        ui->chbPower->setText(val ? "Power On":"Power Off");
        ui->chbPower->setStyleSheet(val? "color: #9acd32":"color: #ff0000");
    });



    //-------------------------------------------------------------------
    //spin box limitation
    if(DC_6_UPTO_8_12 == 0)
    {
        ui->spnCWFrq->setRange         (200,6000);
        ui->spnSpotFrq->setRange       (200,6000);
        ui->spnImpulseFrq->setRange    (200,6000);
        ui->spnSweepStopFrq->setRange  (200,6000);
        ui->spnSweepStartFrq->setRange (200,6000);
    }

    else
    {
        ui->spnCWFrq->setRange         (8000,12000);
        ui->spnSpotFrq->setRange       (8000,12000);
        ui->spnImpulseFrq->setRange    (8000,12000);
        ui->spnSweepStopFrq->setRange  (8000,12000);
        ui->spnSweepStartFrq->setRange (8000,12000);
    }

    //-------------------------------------------------------------------
    ui->chbPower->setVisible(false);
}

Exciter::~Exciter()
{
    delete ui;
}



//void Exciter::receiveIpAddressSlot(bool isConnect)
//{
////    ipAddress = ip;
////    connectToDevice();
//    if(isConnect && firstRun)
//    {
//        initConnection(isConnect);
//        getDataSlot();
//        firstRun = false;
//    }

//    else if(! isConnect)
//    {
//        initConnection(isConnect);
//        firstRun = true;
//    }
//}


void Exciter::connectToDevice()
{
    //    isConnect = adrvObj->connect(ipAddress);
    //globals::ctx=adrvObj->ctx;
}



void Exciter::joshanFuncDataSlot()
{
    QJsonObject exciterData;

    exciterData["exciter_enable"] = isConnect? "on" : "off";
    exciterData["exciter_frequency"] = ui->lblCurrentFrq->text().toInt();
    exciterData["exciter_power"] = ui->lblCurrentPower->text().toInt();
    QStringList p =  ui->cmbBW->currentText().split(" ");
    exciterData["exciter_band_width"] = p[0];
    exciterData["exciter_start_frequency"] = ui->spnSweepStartFrq->value();
    exciterData["exciter_stop_frequency"] = ui->spnSweepStopFrq->value();
    exciterData["exciter_sweep_time"] = ui->spnSweepStep->value();
    exciterData["exciter_pulse_width"] = ui->spnImpulsePulseWidth->value();
    exciterData["exciter_pri"] = ui->spnImpulsePRI->value();

    switch (currentTabState) {
    case CW:
        exciterData["exciter_mode"] = "cw";
        break;

    case Spot:
        exciterData["exciter_mode"] = "spot";
        break;

    case Sweep:
        exciterData["exciter_mode"] = "sweep";
        break;

    case Impulse:
        exciterData["exciter_mode"] = "impulse";
        break;

    case WB:
        exciterData["exciter_mode"] = "wideband";
        break;

    default:
        exciterData["exciter_mode"] = "null";
    }

    //    exciterData["mode"] = val;
    exciterData["device"] = EXCITER_DEVICE_NAME;

    emit sendFuncDataToJoshanSignal(exciterData);
}

void Exciter::joshanStatusDataSlot()
{

}


void Exciter::initConnection(bool val)
{

    isConnect = val;
    ui->btnConnect->setStyleSheet(val?"background-color:"+QString(YELLOW_GREEN) :"background-color:"+QString(RED));
    ui->btnConnect->setText(val?"Connected":"Disconnect");
    ui->btnConnect->setVisible(false);
    //    emit connectionStatusSignal(val);
    //    ui->tabWidget->setEnabled(val);
}

void Exciter::addingCartElementsToExciter()
{
    //    QGroupBox *grbRX1 = ui->groupBox_2;
    //    QGridLayout *gridRX2 = grbRX1->findChild<QGridLayout *>();

    //    gridRX2->addWidget( , 0 , 1)             ;
}

void Exciter::changingDacMsg(QString dacMsg,QString mode)
{
    ui->lblDacMsg->setText(mode  + " " + dacMsg);
}

void Exciter::joshanControlDataSlot(QJsonObject val)
{
    QJsonObject value = val["parameters"].toObject();
    QString type = val["type"].toString();
    //    bool power = val["power"].toBool();

    if(type == "control_exciter_set_mode")
    {

        if(!value["mode"].isNull())
        {


            if(value["mode"].toString() == "cw_dis")
            {
                on_btnDisableCW_clicked(false);
            }

            else if(value["mode"].toString() == "spot_dis")
            {
                on_btnDisableSpot_clicked();
            }

            else if(value["mode"].toString() == "sweep_dis")
            {
                on_btnStopSweep_clicked();
            }

            else if(value["mode"].toString() == "impulse_dis")
            {
                on_btnDisImpulse_clicked();
            }

            else if(value["mode"].toString() == "wideband_dis")
            {
                on_btnDisableWB_clicked(false);
            }




            if(!value["power_on"].isNull())
            {
                //                ui->chbPower->stateChanged(val["power_on"].toInt());
            }


            if(value["mode"].toString() == "cw")
            {
                ui->tabWidget->setCurrentIndex(0);

                if(!value["frequency"].isNull())
                    ui->spnCWFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnCWPower->setValue(value["power"].toDouble());

                ui->btnSetCW->clicked();
            }



            else if(value["mode"].toString() == "spot")
            {
                ui->tabWidget->setCurrentIndex(1);

                if(!value["frequency"].isNull())
                    ui->spnSpotFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnSpotPower->setValue(value["power"].toDouble());

                if(!value["band_width"].isNull())
                {
                    int num = value["band_width"].toInt();

                    if(     num == 10  ||
                            num == 30  ||
                            num == 50  ||
                            num == 70  ||
                            num == 100 ||
                            num == 200 ||
                            num == 300 ||
                            num == 400 ||
                            num == 500 ||
                            num == 600 ||
                            num == 700 ||
                            num == 800 ||
                            num == 900 ||
                            num == 1000 )
                    {
                        ui->cmbBW->setCurrentText(QString::number(num)+" MHz");
                    }
                }

                ui->btnLoadSpot->clicked();
            }




            else if(value["mode"].toString() == "sweep")
            {
                ui->tabWidget->setCurrentIndex(2);

                if(!value["start_frequency"].isNull())
                    ui->spnSweepStartFrq->setValue(value["start_frequency"].toDouble());

                if(!value["stop_frequency"].isNull())
                    ui->spnSweepStopFrq->setValue(value["stop_frequency"].toDouble());

                if(!value["step_frequency"].isNull())
                    ui->spnSweepStep->setValue(value["step_frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnSweepPower->setValue(value["power"].toDouble());

                ui->btnStartSweep->clicked();
            }



            else if(value["mode"].toString() == "impulse")
            {
                ui->tabWidget->setCurrentIndex(3);

                if(!value["frequency"].isNull())
                    ui->spnImpulseFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnImpulsePower->setValue(value["power"].toDouble());

                if(!value["pri"].isNull())
                    ui->spnImpulsePRI->setValue(value["pri"].toDouble());

                if(!value["pulse_width"].isNull())
                    ui->spnImpulsePulseWidth->setValue(value["pulse_width"].toDouble());

                ui->btnLoadImpulse->clicked();
            }




            else if(value["mode"].toString() == "wideband")
            {
                ui->tabWidget->setCurrentIndex(4);

                if(!value["power"].isNull())
                    ui->spnWBPower->setValue(value["power"].toDouble());
                ui->btnSetWB->clicked();
            }
        }
    }

    else
    {


    }
}

void Exciter::smartNoiseIsActiveSlot()
{
    ui->lblDacMsg->setText("Smart noise is active.");
}

void Exciter::isUserLoggedInSlot(double state)
{
    isUserLoggedIn = state;
    for(int i{}; i < 5; i++)
    {
//         setBtn[i]->setEnabled(true ? state==true: false);
        ui->btnSetCW->setEnabled(state);
        ui->btnSetWB->setEnabled(state);
        ui->btnLoadSpot->setEnabled(state);
        ui->btnLoadImpulse->setEnabled(state);
        ui->btnStartSweep->setEnabled(state);
    }
}

//void Exciter::limitationValueSlot(double minFrq, double maxFrq)
//{
//    minFrqLimit = minFrq;
//    maxFrqLimit = maxFrq;
//}

bool Exciter::returnfilePath(QString &fileName)
{
    fullPath =  QDir::currentPath() + "/" + fileName;

    if(!existsFile(fullPath.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText("File doesn't exist.");
        msgBox.exec();
        return false;
    }

    return  true;
}



void Exciter::getDataSlot()
{
    //set frq
    //    ui->spnCWFrq->setValue(adrvObj->GetFrequency());
    //    ui->spnSpotFrq->setValue(adrvObj->GetFrequency());
    //    ui->spnImpulseFrq->setValue(adrvObj->GetFrequency());
    //    ui->lblCurrentFrq->setText("Current Frq: " + QString::number(adrvObj->GetFrequency()));

    //    //set power
    //    ui->spnCWPower->setValue(adrvObj->GetPower());
    //    ui->spnWBPower->setValue(adrvObj->GetPower());
    //    ui->spnSpotPower->setValue(adrvObj->GetPower());
    //    ui->spnSweepPower->setValue(adrvObj->GetPower());
    //    ui->spnImpulsePower->setValue(adrvObj->GetPower());
    //    ui->lblCurrentPower->setText("Current Power: " + QString::number(adrvObj->GetPower()));
}


bool Exciter::existsFile (const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

void Exciter::setProfileBw(int bwMHz)
{
    // Phase 5: active ADRV9009 profile from the receiver Profile tab.
    if (bwMHz > 0)
        profileBw = bwMHz;
}


void Exciter::setModeActive(const QString &mode, bool on)
{
    if (on)
        activeModes.insert(mode);
    else
        activeModes.remove(mode);

    emit modeActivitySignal(!activeModes.isEmpty());
}

void Exciter::setDataSlot()
{


    //    if(!ui->chbPower->isChecked())
    //    {
    //        QMessageBox msgBox;
    //        msgBox.setText("Power is off, please turn it on. ");
    //        msgBox.exec();
    //    }


    //    else
    //    {




    //    ui->lblDacMsg->setText("Loading...");

    switch (ui->tabWidget->currentIndex())
    {

    //==============================CW=========================================
    case 0:
    {
        if ( ui->spnCWFrq->value() >= minFrqLimit and  ui->spnCWFrq->value() < maxFrqLimit ) return;

        emit turnOffSmartNoiseSignal();
        currentTabState = CW;
        setModeActive("cw", true);
        changeDacSignal("set-cw");
        QTimer::singleShot(500,  [&]{emit sendFrqDataSignal(QString::number(ui->spnCWFrq->value()));});
        QTimer::singleShot(500,  [&]{emit sendPowerToCart(ui->spnCWPower->value());});
        isExciterOn = true;
        break;
    }


        //===========================Spot=======================================
    case 1:
    {
        if ( ui->spnSpotFrq->value() >= minFrqLimit and  ui->spnSpotFrq->value() < maxFrqLimit ) return;
        emit turnOffSmartNoiseSignal();
        currentTabState = Spot;
        setModeActive("spot", true);

        //            adrvObj->SetPower(ui->spnSpotPower->value());
        //            adrvObj->SetFrequency(ui->spnSpotFrq->value());
        //            getDataSlot();
        emit sendFrqDataSignal(QString::number(ui->spnSpotFrq->value()));

        // Phase 5: prefer the profile-specific band-limited noise file
        // spot{N}mhz_{P}.txt (P = active ADRV9009 profile bandwidth, set
        // from the receiver Profile tab via setProfileBw()). N is the
        // numeric MHz value typed/selected in cmbBW. Falls back to the
        // legacy spot{N}mhz.txt when the profile file is not present.
        // Only the file name selection changes here; the DAC load path
        // (returnfilePath / sendFileToCardSignal) below is untouched.
        double spotBwMhz = 0.0;
        const QRegularExpression spotBwRx("^\\s*([0-9]+(?:\\.[0-9]+)?)");
        const QRegularExpressionMatch spotBwM = spotBwRx.match(ui->cmbBW->currentText());
        if (spotBwM.hasMatch())
            spotBwMhz = spotBwM.captured(1).toDouble();

        if (spotBwMhz > 0.0)
        {
            const QString profileFile =
                QString("spot/spot%1mhz_%2.txt")
                    .arg(spotBwMhz, 0, 'f', 0).arg(profileBw);
            const QString legacyFile =
                QString("spot/spot%1mhz.txt")
                    .arg(spotBwMhz, 0, 'f', 0);
            if (existsFile((QDir::currentPath() + "/" + profileFile).toStdString()))
                fileName = profileFile;
            else
                fileName = legacyFile;
        }
        else
        {
            fileName = ("spot/spot" + ui->cmbBW->currentText().toLower().replace(" ", "")+ ".txt");
        }
        //            QString fileName = ("spot/iio/msk_20M.txt");

        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0,"spot");

        //            try
        //            {
        //            QString mess = adrvObj->SetFile(fullPath,0);
        //                ui->lblStatus->setText("Status: "+mess);

        //                if(mess.contains("successfully")){
        //                    ui->btnLoadSpot->setStyleSheet("background-color:#186a3b");
        //                    ui->btnDisableSpot->setStyleSheet("background-color:red");
        //                }
        //                else {
        //                    ui->btnLoadSpot->setStyleSheet("background-color:red");
        //                    ui->btnDisableSpot->setStyleSheet("background-color:red");
        //                }
        //            }
        //            catch (...)
        //            {
        //                QMessageBox msgBox;
        //                msgBox.setText("Something went wrong! Try again");
        //                msgBox.exec();
        //                return;
        //            }
        isExciterOn = true;
        break;
    }

        //===========================Sweep=====================================
    case 2:
    {
        if ( ui->spnSweepStartFrq->value() >= minFrqLimit and  ui->spnSweepStartFrq->value() < maxFrqLimit ) return;
        if ( ui->spnSweepStopFrq->value() >= minFrqLimit and  ui->spnSweepStopFrq->value() < maxFrqLimit ) return;

        emit turnOffSmartNoiseSignal();
        currentTabState = Sweep;
        setModeActive("sweep", true);
        changeDacSignal("set-sweep");
        QTimer::singleShot(100, [&]{emit sendStartFrqToCart(ui->spnSweepStartFrq->value());});
        QTimer::singleShot(100, [&]{emit sendStoptFrqToCart(ui->spnSweepStopFrq->value());});
        QTimer::singleShot(100, [&]{emit sendStepFrqToCart(ui->spnSweepStep->value());});
        QTimer::singleShot(100, [&]{emit sendPowerToCart(ui->spnSweepPower->value());});

        QTimer::singleShot(600, [&]{emit startHopp();}); //when user click on start in sweep ecxiter, first stop capturing
        //        ui->btnStartSweep->setStyleSheet("background-color:#186a3b");
        //        ui->btnStopSweep->setStyleSheet("background-color:black");
        isExciterOn = true;
        break;
    }


        //==============================Impulse===================================
    case 3:
    {
        if ( ui->spnImpulseFrq->value() >= minFrqLimit and  ui->spnImpulseFrq->value() < maxFrqLimit ) return;
        emit turnOffSmartNoiseSignal();
        //            changeDacSignal("impulse");
        currentTabState = Impulse;
        setModeActive("impulse", true);
        //            adrvObj->SetPower(ui->spnImpulsePower->value());
        //            adrvObj->SetFrequency(ui->spnImpulseFrq->value());
        //            getDataSlot();

        emit sendFrqDataSignal(QString::number(ui->spnImpulseFrq->value()));
        double pri = (ui->spnImpulsePRI->value() * 2000) / 4.1;
        double pw = (ui->spnImpulsePulseWidth->value() * 2000) / 4.1;

        QString impulseFileName = "Impulse.txt";
        createImpulseFile(pri,pw,impulseFileName);

        if (!returnfilePath(impulseFileName)) return;
        fileName = "Impulse.txt";
        emit sendFileToCardSignal(fileName, 0,"impulse");
        isExciterOn = true;
        //            fileName = "pls.txt";
        //            emit sendFileToCardSignal(fileName, 0,"impulse");
        try
        {

            //                QString mess = adrvObj->SetFile(fullPath,0);
            //                ui->lblImpulseStatus->setText("Status: "+mess);

            //                if(mess.contains("successfully")){
            //                    ui->btnLoadImpulse->setStyleSheet("background-color:#186a3b");
            //                    ui->btnDisImpulse->setStyleSheet("background-color:red");
            //                }
            //                else {
            //                    ui->btnLoadImpulse->setStyleSheet("background-color:red");
            //                    ui->btnDisImpulse->setStyleSheet("background-color:red");
            //                }
        }
        catch (...)
        {
            QMessageBox msgBox;
            msgBox.setText("Something went wrong! Try again");
            msgBox.exec();
            return;
        }


        break;
    }


        //=============================WB==========================================
    case 4:
    {
        emit turnOffSmartNoiseSignal();
        //            changeDacSignal("we");
        currentTabState = WB;
        setModeActive("wb", true);
        //            adrvObj->SetPower(ui->spnWBPower->value());
        //            getDataSlot();
        QString fileName = ("spot/widebandnoise.txt");

        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0,"wb");
        break;
    }

    default:
        break;
    }

    //    ui->lblDacMsg->setText("...");
    //    }



}


void Exciter::createImpulseFile(double pri, double pw, QString &impulseFileName)
{
    uint iValue{}, qValue{};

    QFile file(impulseFileName);

    if(file.exists())
    {
        QFile::remove(impulseFileName);
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out<< "TEXT\n";

        for(uint i{}; i < uint(pri); i++)
        {
            if(i < pw)
            {
                iValue = 1;
                qValue = 1;
                out<<iValue <<" "<<qValue <<"\n";
            }

            else
            {
                iValue = 0;
                qValue = 0;
                out<<iValue <<" "<<qValue<<"\n";
            }
        }

        file.close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Somthing went wrong, Try again.");
        msgBox.exec();
        return;
    }
}


void Exciter::on_btnStopSweep_clicked()
{
    QTimer::singleShot(1000, [&]{emit stoptHopp();});
    setModeActive("sweep", false);
    //    ui->btnStartSweep->setStyleSheet("background-color:black");
    //    ui->btnStopSweep->setStyleSheet("background-color:black");
}


void Exciter::on_btnDisableSpot_clicked()
{

    //    adrvObj->DisableDac();
    if (isExciterOn) emit changeDacSignal("Spot");
    setModeActive("spot", false);
    //    ui->lblSpotStatus->setText("Status: disable");
    //    ui->btnDisableSpot->setStyleSheet("background-color:#186a3b");
    //    ui->btnLoadSpot->setStyleSheet("background-color:#d30000");
    //    ui->btnLoadSpot->setStyleSheet("background-color:");
}

void Exciter::on_btnDisImpulse_clicked()
{    if (isExciterOn) emit changeDacSignal("Impulse");
    setModeActive("impulse", false);
    //     ui->lblImpulseStatus->setText("Status: disable");
    //     ui->btnDisImpulse->setStyleSheet("background-color:#186a3b");
    //     ui->btnLoadImpulse->setStyleSheet("background-color:red");
}


void Exciter::on_btnDisableCW_clicked(bool checked)
{
    if (isExciterOn) emit changeDacSignal("CW");
    setModeActive("cw", false);
}

void Exciter::on_btnDisableWB_clicked(bool checked)
{
    if (isExciterOn) emit changeDacSignal("WB");
    setModeActive("wb", false);
}
