#ifndef JOSHAN_H
#define JOSHAN_H

#include <QWidget>
#include "QJsonDocument"
#include "Communications/appconfiguration.h"
#include "constants/project_constans.h"
#include "Communications/tcplan.h"
//devices
#include "devices/joshan/datastruct.h"



namespace Ui {
class Joshan;
}

class Joshan : public QWidget
{
    Q_OBJECT

public:
    explicit Joshan(QWidget     * parent      = nullptr);




    void isJoshanStopSetter(bool);

    ~Joshan();

public slots:
    void on_DeviceResponseSlot(QByteArray);
    void getFuncDataJoshanSlot(QJsonObject);
    void getStatusDataJoshanSlot(QJsonObject);
    void joshanIsConnected(bool);

private slots:

    //    void gettingData(QString,QString);

private:
    AppConfiguration * mAppConfig;
    BTcpLan * mJoshan;

    QTimer * joshanFunctionalTimer;
    QTimer * joshanStatusTimer;


    void init();
    void connections();
    void handle_info(QString);
    int cntFunc{};
    int cntStatus{};
    bool isJoshanStop{false};
    QString lnaStatus{};
    QString receivedDeviceValue;
    QJsonObject deviceFuncData;
    QJsonObject deviceStatusData;
//    QJsonArray /*deviceDataArr*/;
//    QJsonDocument val(deviceData);

    Ui::Joshan *ui;

signals:
    void servoData   (QString);
    void solidData   (QString);
    void exiterData  (QString);
    void coolingData (QString);
    void rffData     (QString);
    void sendFuncDataToJoshanSignal();
    void sendStatusDataToJoshanSignal();
    void sendCommandToDeviceSignal(QByteArray);

    void joshanDataToRffSignal           (QJsonObject);
    void joshanDataToSolidSignal         (QJsonObject);
    void joshanDataToServoSignal         (QJsonObject);
    void joshanDataToCoolingSignal       (QJsonObject);
    void joshanDataToExciterSetSignal    (QJsonObject);
    void joshanDataToExciterEnableSignal (QJsonObject);

//    void connectionStatusSignal(bool);
    void structFuncDataSignal(joshanFuncStruct*);
    void structStatusDataSignal(joshanStatusStruct*);


};

#endif // JOSHAN_H
