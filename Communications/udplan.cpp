/**
 * @file udplan.cpp
 * @brief Implementation of the Multi-threaded UDP Datagram Communication Worker.
 */

#include "udplan.h"

/**
 * @brief Constructs a new UdpLan worker instance and initializes socket and timer.
 * @param cfg Pointer to application configuration repository.
 * @param _deviceName Target hardware device name.
 */
UdpLan::UdpLan( AppConfiguration *cfg, QString _deviceName):m_config(cfg)
{
    m_udpClient= nullptr;
    m_checkingConnectionTimer= nullptr;
    m_lastReceivePacketTime = QTime::currentTime();
    m_TimeCheckingInterval = 5000;
    m_loggingFormat = LoggingFormat::STRING;//default logging Format;

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

/**
 * @brief Destructor terminating worker thread execution and cleaning up sockets.
 */
UdpLan::~UdpLan()
{
    if(isRunning())
    {
        QThread::quit();
        QThread::wait();
    }
}

/**
 * @brief Thread execution entry point.
 */
void UdpLan::run()
{
    exec();
    m_udpClient->deleteLater();
}

/**
 * @brief Sends binary datagram to target device.
 * @param data Binary payload byte array.
 * @param loggingFlag Enable/disable logging to console.
 */
void UdpLan::sendDataToDevice(QByteArray data, bool loggingFlag)
{    
    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
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
    Q_UNUSED(bytesWritten);

    if(loggingFlag)
    {
        QString cmd = "";
        if(m_loggingFormat == LoggingFormat::STRING)
        {
            cmd = "Send To " + m_deviceName +":\t" + QString(data).simplified();
        }
        else if(m_loggingFormat == LoggingFormat::HEX)
        {
            cmd = "Send To " + m_deviceName +":\t" + data.toHex(',').simplified();
        }

        QByteArray buffer = cmd.toLatin1();
        emit(savePacketToFileSignal(buffer));
    }
}

/**
 * @brief Sends string datagram payload to target device.
 * @param data ASCII payload string.
 */
void UdpLan::sendDataToDevice(QString data)
{
    Q_UNUSED(data);
}

/**
 * @brief Binds local UDP socket to target port.
 */
void UdpLan::openPort()
{
    if(m_udpClient==nullptr)
    {
        return;
    }

    quint16 port=0;
    bool connected = false;
    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
    {
        port = m_config->m_setting->solidPort;
    }

    connected = m_udpClient->bind(port);
    if(connected)
    {
        this->newConnection();
    }
}

/**
 * @brief Sets up readyRead binding and transmits initialization datagram.
 */
void UdpLan::newConnection()
{
    connect(m_udpClient, &QUdpSocket::readyRead, this, &UdpLan::readDatagram);

    if(m_deviceName.compare(SOLID_DEVICE_NAME, Qt::CaseInsensitive)==0)
    {
        sendDataToDevice(QString("@JamSolidStart$").toLatin1(), false);
    }
}

/**
 * @brief Closes UDP socket and resets connection state.
 */
void UdpLan::closePort()
{
    m_udpClient->disconnectFromHost();
    m_udpClient->deleteLater();

    m_udpClient = new QUdpSocket(0);
    m_udpClient->moveToThread(this);

    emit(connectionStatusSignal(false));
}

/**
 * @brief Ingests pending datagrams from socket, strips carriage returns, and emits signal.
 */
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

    QString response = "";
    if(m_loggingFormat == LoggingFormat::STRING)
    {
        response = "Recieve From " + m_deviceName +":\t" + QString(buffer).simplified();
    }
    else if(m_loggingFormat == LoggingFormat::HEX)
    {
        response = "Recieve From " + m_deviceName +":\t" + buffer.toHex(',').simplified();
    }

    QByteArray b = response.toLatin1();
    emit(savePacketToFileSignal(b));
}

/**
 * @brief Slot receiving datagrams from external UI modules to forward to device.
 * @param data Command byte array.
 */
void UdpLan::on_sendCommandToDeviceSlot(QByteArray data)
{
    this->sendDataToDevice(data,true);
}

/**
 * @brief Watchdog timer slot validating UDP connection liveness.
 */
void UdpLan::checkingConnectionAlive()
{
    int diff_time = -1;
    diff_time = m_lastReceivePacketTime.msecsTo(QTime::currentTime());

    if(qAbs(diff_time) > m_TimeCheckingInterval)
    {
        closePort();
        openPort();
    }
    else
    {
        emit(connectionStatusSignal(true));
    }
}

/**
 * @brief Retrieves target device name string.
 * @return Device identifier.
 */
QString UdpLan::getDeviceName() const
{
    return m_deviceName;
}

/**
 * @brief Sets target device name string.
 * @param value Device identifier.
 */
void UdpLan::setDeviceName(const QString &value)
{
    m_deviceName = value;
}

/**
 * @brief Sets watchdog timeout threshold in milliseconds.
 * @param value Timeout in ms.
 */
void UdpLan::setTimeCheckingInterval(int value)
{
    m_TimeCheckingInterval = value;
}
