#include "udplan.h"

UdpLan::UdpLan( AppConfiguration *cfg, QString _deviceName):m_config(cfg)
{
    m_udpClient= nullptr;
    m_checkingConnectionTimer= nullptr;
    m_lastReceivePacketTime = QTime::currentTime();
    m_TimeCheckingInterval = 5000;
    m_loggingFormat = LoggingFormat::STRING;//default logging Format;

    //===============
    m_deviceName = _deviceName;
    m_udpClient = new QUdpSocket(0);
    m_checkingConnectionTimer = new QTimer(0);

    connect(m_checkingConnectionTimer, &QTimer::timeout, this, &UdpLan::checkingConnectionAlive);

    start(HighPriority);
    m_checkingConnectionTimer->start(m_TimeCheckingInterval);

    moveToThread(this);
    m_udpClient->moveToThread(this);
    m_checkingConnectionTimer->moveToThread(this);
}

UdpLan::~UdpLan()
{
    if(isRunning())
    {
        QThread::quit();
        QThread::wait();
    }
}

void UdpLan::run()
{
    exec();
    m_udpClient->deleteLater();
}

void UdpLan::sendDataToDevice(QByteArray data, bool loggingFlag)
{    
    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
        //    if(m_deviceName == "SOLID")
    {
        m_hostIp = QHostAddress(m_config->m_setting->solidIpAddress);
        m_hostPort = m_config->m_setting->solidPort;

        if(m_config->m_setting->solidLoggingFormat.compare("string",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::STRING;
        }
        else if(m_config->m_setting->solidLoggingFormat.compare("hex",Qt::CaseInsensitive) ==0)
        {
            m_loggingFormat = LoggingFormat::HEX;
        }
    }

    qint64 bytesWritten = m_udpClient->writeDatagram(data, m_hostIp, m_hostPort);

    //==========
    if(loggingFlag)
    {
        QString cmd = "";
        if(m_loggingFormat == LoggingFormat::STRING)
        {
            cmd = "Send To " + m_deviceName +":\t" + QString(data).simplified();
        }
        else   if(m_loggingFormat == LoggingFormat::HEX)
        {
            cmd = "Send To " + m_deviceName +":\t" + data.toHex(',').simplified();
        }

        QByteArray buffer = cmd.toLatin1();

        emit(savePacketToFileSignal(buffer));
    }
}

void UdpLan::sendDataToDevice(QString data)
{

}

void UdpLan::openPort()
{
    if(m_udpClient==nullptr)
    {
        return;
    }

    //=================
    quint16 port=0;
    bool connected = false;
    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
    {
        port = m_config->m_setting->solidPort;
    }

    //==========
    connected = m_udpClient->bind(port);

    if(connected)
    {
        this->newConnection();
    }
}

void UdpLan::newConnection()
{
    connect(m_udpClient, &QUdpSocket::readyRead, this, &UdpLan::readDatagram);

    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
    {
        sendDataToDevice(QString("@JamSolidStart$").toLatin1(), false);
    }
}

void UdpLan::closePort()
{
    m_udpClient->disconnectFromHost();
    m_udpClient->deleteLater();
    //=================
    m_udpClient = new QUdpSocket(0);
    m_udpClient->moveToThread(this);

    emit(connectionStatusSignal(false));
}

void UdpLan::readDatagram()
{
    m_lastReceivePacketTime = QTime::currentTime();

    QByteArray buffer;
    while(m_udpClient->hasPendingDatagrams())
    {
        buffer.resize(m_udpClient->pendingDatagramSize());
        m_udpClient->readDatagram(buffer.data(), buffer.size());
    }
    buffer.replace("\n","");
    buffer.replace("\r","");

    emit(DeviceResponseSignal(buffer));

    //=================
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

void UdpLan::on_sendCommandToDeviceSlot(QByteArray data)
{

    this->sendDataToDevice(data,true);
}

void UdpLan::checkingConnectionAlive()
{
    int diff_time = -1;
    diff_time = m_lastReceivePacketTime.msecsTo(QTime::currentTime());

    int p{};
    if(qAbs(diff_time) > m_TimeCheckingInterval)
    {
        closePort();

        openPort();
    }
    else
    {
//        sleep(2);
        emit(connectionStatusSignal(true));
    }
}

QString UdpLan::getDeviceName() const
{
    return m_deviceName;
}

void UdpLan::setDeviceName(const QString &value)
{
    m_deviceName = value;
}

void UdpLan::setTimeCheckingInterval(int value)
{
    m_TimeCheckingInterval = value;
}
