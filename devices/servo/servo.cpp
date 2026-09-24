#include "servo.h"
#include "ui_servo.h"

Servo::Servo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Servo)
{
    ui->setupUi(this);
    init();

}

///#####################################
//######### Common functions ###########
///#####################################

void Servo::init()
{
    connect(this,&Servo::sendUdpCommandToTargetDetectionSignal, ui->targrtDetection,&TargetDetector::udpDeviceResponseSlot);
    connect(this,&Servo::sendTcpCommandToTargetDetectionSignal, ui->targrtDetection,&TargetDetector::tcpDeviceResponseSlot);
    connect(this,&Servo::sendCommandToObjectSignal, ui->targrtDetection,&TargetDetector::receiveCommandFromObjectSlot);
    comeType = lan;
    timerAutoSet.setInterval(3000);
    connect(&timerAutoSet, &QTimer::timeout, this, &Servo::updateAutoSet);

//    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+O"), parent);
//    QObject::connect(shortcut, SIGNAL(activated()), receiver, SLOT(yourSlotHere()));
}


int Servo::showMessage(QString meg, int status){

    QMessageBox msgBox;
    if(status == 0)
    {
        msgBox.setText(meg);
        msgBox.exec();

    }

    else if (status == 1){
        msgBox.setWindowTitle     ("Automating the rotator status!");
        msgBox.setInformativeText ("Are you sure to automated the rotator status?");
        msgBox.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton   (QMessageBox::Yes);
        return msgBox.exec();
    }

    return -1;
}

void Servo::on_btnPark_clicked()
{
    if (ui->chbGun->isChecked())
    {
        showMessage("Gun is parked", 0);
        return;
    }
    ui->dsbPanAngle->setValue(90);
    ui->dsbTiltAngle->setValue(0);
    ui->sbPanSpeed->setValue(2);
    ui->sbTiltSpeed->setValue(2);

    on_btnSetPan_clicked();

    connect(&timerPark, &QTimer::timeout, this, &Servo::checkingPanInPark);
    timerPark.start(500);
}



void Servo::setTilPanSpeed(int tiltSpeed , int panSpeed, QString status = "")
{
    if (ui->chbGun->isChecked())
    {
        showMessage("Gun is parked", 0);
        return;
    }
    try
    {
        QByteArray cmd = creatingSpeedCmd(tiltSpeed, panSpeed,  status);
        cmd = handleWildCharacter(cmd);

        if (comeType == lan)
        {
            emit(sendCommandToDeviceSignal(cmd));
            return;
        }
        else if(comeType == serial)
        {
            // this needs serial to send data
        }
    }

    catch (...)
    {
        showMessage("there is a problem! try again", 0);
    }

}

QByteArray Servo::creatingSpeedCmd(int temp1, int temp2, QString status)
{
    QByteArray byteList    = nullptr;

    std::string A;
    std::string B;
    byteList.append(0xff); // header 1
    byteList.append(0x01); // header 2
    byteList.append(uchar(0x00)); // header 3
    if(status == btnJog )
    {
        byteList.append(uchar(0x00)); // key code
    }
    else if (status == "")
    {
        byteList.append(0xa0); // key code
    }


    A = std::bitset<16>(0x8000 + 10 * temp2).to_string();
    B = std::bitset<16>(0x8000 + 10 * temp1).to_string();

    QString sA = QString::fromStdString(A);
    QString sB = QString::fromStdString(B);

    //slicing bits to low and high bits
    QString lowByte_A  = sA.left(8);
    QString highByte_A = sA.right(8);
    byteList.append(lowByte_A.toUInt(&ok,2));
    byteList.append(highByte_A.toUInt(&ok,2));

    QString lowByte_B  = sB.left(8);
    QString highByte_B = sB.right(8);
    byteList.append(lowByte_B.toUInt(&ok,2));
    byteList.append(highByte_B.toUInt(&ok,2));

    quint8 checksum = getCheckSum(byteList);
    byteList.append(checksum);
    return  byteList;
}

QByteArray Servo::creatingStopCmd()
{
    QByteArray byteList    = nullptr;

    std::string A;
    std::string B;
    byteList.append(0xff);        // header 1
    byteList.append(0x01);        // header 2
    byteList.append(uchar(0x00)); // header 3

    byteList.append((0xa1));     // key code

    byteList.append(uchar(0x00)); //data 1 high byte
    byteList.append(uchar(0x00)); //data 1 low byte

    byteList.append(uchar(0x00)); //data 1 high byte
    byteList.append(uchar(0x00)); //data 1 low byte

    quint8 checksum = getCheckSum(byteList);
    byteList.append(checksum);
    return  byteList;
}



//creating cmd message in order to send it to devices
QByteArray Servo::creatingCmd(double degree, QString state)
{
    QByteArray byteList    = nullptr;

    std::string A;
    std::string B;
    byteList.append(0xff); // header 1
    byteList.append(0x01); // header 2
    byteList.append(0x06); // header 3
    byteList.append(uchar(0x00)); // key code

    if(state == "tilt")
    {
        A = std::bitset<16>(0).to_string();
        B = std::bitset<16>(0x8000 + 50 * degree).to_string();
    }
    else
    {
        A = std::bitset<16>(0x8000 + 50 * degree).to_string();
        B = std::bitset<16>(0).to_string();
    }

    QString sA = QString::fromStdString(A);
    QString sB = QString::fromStdString(B);

    //slicing bits to low and high bits
    QString lowByte_A  = sA.left(8);
    QString highByte_A = sA.right(8);
    byteList.append(lowByte_A.toUInt(&ok,2));
    byteList.append(highByte_A.toUInt(&ok,2));

    QString lowByte_B  = sB.left(8);
    QString highByte_B = sB.right(8);
    byteList.append(lowByte_B.toUInt(&ok,2));
    byteList.append(highByte_B.toUInt(&ok,2));

    quint8 checksum = getCheckSum(byteList);
    byteList.append(checksum);
    return  byteList;
}


QByteArray Servo::handleWildCharacter(const QByteArray &byteArr)
{
    QByteArray tempArr;
    if (comeType == serial ) {
        return byteArr;
    }

    else

        QByteArray tempArr;

    for (int i = 0; i< byteArr.count(); i++ )
    {
        tempArr.append(byteArr[i]);
        bool isSame = Utils::compare_TowBytes(byteArr[i], 0xff);
        if (isSame)
        {
            tempArr.append(0xff);
        }
    }
    return  tempArr;
}

void Servo::on_btnStop_clicked()
{
    rotatorIsStop = true;
    try {

        QByteArray cmd = creatingStopCmd();
        cmd = handleWildCharacter(cmd);
        if (comeType == lan)
        {
            emit(sendCommandToDeviceSignal(cmd));
        }
        else if(comeType == serial)
        {
            // this needs serial to send data
        }

    } catch (...) {
        showMessage("there is a problem in buttun Stop!!!", 0);
    }

}



///#####################################
//############## Btn Jog ###############
///#####################################
// btn top
void Servo::on_btnJogUp_pressed()
{
    setTilPanSpeed(0, +5, btnJog);
}

void Servo::on_btnJogUp_released()
{
    on_btnStop_clicked();
}


//btn down
void Servo::on_btnJogDown_pressed()
{
    setTilPanSpeed(0, -3, btnJog);
}

void Servo::on_btnJogDown_released()
{
    on_btnStop_clicked();
}


//btn left
void Servo::on_btnJogLeft_pressed()
{
    setTilPanSpeed(-3, 0, btnJog);
}

void Servo::on_btnJogLeft_released()
{
    on_btnStop_clicked();
}


//btn right
void Servo::on_btnJogRight_pressed()
{
    setTilPanSpeed(3, 0, btnJog);
}

void Servo::on_btnJogRight_released()
{
    on_btnStop_clicked();
}

void Servo::checkingPanInPark()
{
    if ( (panPosDeg > 87) && (panPosDeg < 93) )
    {
        on_btnSetTilt_clicked();
        timerPark.stop();
    }
}

///#####################################
//########### Tilte functions ##########
///#####################################
///
void Servo::on_btnSetTilt_clicked()
{
    isTileStart = !isTileStart;

    if(!isTileStart)
    {
        on_btnStop_clicked();
        return;
    }

    double setTiltVal = ui->dsbTiltAngle->value(); //getting titl value
    bool temp = setTiltPosition(setTiltVal);
    if (temp)
    {
        //        ui->btnSetTilt->setStyleSheet("background-color:red");
    }
}





bool Servo::setTiltPosition(const double &tiltPosDeg){

    try
    {
        tiltDestination = tiltPosDeg;
        QByteArray cmd = getByteSetTilt(tiltPosDeg);
        cmd = handleWildCharacter (cmd);

        if (ui->chbGun->isChecked())
        {
            showMessage("Gun is parked",0 );
            return false;
        }

        if (comeType == lan)
        {
            emit(sendCommandToDeviceSignal(cmd));
        }
        else if(comeType == serial)
        {
            // this needs serial to send data
        }
        return true;
    }
    catch (...)
    {
        showMessage("there is a problem! try again", 0);
        return false;
    }

}



QByteArray Servo::getByteSetTilt  (const double &TiltPos){

    if (TiltPos > +70){
        showMessage("Tilt is not in range", 0);
    }
    if (TiltPos < -20){
        showMessage("Tilt is not in range", 0);
    }

    return creatingCmd(TiltPos, "tilt");
}

quint8 Servo::getCheckSum(const QByteArray &byteArr)
{
    quint8 checksum = 0;
    for(int i = 1; i < byteArr.count(); i ++)
    {
        checksum += byteArr[i];
    }
    return checksum;
}




///#####################################
//########### Pan functions ############
///#####################################
void Servo::on_btnSetPan_clicked()
{
    isPanStart = !isPanStart;

//    if(!isPanStart)
//    {
//        on_btnStop_clicked();
//        return;
//    }
    rotatorIsStop = false;
    setTilPanSpeed(ui->sbTiltSpeed->value(), ui->sbPanSpeed->value());
    //    QThread::currentThread()->msleep(50);

    double setPosVal = ui->dsbPanAngle->value();
    setPanPos(setPosVal, northOffset);
}

bool Servo::setPanPos(double &panPosDeg ,double &northOffset)
{
    try {
        panDestination = panPosDeg;
        QByteArray cmd =getCmdSetPanPos(panPosDeg + northOffset);
        cmd = handleWildCharacter (cmd);
        if (ui->chbGun->isChecked())
        {
            showMessage("Gun is parked", 0);
            return false;
        }

        if (comeType == lan)
        {
            emit(sendCommandToDeviceSignal(cmd));
        }
        else if(comeType == serial)
        {
            // this needs serial to send data
        }
        return true;


    } catch (...) {

        showMessage("Tilt is not in range", 0);
        return false;
    }
    return  true;
}

QByteArray Servo::getCmdSetPanPos(double panPos)
{
    if (panPos > 180)  panPos -=360;
    if (panPos < -180) panPos += 360;

    if(panPos > 180)  {
        showMessage("Pan is not in range", 0);
    }

    if(panPos < -180) {
        showMessage("Pan is not in range", 0);
    }

    return  creatingCmd(panPos, "Pan");
}


void Servo::on_btnSetPanSpeed_clicked()
{
    setTilPanSpeed(ui->sbTiltSpeed->value(), ui->sbPanSpeed->value());
}

void Servo::on_btnSetTiltSpeed_clicked()
{
    setTilPanSpeed(ui->sbTiltSpeed->value(), ui->sbPanSpeed->value());
}
///#####################################
//############ Chech Offset ############
///#####################################

void Servo::on_chbCheckoffset_stateChanged(int arg1)
{
    if(arg1)
    {
        offsetNorth = panPosDeg;
        checkOffsetFlag = true;
    }
}


void Servo::on_chbAutoSet_clicked(bool checked)
{
    if(checked)
    {
        int messageStatus = showMessage("",1);
        switch (messageStatus) {
        case QMessageBox::Yes:
            rotatorIsStop = false;
            ui->sbPanSpeed->setValue(2);
            ui->sbTiltSpeed->setValue(2);
            timerAutoSet.start();


            break;
        case QMessageBox::No:
            ui->chbAutoSet->setChecked(false);

            break;
        default:
            break;
        }
    }

    else {
        timerAutoSet.stop();
    }
}


void Servo::updateAutoSet()
{
    double targetPan  = 90;
    targetPan = coordinatorToAngle(UserType::gisLAt, UserType::gisLong, UserType::gisRadarLat, UserType::gisRadarLong);

    if (qAbs(targetPan - lastTarget) > 1)
    {
        rotatorIsStop = false;
        lastTarget = targetPan;
    }
    if(!rotatorIsStop)
    {
        on_btnAutoSet_clicked();
    }
    timerAutoSet.stop();
}


double Servo::degreeToRadian(double degree)
{
    return  (degree * PI / 180.0);
}

double Servo::radianToDegree(double degree)
{
    return  (degree * 180.0 / PI);
}

double Servo::coordinatorToAngle(double jammerLatitude, double jammerLongtitude, double targetLatitude, double targetLongitude )
{
    double longitudeDifferenceRadios = degreeToRadian(targetLongitude - jammerLongtitude);
    double latitude1Radian           = degreeToRadian(jammerLatitude);
    double latitude2Radian           = degreeToRadian(targetLatitude);

    double x = (cos(latitude1Radian) * sin(latitude2Radian)) - (sin(latitude1Radian) * cos(latitude2Radian)) * (cos(longitudeDifferenceRadios));
    double y = sin(longitudeDifferenceRadios) * cos(latitude2Radian);
    return radianToDegree(atan2(y,x));
}




Servo::~Servo()
{
    delete ui;
}


void Servo::on_btnAutoSet_clicked()
{
    emit(
    sendCommandToObjectSignal("SETBTNCLICKED,1"));

    if (ui->chbGun->isChecked())
    {
        showMessage("Gun is parked", 0);
        return;
    }

    double targetPan ;
    targetPan = coordinatorToAngle(UserType::gisLAt, UserType::gisLong, UserType::gisRadarLat, UserType::gisRadarLong);

    if(targetPan < 0)
    {
        targetPan = 360 + targetPan;

    }
    panInRange = true;
    if(!compareDouble(targetPan, ui->dsbPanAngle->value()))
    {
        try
        {
            ui->dsbPanAngle->setValue(targetPan);
        }

        catch (...)
        {
            panInRange = false;
        }
    }

    double temp1 = qAbs(panPosDeg - ui->dsbPanAngle->value());
    bool   temp2 = temp1 > UserType::Servo_PanThreshold;
    if(panInRange && temp2)
    {
        on_btnSetPan_clicked();
    }
}

bool Servo::compareDouble(double val1, double val2)
{
    return qAbs(val1 - val2) <= 0.01;
}

void Servo::on_chbCheckoffset_clicked(bool checked)
{
    if (checked)
    {
        offsetNorth = panPosDeg;
        checkOffsetFlag = true;
    }
}


void Servo::on_DeviceResponseSlot(QByteArray response){

    QString packet = response;
    //        std::cout<< "-----> " << packet.toStdString() <<std::endl;

    if(    response.isEmpty()
           || response.isNull()
           || (response.length() != 13)
           || !(Utils::compare_TowBytes (response[0], 0xeb) )
           || !(Utils::compare_TowBytes (response[1], 0x90) )  )
    {
        return;
    }

    //pan value
    QByteArray az_byte;
    az_byte.resize(2);
    az_byte[0] = response[2]; // low
    az_byte[1] = response[3]; // high
    double az_val = az_byte.toHex().toInt(nullptr, 16);
    az_val /= 100.0;
    panPosDeg = az_val;
    ui->gagePan->setValue(panPosDeg);
    ui->lcdPan->display(panPosDeg);

    //tilt value
    QByteArray el_byte;
    el_byte.resize(2);
    el_byte[0] = response[4]; // low
    el_byte[1] = response[5]; // high
    double el_val = el_byte.toHex().toInt(nullptr, 16);
    el_val /= 100.0;
    tiltPosDeg = el_val;
    ui->gageTilt->setValue(tiltPosDeg);
//    ui->dsbTiltAngle->setValue(tiltPosDeg);
    ui->lcdTilt->display(tiltPosDeg);
//    ui->dsbPanAngle->setValue(tiltPosDeg);

    if(panPosDeg == az_val
            && rotatorIsStop == false
            && ui->chbGun->isChecked() == false)
    {
        on_btnSetPan_clicked();
    }
    sendPanTiltToCommander(az_val, el_val);

    if(checkOffsetFlag)
    {
        panDestination = panDestination + offsetNorth;
    }

    if(ui->chbCheckoffset->isChecked()){
        panDestination = panDestination + offsetNorth;
    }

    panTiltStopEarlier();

    UserType::gis_azimuth = az_val;
    UserType::gis_elevation = el_val;
}

void Servo::sendPanTiltToCommander(double az_val, double el_val)
{

    if(ui->chbCheckoffset->isChecked()){
        if(rotatorNorthOffset < 0)
        {
            //             emit(sendCommandToDeviceSignal(cmd));
        }
    }
}

void Servo::receiveCommandFromObjectSlot(QString response)
{
    QStringList frame;
    frame = response.split(',');
    if (frame[0]==("CMPANGLE") && frame.length()==2)
    {
        if (ui->chbCheckoffset->isChecked())
        {
            double pan_0_360 = panPosDeg;
            if(pan_0_360 < 0){
                pan_0_360 += 360;
            }
            double offset = pan_0_360 - frame[1].toDouble();
            northOffset = offset +  UserType::servoPanTiltMechanicalOffset;
        }
    }
}

void Servo::joshanFuncDataSlot()
{
    QJsonObject servoData;
    servoData["pan"]          = ui->dsbPanAngle->value();
    servoData["pan_speed"]    = ui->sbPanSpeed->value();
    servoData["tilt"]         = ui->dsbTiltAngle->value();
    servoData["tilt_speed"]   = ui->sbTiltSpeed->value();
    servoData["servo_in_gun"] = ui->chbGun->isChecked();
    servoData["device"]       = SERVO_DEVICE_NAME;

    emit sendFuncDataToJoshanSignal(servoData);
}

void Servo::joshanStatusDataSlot()
{
    QJsonObject servoData;
    servoData["servo_lan_status"] = isDeviceConnected;
    emit sendStatusDataToJoshanSignal(servoData);
}

void Servo::isDeviceConnectedSlot(bool val)
{
    isDeviceConnected = val;
}

void Servo::joshanControlDataSlot(QJsonObject val)
{
    QJsonObject value = val["parameters"].toObject();


    if(!value["mode"].isNull())
    {
        if(value["mode"].toString() == "park")
        {
            on_btnPark_clicked();
            return;
        }

        if(value["mode"].toString() == "fixed")
        {
            //            on_btnStop_clicked();
            //            return;
        }

        if(value["mode"].toString() == "jog")
        {
            //            on_btnStop_clicked();
            //            return;
        }

        if(value["mode"].toString() == "stop")
        {
            on_btnStop_clicked();
            return;
        }
    }



    if(!value["pan"].isNull())
        ui->dsbPanAngle->setValue(value["pan"].toDouble());

    if(!value["pan_speed"].isNull())
        ui->sbPanSpeed->setValue(value["pan_speed"].toDouble());

    if(!value["tilt"].isNull())
        ui->dsbTiltAngle->setValue(value["tilt"].toDouble());

    if(!value["tilt_speed"].isNull())
        ui->sbTiltSpeed->setValue(value["tilt_speed"].toDouble());


    if(!value["direction"].isNull())
    {
        if(value["direction"].toString() == "up")
        {
            QTimer::singleShot(100, [=]{on_btnJogUp_pressed();});
            QTimer::singleShot(400, [=]{on_btnJogUp_released();});
        }

        else if(val["direction"].toString() == "down")
        {
            QTimer::singleShot(100, [=]{on_btnJogDown_pressed();});
            QTimer::singleShot(400, [=]{on_btnJogDown_released();});
        }

        else if(val["direction"].toString() == "left")
        {
            QTimer::singleShot(100, [=]{on_btnJogLeft_pressed();});
            QTimer::singleShot(400, [=]{on_btnJogLeft_released();});
        }

        else if(val["direction"].toString() == "right")
        {
            QTimer::singleShot(100, [=]{on_btnJogRight_pressed();});
            QTimer::singleShot(400, [=]{on_btnJogRight_released();});
        }
    }


}


void Servo::panTiltStopEarlier()
{


    if(UserType::panTiltStopEarlier.compare("yes", Qt::CaseInsensitive)==0){
        double diffPan = 0;
        if(panDestination <= 180)
        {
            diffPan = qAbs(panPosDeg - panDestination);
        }
        else {
            double panPosDeg360 = panPosDeg + 360;
            diffPan = qAbs(panPosDeg360 - panDestination);
        }

        if(diffPan < UserType::Servo_PanThreshold){
            on_btnStop_clicked();
            panDestination = 1000;
        }
        double diff_tilt = qAbs(tiltPosDeg - tiltDestination);
        if(diff_tilt < UserType::Servo_TiltThreshold){
            on_btnStop_clicked();
            tiltDestination = 1000;
        }

    }
}

