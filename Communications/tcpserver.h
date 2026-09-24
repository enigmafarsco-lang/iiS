#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include <QMessageBox>

#include "appconfiguration.h"

class TcpServer : public QThread
{
    Q_OBJECT
public:
    TcpServer(AppConfiguration*);
    ~TcpServer();

signals:
    void outerNodeResponseSignal(QByteArray);
    void connectionStatusSignal(bool);

    void savePacketToFileSignal(QByteArray);
private slots:
    void newConnection();
    void handelReadyRead();
    void checkingConnectionAlive();
    void closePort();

public slots:
    void on_sendCommandToOuterNodeSlot(QByteArray);

private:
    QTcpServer *m_tcpServer;
    AppConfiguration *m_config;
    QTcpSocket *m_tcpClient= nullptr;

    // QThread interface
protected:
    virtual void run() Q_DECL_OVERRIDE;
};

#endif // TCPSERVER_H
