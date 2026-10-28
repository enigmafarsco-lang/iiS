// ============================================================================
// File: tcpclient.h
// Subsystem: eLynxSDR cooling
// Description: Subsystem implementation and interface guidance for tcpclient.h.
// ============================================================================

#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMetaEnum>
#include <QNetworkProxy>

class TCPClient :public QObject
{
    Q_OBJECT
public:
    // Constructor: Initializes TCPClient subsystem components, default parameters, and state.
    explicit TCPClient(QObject *parent = nullptr);

signals:
    // Executes new packet received routine.
    void newPacketReceived(QByteArray packet);
    // Executes state changed routine.
    void StateChanged(QAbstractSocket::SocketState socketState);
public slots:
    // Executes connet to host routine.
    void connetToHost(QString host,quint16 port);
    // Executes disconnect routine.
    void disconnect();
    // Transmits write command/packet over communication interface.
    void write(const QByteArray& data);

private slots:
    // Executes connected routine.
    void connected();
    // Executes disconnected routine.
    void disconnected();
    // Executes state changed routine.
    void stateChanged(QAbstractSocket::SocketState socketState);
    // Reads and parses incoming ready read data.
    void readyRead();

public :
    QString message_tcp;
    QString packet_tcp;
    // Executes error routine.
    void error(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket socket_tcp;
};

#endif // TCPCLIENT_H
