// ============================================================================
// File: joshan.h
// Subsystem: eLynxSDR joshan
// Description: Subsystem implementation and interface guidance for joshan.h.
// ============================================================================

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
    // Constructor: Initializes Joshan subsystem components, default parameters, and state.
    explicit Joshan(QWidget     * parent      = nullptr);




    // Retrieves current oshan stop setter state.
    void isJoshanStopSetter(bool);

    // Destructor: Cleans up Joshan resources, timers, and allocated memory.
    ~Joshan();

public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray);
    // Retrieves current func data joshan slot state.
    void getFuncDataJoshanSlot(QJsonObject);
    // Retrieves current status data joshan slot state.
    void getStatusDataJoshanSlot(QJsonObject);
    // Executes joshan is connected routine.
    void joshanIsConnected(bool);

private slots:

    //    void gettingData(QString,QString);

private:
    AppConfiguration * mAppConfig;
    BTcpLan * mJoshan;

    QTimer * joshanFunctionalTimer;
    QTimer * joshanStatusTimer;


    // Initializes init for Joshan.
    void init();
    // Executes connections routine.
    void connections();
    // Executes handle info routine.
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
    // Executes servo data routine.
    void servoData   (QString);
    // Executes solid data routine.
    void solidData   (QString);
    // Executes exiter data routine.
    void exiterData  (QString);
    // Executes cooling data routine.
    void coolingData (QString);
    // Executes rff data routine.
    void rffData     (QString);
    // Transmits send func data to joshan signal command/packet over communication interface.
    void sendFuncDataToJoshanSignal();
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal();
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);

    // Executes joshan data to rff signal routine.
    void joshanDataToRffSignal           (QJsonObject);
    // Executes joshan data to solid signal routine.
    void joshanDataToSolidSignal         (QJsonObject);
    // Executes joshan data to servo signal routine.
    void joshanDataToServoSignal         (QJsonObject);
    // Executes joshan data to cooling signal routine.
    void joshanDataToCoolingSignal       (QJsonObject);
    // Executes joshan data to exciter set signal routine.
    void joshanDataToExciterSetSignal    (QJsonObject);
    // Executes joshan data to exciter enable signal routine.
    void joshanDataToExciterEnableSignal (QJsonObject);

//    void connectionStatusSignal(bool);
    void structFuncDataSignal(joshanFuncStruct*);
    // Executes struct status data signal routine.
    void structStatusDataSignal(joshanStatusStruct*);


};

#endif // JOSHAN_H
