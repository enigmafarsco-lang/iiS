#ifndef UDPLAN_H
#define UDPLAN_H
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include "appconfiguration.h"
#include "constants/project_constans.h"
#include <QTime>

class UdpLan : public QThread
{
    enum LoggingFormat {STRING,HEX};

    Q_OBJECT
public:
    UdpLan(AppConfiguration *cfg,QString _deviceName="TEST");
    ~UdpLan();

    void run() override;

    void sendDataToDevice(QByteArray data, bool loggingFlag = true);
    void sendDataToDevice(QString data);

    QString getDeviceName() const;
    void setDeviceName(const QString &value);

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
