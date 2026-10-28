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
    enum LoggingFormat { STRING, HEX };

    Q_OBJECT

public:
    QMutex addNewReadDataMutex; ///< Protects raw inbound packet buffers during concurrent access.

    /**
     * @brief Constructs a new BTcpLan worker instance.
     * @param cfg Pointer to global application network configuration.
     * @param _deviceName Target peripheral device identifier name.
     */
    BTcpLan(AppConfiguration *cfg, QString _deviceName = "TEST");

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
    void setTimerCheckingInterval(int value = 2000);

signals:
    /**
     * @brief Emitted when a parsed JSON telemetry packet is received.
     * @param packet JSON raw byte array.
     */
    void jsonPacket(QByteArray packet);

    /**
     * @brief Emitted when socket connection state transitions (connected/disconnected).
     * @param isConnected True if socket is connected and authenticated.
     */
    void connectionStatusSignal(bool isConnected);

    /**
     * @brief Emitted when raw device response bytes arrive.
     * @param response Raw byte array response from peripheral hardware.
     */
    void DeviceResponseSignal(QByteArray response);

    /**
     * @brief Emitted to request packet logging to disk.
     * @param rawPacket Packet content bytes.
     */
    void savePacketToFileSignal(QByteArray rawPacket);

private slots:
    /**
     * @brief Handles successful TCP socket connection event.
     */
    void newConnection();

    /**
     * @brief Attempts to connect to target device IP and Port.
     */
    void openPort();

    /**
     * @brief Handles socket disconnection, triggers retry timer.
     */
    void closePort();

    /**
     * @brief Ingests and processes incoming data from socket readyRead signal.
     */
    void handelReadyread();

    /**
     * @brief Watchdog timer slot validating connection liveness.
     */
    void checkingConnectionAlive();

public slots:
    /**
     * @brief Enables or disables connection watchdog timer.
     * @param enable True to enable, false to stop.
     */
    void on_startStopTimerCheckingInterval(bool enable);

    /**
     * @brief Slot receiving commands to transmit to the peripheral device.
     * @param cmd Byte array command payload.
     */
    void on_sendCommandToDeviceSlot(QByteArray cmd);

private:
    QTcpSocket       * m_tcpClient;               ///< Active TCP socket handle.
    QTimer           * m_connectionTimer;         ///< Periodic connection retry timer.
    AppConfiguration * m_config;                  ///< Network configuration settings.
    QTimer           * m_checkingConnectionTimer; ///< Watchdog heartbeat timer.
    QTime              m_lastReceivePacketTime;   ///< Timestamp of last received packet.
    int                m_TimeCheckingInterval;    ///< Watchdog timeout threshold in ms.
    QString            m_deviceName;              ///< Target device identifier name.
    LoggingFormat      m_loggingFormat;           ///< Packet logging format (STRING or HEX).
};

#endif // TCPLAN_H
