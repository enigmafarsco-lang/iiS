/**
 * @file udplan.h
 * @brief Threaded UDP Datagram Communication Worker for Fast Telemetry.
 *
 * UdpLan manages low-latency UDP datagram transmission and reception for high-speed
 * hardware peripherals (such as Solid-State Amplifiers and radar radar pulse detectors).
 * It runs on a dedicated QThread with heartbeat watchdog validation.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef UDPLAN_H
#define UDPLAN_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include <QTime>

#include "appconfiguration.h"
#include "constants/project_constans.h"

/**
 * @class UdpLan
 * @brief Multi-threaded UDP socket communication engine for telemetry exchange.
 */
class UdpLan : public QThread
{
    enum LoggingFormat { STRING, HEX };

    Q_OBJECT

public:
    /**
     * @brief Constructs a new UdpLan worker instance.
     * @param cfg Pointer to application configuration repository.
     * @param _deviceName Target hardware device name.
     */
    explicit UdpLan(AppConfiguration *cfg, QString _deviceName = "TEST");

    /**
     * @brief Destructor terminating thread execution and closing UDP sockets.
     */
    ~UdpLan();

    /**
     * @brief Thread execution entry point.
     */
    void run() override;

    /**
     * @brief Sends binary datagram to target device.
     * @param data Binary payload byte array.
     * @param loggingFlag Enable/disable logging to console.
     */
    void sendDataToDevice(QByteArray data, bool loggingFlag = true);

    /**
     * @brief Sends string datagram to target device.
     * @param data ASCII payload string.
     */
    void sendDataToDevice(QString data);

    /**
     * @brief Retrieves target device name.
     * @return String device identifier.
     */
    QString getDeviceName() const;

    /**
     * @brief Assigns new target device name.
     * @param value Device identifier string.
     */
    void setDeviceName(const QString &value);

    /**
     * @brief Sets keepalive watchdog interval.
     * @param value Heartbeat interval in milliseconds.
     */
    void setTimeCheckingInterval(int value = 2000);

private slots:
    /**
     * @brief Binds local UDP socket port.
     */
    void openPort();

    /**
     * @brief Handles initialization upon connection.
     */
    void newConnection();

    /**
     * @brief Closes UDP socket.
     */
    void closePort();

    /**
     * @brief Ingests ready UDP datagrams.
     */
    void readDatagram();

    /**
     * @brief Checks UDP packet reception timeout.
     */
    void checkingConnectionAlive();

public slots:
    /**
     * @brief Slot receiving datagrams to send to hardware.
     * @param cmd Binary command payload.
     */
    void on_sendCommandToDeviceSlot(QByteArray cmd);

signals:
    /**
     * @brief Emitted when datagram connection state changes.
     * @param isConnected True if packets are actively being received.
     */
    void connectionStatusSignal(bool isConnected);

    /**
     * @brief Emitted when a new UDP datagram is received from the device.
     * @param response Datagram byte array.
     */
    void DeviceResponseSignal(QByteArray response);

    /**
     * @brief Emitted to log datagram to disk.
     * @param rawPacket Binary packet payload.
     */
    void savePacketToFileSignal(QByteArray rawPacket);

private:
    QUdpSocket       * m_udpClient;               ///< UDP socket handle.
    AppConfiguration * m_config;                  ///< Network configuration settings.
    QString            m_deviceName;              ///< Target device identifier.
    LoggingFormat      m_loggingFormat;           ///< Logging format (STRING/HEX).
    QHostAddress       m_hostIp;                  ///< Target host IPv4 address.
    quint16            m_hostPort;                ///< Target host UDP port.
    QTimer           * m_checkingConnectionTimer; ///< Watchdog timer.
    QTime              m_lastReceivePacketTime;   ///< Last packet reception timestamp.
    int                m_TimeCheckingInterval;    ///< Timeout interval in milliseconds.
};

#endif // UDPLAN_H
