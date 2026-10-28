/**
 * @file tcplan.h
 * @brief Multi-threaded TCP Client Worker for Peripheral Hardware Communication.
 *
 * BTcpLan manages asynchronous, persistent TCP socket connections to peripheral
 * radar modules (RFF, Tuner, Calibration unit, Servo, Cooling unit, Joshan).
 * It runs in its own dedicated QThread, handling automatic reconnects, keepalive
 * watchdog checks, packet serialization, and mutex-protected buffer extraction.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef TCPLAN_H
#define TCPLAN_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QTime>
#include <QQueue>
#include <QMutex>

#include "appconfiguration.h"
#include "constants/project_constans.h"

/**
 * @class BTcpLan
 * @brief Threaded TCP socket client for high-reliability peripheral device control.
 */
class BTcpLan : public QThread
{
    enum LoggingFormat {STRING,HEX};

    Q_OBJECT
public:
    QMutex  addNewReadDataMutex;

    /**
     * @brief Constructs a new BTcpLan worker instance.
     * @param cfg Pointer to global application network configuration.
     * @param _deviceName Target peripheral device identifier name.
     */
    BTcpLan( AppConfiguration *cfg, QString _deviceName = "TEST");

    /**
     * @brief Destructor terminating thread execution and cleaning up sockets.
     */
    ~BTcpLan();

    /**
     * @brief Thread execution entry point running the dedicated QEventLoop.
     */
    void run() Q_DECL_OVERRIDE;

    /**
     * @brief Transmits binary byte array payload to the connected hardware device.
     * @param data Binary payload to transmit over TCP socket.
     */
    void sendDataToDevice(QByteArray data);

    /**
     * @brief Transmits ASCII/Latin-1 string payload to the connected hardware device.
     * @param data String command to transmit.
     */
    void sendDataToDevice(QString data);

    /**
     * @brief Retrieves the assigned device name.
     * @return String device name.
     */
    QString getDeviceName() const;

    /**
     * @brief Assigns a new device name identifier.
     * @param value Device name string.
     */
    void setDeviceName(const QString &value);

    /**
     * @brief Configures keepalive heartbeat checking interval.
     * @param value Watchdog interval in milliseconds (-1 to disable).
     */
    void setTimerCheckingInterval(int value=2000);

signals:
    void jsonPacket(QByteArray);
    void connectionStatusSignal(bool);
//    void DeviceResponseSignal(QString);
    void DeviceResponseSignal(QByteArray);
    void savePacketToFileSignal(QByteArray);

private slots:
    void newConnection();
    void openPort();
    void closePort();
    void handelReadyread();
    void checkingConnectionAlive();

public slots:
    void on_startStopTimerCheckingInterval(bool);

public slots:
    void on_sendCommandToDeviceSlot(QByteArray);

private:
    QTcpSocket *m_tcpClient;
    QTimer *m_connectionTimer;
    AppConfiguration *m_config;
    QTimer *m_checkingConnectionTimer;
    QTime m_lastReceivePacketTime;
    int m_TimeCheckingInterval;
    QString m_deviceName;
    LoggingFormat m_loggingFormat;
};

#endif // TCPLAN_H
