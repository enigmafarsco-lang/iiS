/**
 * @file udplan.h
 * @brief Threaded UDP Datagram Communication Worker for Fast Telemetry.
 *
 * UdpLan manages low-latency UDP datagram transmission and reception for high-speed
 * hardware peripherals (such as Solid-State Amplifiers and radar pulse detectors).
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
    enum LoggingFormat {STRING,HEX};

    Q_OBJECT
public:
    /**
     * @brief Constructs a new UdpLan worker instance.
     * @param cfg Pointer to application configuration repository.
     * @param _deviceName Target hardware device name.
     */
    UdpLan(AppConfiguration *cfg,QString _deviceName="TEST");

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
    void setTimeCheckingInterval(int value=2000);

private slots:
    void openPort();
    void newConnection();
    void closePort();
    void readDatagram();
    void checkingConnectionAlive();

public slots:
    void on_sendCommandToDeviceSlot(QByteArray);

signals:
    void connectionStatusSignal(bool);
    void DeviceResponseSignal(QByteArray);
    void savePacketToFileSignal(QByteArray);

private:
    QUdpSocket *m_udpClient;
    //QTimer *m_connectionTimer;
    AppConfiguration *m_config;
    QString m_deviceName;
    LoggingFormat m_loggingFormat;

    QHostAddress m_hostIp;
    quint16 m_hostPort;
    QTimer * m_checkingConnectionTimer;
    QTime m_lastReceivePacketTime;
    int m_TimeCheckingInterval;
};

#endif // UDPLAN_H
