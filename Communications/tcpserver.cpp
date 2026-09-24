#include "tcpserver.h"

TcpServer::TcpServer(AppConfiguration* cfg):m_config(cfg)
{
    m_tcpServer = new QTcpServer(0);

    if((!m_tcpServer->listen(QHostAddress::AnyIPv4,m_config->m_setting->RffPort)))
    {
        QMessageBox::warning(0,tr("Fortune Server"),
                             tr("Unable to Start The Server: %1.")
                             .arg(m_tcpServer->errorString()));
        deleteLater();

        emit(connectionStatusSignal(false));
    }

    connect(m_tcpServer,&QTcpServer::newConnection,this, &TcpServer::newConnection);
    start(HighestPriority);

    m_tcpServer->moveToThread(this);
    moveToThread(this);
}

TcpServer::~TcpServer()
{
    if((!isRunning()))
    {
        QThread::quit();
        QThread::wait();
    }
}

void TcpServer::newConnection()
{
    m_tcpClient = m_tcpServer->nextPendingConnection();

    connect(m_tcpClient, &QTcpSocket::disconnected, this, &TcpServer::closePort);
    connect(m_tcpClient, &QTcpSocket::readyRead, this, &TcpServer::handelReadyRead);

    m_tcpClient->moveToThread(this);

    //=============
    emit(connectionStatusSignal(true));
}

void TcpServer::handelReadyRead()
{
    QByteArray buffer;

    buffer = m_tcpClient->readAll();

    emit(outerNodeResponseSignal(buffer));
}

void TcpServer::on_sendCommandToOuterNodeSlot(QByteArray data)
{
    if(m_tcpClient != nullptr && m_tcpClient->isOpen())
    {
        qint64 bytesWritten = m_tcpClient->write(data);
    }
}

void TcpServer::checkingConnectionAlive()
{

}

void TcpServer::closePort()
{
    m_tcpClient->deleteLater();
    m_tcpClient =nullptr;
    emit(connectionStatusSignal(false));
}

void TcpServer::run()
{
    exec();

    m_tcpServer->deleteLater();
}
