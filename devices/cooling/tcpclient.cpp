// ============================================================================
// File: tcpclient.cpp
// Subsystem: eLynxSDR cooling
// Description: Subsystem implementation and interface guidance for tcpclient.cpp.
// ============================================================================

#include "tcpclient.h"
#include <QDebug>
// Constructor: Initializes TCPClient subsystem components, state, and UI bindings.
TCPClient::TCPClient(QObject *parent) : QObject(parent)
{
//    connect(&socket_tcp,&QTcpSocket::connected,this,&TCPClient::connected);

//    connect(&socket_tcp,&QTcpSocket::disconnected,this,&TCPClient::disconnected);

//    connect(&socket_tcp,&QTcpSocket::stateChanged,this,&TCPClient::stateChanged);

//    connect(&socket_tcp,&QTcpSocket::readyRead,this,&TCPClient::readyRead);

//    connect(&socket_tcp,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&TCPClient::error);


}

// Executes connet to host routine.
void TCPClient::connetToHost(QString host,quint16 port)
{
//    /*
//     * For UDP only one socket cab be opened at the same time so it should be closed first
//    */
//    socket_tcp.abort();
//    if(socket_tcp.isOpen()) disconnect();

//   // qInfo() <<"Connecting to "<<host<<" on port "<<port;

//    socket_tcp.connectToHost(host,port);
}

/*
 * Fire this  when connected socket want to be closed
*/
void TCPClient::disconnect()
{
//    socket_tcp.close();
   // qWarning()<< "closed socket";


}

// Transmits write command/packet over interface.
void TCPClient::write(const QByteArray &data)
{
//    socket_tcp.write(data);
}

/*
 * Fire this  when client connected
*/
void TCPClient::connected()
{
//    qInfo()<< "Connected";

//    qInfo()<< "Sending";
//    socket_tcp.write("Hello\r\n");

}
/*
 * Fire this  when client disconnected
*/
void TCPClient::disconnected()
{
//    qInfo()<< "Disconnected";
}
/*
 * Fire this when connecting to socket has an error
*/
void TCPClient::error(QAbstractSocket::SocketError socketError)
{
//    qCritical()<< "Error:"<<socketError<<" "<<socket_tcp.errorString();
}
/*
 * Fire this  when the state of connection is changed
*/
void TCPClient::stateChanged(QAbstractSocket::SocketState socketState)
{
//    QMetaEnum metaEnum=QMetaEnum::fromType<QAbstractSocket::SocketState>();

//    qInfo()<<"State: "<<metaEnum.valueToKey(socketState);
//    emit StateChanged(socketState);
}
/*
*Fire this signal  once every time new data is available for reading from the device
*/
void TCPClient::readyRead()
{
//    QByteArray datagram;
//    QByteArray datagram2;
//    //    qInfo()<<"Data from: "<<sender()<<" bytes"<<socket_tcp.bytesAvailable();
//    datagram=socket_tcp.readAll();
////        qInfo()<<"Data: "<<datagram2;
//    newPacketReceived(datagram.toHex());
//    newPacketReceived(datagram2.toHex());
}



