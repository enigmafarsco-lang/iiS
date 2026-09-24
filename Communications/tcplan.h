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


class BTcpLan : public QThread
{
    enum LoggingFormat {STRING,HEX};

    Q_OBJECT
public:
    QMutex  addNewReadDataMutex;

    BTcpLan( AppConfiguration *cfg, QString _deviceName = "TEST");

    ~BTcpLan();

    void run() Q_DECL_OVERRIDE;
    void sendDataToDevice(QByteArray data);
    void sendDataToDevice(QString data);
    QString getDeviceName() const;
    void setDeviceName(const QString &value);
    ///
    /// \brief setCheckingTime
    /// \param value msec
    ///
    void setTimerCheckingInterval(int value=2000);

signals:
    void jsonPacket(QByteArray);
    ///
    /// \brief connectionStatusSignal
    /// device status ;)
    ///
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
