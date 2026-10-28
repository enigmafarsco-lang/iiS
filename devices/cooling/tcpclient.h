/**
 * @file tcpclient.h
 * @brief Subsystem module for cooling in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMetaEnum>
#include <QNetworkProxy>

/**
 * @class TCPClient
 * @brief High-level controller class for TCPClient.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TCPClient :public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new TCPClient instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    explicit TCPClient(QObject *parent = nullptr);

signals:
    /**
     * @brief Executes the new packet received procedure for TCPClient.
     * @param packet Input parameter specifying packet (QByteArray).
     */
    void newPacketReceived(QByteArray packet);
    /**
     * @brief Executes the state changed procedure for TCPClient.
     * @param socketState Input parameter specifying socketState (QAbstractSocket::SocketState).
     */
    void StateChanged(QAbstractSocket::SocketState socketState);
public slots:
    /**
     * @brief Executes the connet to host procedure for TCPClient.
     * @param host Input parameter specifying host (QString).
     * @param port Input parameter specifying port (quint16).
     */
    void connetToHost(QString host,quint16 port);
    /**
     * @brief Executes the disconnect procedure for TCPClient.
     */
    void disconnect();
    /**
     * @brief Transmits write command/packet over the communication bus.
     * @param data Input parameter specifying data (const QByteArray&).
     */
    void write(const QByteArray& data);

private slots:
    /**
     * @brief Executes the connected procedure for TCPClient.
     */
    void connected();
    /**
     * @brief Executes the disconnected procedure for TCPClient.
     */
    void disconnected();
    /**
     * @brief Executes the state changed procedure for TCPClient.
     * @param socketState Input parameter specifying socketState (QAbstractSocket::SocketState).
     */
    void stateChanged(QAbstractSocket::SocketState socketState);
    /**
     * @brief Reads, parses, and decodes incoming ready read data buffer.
     */
    void readyRead();

public :
    QString message_tcp;
    QString packet_tcp;
    /**
     * @brief Executes the error procedure for TCPClient.
     * @param socketError Input parameter specifying socketError (QAbstractSocket::SocketError).
     */
    void error(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket socket_tcp;
};

#endif // TCPCLIENT_H
