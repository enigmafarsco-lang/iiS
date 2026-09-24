#include "tcplan.h"


BTcpLan::BTcpLan( AppConfiguration *cfg, QString _deviceName):m_config(cfg)
{
    m_tcpClient = nullptr;
    m_connectionTimer = nullptr;
    m_checkingConnectionTimer = nullptr;
    m_lastReceivePacketTime = QTime::currentTime();
    m_TimeCheckingInterval = 2000;


    //==============
    this->m_deviceName = _deviceName;
    m_loggingFormat = LoggingFormat::STRING;//default logging Format;

    m_tcpClient = new QTcpSocket(0);
    m_connectionTimer = new QTimer(0);
    m_checkingConnectionTimer = new QTimer(0);

    connect(m_connectionTimer, &QTimer::timeout,          this, &BTcpLan::openPort);
    connect(m_tcpClient,       &QTcpSocket::connected,    this, &BTcpLan::newConnection);
    connect(m_tcpClient,       &QTcpSocket::disconnected, this, &BTcpLan::closePort);


    start(HighestPriority);
    m_connectionTimer->start(2000);

    moveToThread(this);
    m_tcpClient->moveToThread(this);
    m_connectionTimer->moveToThread(this);
    m_checkingConnectionTimer->moveToThread(this);

}

BTcpLan::~BTcpLan()
{
    if(isRunning())
    {
        QThread::exit(0);
        QThread::wait();
    }
}

void BTcpLan::run()
{
    exec();
    m_tcpClient->deleteLater();
}

void BTcpLan::sendDataToDevice(QByteArray data)
{


    if(m_tcpClient->isOpen())
    {

        if(m_deviceName == COLIBR_DEVICE_NAME)
        {
             qInfo() << data;
        }
        qint64 bytesWritten = m_tcpClient->write(data);
//        qInfo() << bytesWritten;

    }



}

void BTcpLan::sendDataToDevice(QString data)
{
    QByteArray buffer = data.toLatin1();
    if(m_tcpClient->isOpen())
        qint64 bytesWritten = m_tcpClient->write(buffer);
}

void BTcpLan::newConnection()
{
//    if(m_deviceName==RFF_DEVICE_NAME)
//    {
//        sendDataToDevice(QString("<BIT:?>").toLatin1());
//    }

    disconnect(m_connectionTimer, &QTimer::timeout, this, &BTcpLan::openPort);
    m_connectionTimer->stop();

    connect(m_tcpClient, &QTcpSocket::readyRead, this, &BTcpLan::handelReadyread);
//    sleep(1);
    //==============
    emit(connectionStatusSignal(true));
    //================
    connect(m_checkingConnectionTimer, &QTimer::timeout, this, &BTcpLan::checkingConnectionAlive);
    m_checkingConnectionTimer->start(m_TimeCheckingInterval);
    //    m_connectionTimer->stop();
}

void BTcpLan::openPort()
{
    if(m_tcpClient==nullptr)
    {
        return;
    }

    QString ipAddress="";
    quint16 port=0;

    if(m_deviceName==RFF_DEVICE_NAME)
    {
        ipAddress = m_config->m_setting->RffIpAddress;
        port = m_config->m_setting->RffPort;

        if(m_config->m_setting->RffLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::STRING;
        }
        else if(m_config->m_setting->RffLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::HEX;
        }
    }


    if(m_deviceName==TUNER_DEVICE_NAME)
    {
        ipAddress = m_config->m_setting->tunerIpAddress;
        port = m_config->m_setting->tunerPort;

        if(m_config->m_setting->tunerLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::STRING;
        }
        else if(m_config->m_setting->tunerLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::HEX;
        }
    }

    if(m_deviceName == COOLING_DEVICE_NAME)
    {
        ipAddress = m_config->m_setting->coolingIpAddress;
        port = m_config->m_setting->coolingPort;

        if(m_config->m_setting->coolingLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::STRING;
        }
        else if(m_config->m_setting->coolingLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::HEX;
        }
    }


    if(m_deviceName == COLIBR_DEVICE_NAME)
    {
        ipAddress = m_config->m_setting->calibrationAddress;
        port = m_config->m_setting->calibrationPort;

//        if(m_config->m_setting->coolingLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
//        {
//            m_loggingFormat = LoggingFormat::STRING;
//        }
//        else if(m_config->m_setting->coolingLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
//        {
//            m_loggingFormat = LoggingFormat::HEX;
//        }
    }

    else if(m_deviceName==SERVO_DEVICE_NAME)
    {
        ipAddress = m_config->m_setting->servoIpAddress;
        port = m_config->m_setting->servoPort;

        if(m_config->m_setting->servoLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::STRING;
        }
        else if(m_config->m_setting->servoLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::HEX;
        }
    }


    //======================
    m_tcpClient->connectToHost(ipAddress,port);
}

void BTcpLan::closePort()
{
    connect(m_connectionTimer, &QTimer::timeout, this, &BTcpLan::openPort);
    m_connectionTimer->start(500);

    m_tcpClient->deleteLater();
    m_tcpClient = new QTcpSocket(0);
    m_tcpClient->moveToThread(this);
    connect(m_tcpClient, &QTcpSocket::connected, this, &BTcpLan::newConnection);
    connect(m_tcpClient, &QTcpSocket::disconnected, this,  &BTcpLan::closePort);

    emit(connectionStatusSignal(false));
}

void BTcpLan::handelReadyread()
{
    m_lastReceivePacketTime = QTime::currentTime();

    //==================
    QByteArray buffer;
    QByteArray d;
    buffer = m_tcpClient->readAll();


      if(m_deviceName == COLIBR_DEVICE_NAME)
      {
           qInfo() << buffer;
      }
    emit(DeviceResponseSignal(buffer));

    //=====================
    QString response = "";
    if(m_loggingFormat == LoggingFormat::STRING)
    {
        response = "Recieve From " + m_deviceName +":\t" + QString(buffer).simplified();
    }
    else   if(m_loggingFormat == LoggingFormat::HEX)
    {
        response = "Recieve From " + m_deviceName +":\t" + buffer.toHex(',').simplified();
    }

    QByteArray b = response.toLatin1();

    emit(savePacketToFileSignal(b));
}

void BTcpLan::on_sendCommandToDeviceSlot(QByteArray data)
{
//    qInfo() <<"===================================> " << data;
    this->sendDataToDevice(data);

    //===============
//    QString cmd = "";
//    if(m_loggingFormat == LoggingFormat::STRING)
//    {
//        cmd = "Send To " + m_deviceName +":\t" + QString(data).simplified();
//    }
//    else   if(m_loggingFormat == LoggingFormat::HEX)
//    {
//        cmd = "Send To " + m_deviceName +":\t" + data.toHex(',').simplified();
//    }

//    QByteArray buffer = cmd.toLatin1();

//    emit(savePacketToFileSignal(buffer));
}

void BTcpLan::checkingConnectionAlive()
{
    int diff_time = -1;
    QTime timeNow = QTime::currentTime();
    diff_time = (m_lastReceivePacketTime.msecsTo(timeNow));

    if(qAbs(diff_time) > m_TimeCheckingInterval)
    {
        disconnect(m_checkingConnectionTimer, &QTimer::timeout, this, &BTcpLan::checkingConnectionAlive);
        m_checkingConnectionTimer->stop();
        closePort();
    }
}

void BTcpLan::on_startStopTimerCheckingInterval(bool _started)
{
    if(_started && (!m_checkingConnectionTimer->isActive()) )
    {
        m_checkingConnectionTimer->start();
    }
    else//_started == false
    {
        if(m_checkingConnectionTimer->isActive())
            m_checkingConnectionTimer->stop();
    }
}

void BTcpLan::setTimerCheckingInterval(int value)
{
    m_TimeCheckingInterval = value;
}

QString BTcpLan::getDeviceName() const
{
    return m_deviceName;
}

void BTcpLan::setDeviceName(const QString &value)
{
    m_deviceName = value;
}

