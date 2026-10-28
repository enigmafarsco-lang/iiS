/**
 * @file tcplan.cpp
 * @brief Implementation of the Multi-threaded TCP Client Worker for Peripheral Hardware.
 */

#include "tcplan.h"

/**
 * @brief Constructs a new BTcpLan worker instance and starts its event loop.
 * @param cfg Pointer to application configuration repository.
 * @param _deviceName Target peripheral device identifier name.
 */
BTcpLan::BTcpLan( AppConfiguration *cfg, QString _deviceName):m_config(cfg)
{
    m_tcpClient = nullptr;
    m_connectionTimer = nullptr;
    m_checkingConnectionTimer = nullptr;
    m_lastReceivePacketTime = QTime::currentTime();
    m_TimeCheckingInterval = 2000;

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

/**
 * @brief Destructor terminating thread execution and releasing sockets.
 */
BTcpLan::~BTcpLan()
{
    if(isRunning())
    {
        QThread::exit(0);
        QThread::wait();
    }
}

/**
 * @brief Worker thread entry point executing the Qt event loop.
 */
void BTcpLan::run()
{
    exec();
    m_tcpClient->deleteLater();
}

/**
 * @brief Transmits a binary byte array payload over the active TCP socket.
 * @param data Byte array payload to write to the socket.
 */
void BTcpLan::sendDataToDevice(QByteArray data)
{
    if(m_tcpClient->isOpen())
    {
        if(m_deviceName == COLIBR_DEVICE_NAME)
        {
             qInfo() << data;
        }
        qint64 bytesWritten = m_tcpClient->write(data);
        Q_UNUSED(bytesWritten);
    }
}

/**
 * @brief Transmits an ASCII/Latin-1 string payload over the active TCP socket.
 * @param data String command to write to the socket.
 */
void BTcpLan::sendDataToDevice(QString data)
{
    QByteArray buffer = data.toLatin1();
    if(m_tcpClient->isOpen()) {
        qint64 bytesWritten = m_tcpClient->write(buffer);
        Q_UNUSED(bytesWritten);
    }
}

/**
 * @brief Handles successful TCP connection establishment and starts the watchdog timer.
 */
void BTcpLan::newConnection()
{
    disconnect(m_connectionTimer, &QTimer::timeout, this, &BTcpLan::openPort);
    m_connectionTimer->stop();

    connect(m_tcpClient, &QTcpSocket::readyRead, this, &BTcpLan::handelReadyread);
    emit(connectionStatusSignal(true));

    connect(m_checkingConnectionTimer, &QTimer::timeout, this, &BTcpLan::checkingConnectionAlive);
    m_checkingConnectionTimer->start(m_TimeCheckingInterval);
}

/**
 * @brief Resolves target device IP and Port from configuration and attempts connection.
 */
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

    m_tcpClient->connectToHost(ipAddress,port);
}

/**
 * @brief Handles socket disconnection, cleans up resources, and initiates retry timer.
 */
void BTcpLan::closePort()
{
    connect(m_connectionTimer, &QTimer::timeout, this, &BTcpLan::openPort);
    m_connectionTimer->start(2000);
    m_checkingConnectionTimer->stop();
    m_tcpClient->deleteLater();
    m_tcpClient = new QTcpSocket(0);
    m_tcpClient->moveToThread(this);
    connect(m_tcpClient, &QTcpSocket::connected, this, &BTcpLan::newConnection);
    connect(m_tcpClient, &QTcpSocket::disconnected, this,  &BTcpLan::closePort);

    emit(connectionStatusSignal(false));
}

/**
 * @brief Ingests ready-to-read TCP bytes from socket and emits DeviceResponseSignal.
 */
void BTcpLan::handelReadyread()
{
    m_lastReceivePacketTime = QTime::currentTime();

    QByteArray buffer;
    buffer = m_tcpClient->readAll();

    if(m_deviceName == COLIBR_DEVICE_NAME)
    {
        qInfo() << buffer;
    }
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
 * @brief Watchdog timer verifying socket liveness.
 */
void BTcpLan::checkingConnectionAlive()
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
 * @brief Enables or disables connection watchdog timer.
 * @param val True to enable.
 */
void BTcpLan::on_startStopTimerCheckingInterval(bool val)
{
    if(val)
    {
        m_checkingConnectionTimer->start(m_TimeCheckingInterval);
    }
    else
    {
        m_checkingConnectionTimer->stop();
    }
}

/**
 * @brief Slot receiving commands to transmit to the peripheral device.
 * @param data Byte array payload.
 */
void BTcpLan::on_sendCommandToDeviceSlot(QByteArray data)
{
    this->sendDataToDevice(data);
}

/**
 * @brief Retrieves the assigned device name.
 * @return String device name.
 */
QString BTcpLan::getDeviceName() const
{
    return m_deviceName;
}

/**
 * @brief Assigns a new device name identifier.
 * @param value Device name string.
 */
void BTcpLan::setDeviceName(const QString &value)
{
    m_deviceName = value;
}

/**
 * @brief Configures keepalive heartbeat checking interval.
 * @param value Watchdog interval in milliseconds (-1 to disable).
 */
void BTcpLan::setTimerCheckingInterval(int value)
{
    m_TimeCheckingInterval = value;
}
