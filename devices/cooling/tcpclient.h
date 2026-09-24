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
    explicit TCPClient(QObject *parent = nullptr);

signals:
    void newPacketReceived(QByteArray packet);
    void StateChanged(QAbstractSocket::SocketState socketState);
public slots:
    void connetToHost(QString host,quint16 port);
    void disconnect();
    void write(const QByteArray& data);

private slots:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void readyRead();

public :
    QString message_tcp;
    QString packet_tcp;
    void error(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket socket_tcp;
};

#endif // TCPCLIENT_H
