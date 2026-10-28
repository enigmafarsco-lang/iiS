// ============================================================================
// File: rff.h
// Subsystem: eLynxSDR rff
// Description: Subsystem implementation and interface guidance for rff.h.
// ============================================================================

#ifndef RFF_H
#define RFF_H

#include <QWidget>
#include <QTimer>
#include <QThread>
#include <iostream>
#include "../../usertype.h"
#include "constants/project_constans.h"


namespace Ui {
class RFF;
}

class RFF : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes RFF subsystem components, default parameters, and state.
    explicit RFF(QWidget *parent = nullptr);
    // Executes handle info routine.
    void handle_info(QString);

    // Destructor: Cleans up RFF resources, timers, and allocated memory.
    ~RFF();


private:
    Ui::RFF *ui;

    bool m_powerUnitDbm = false;
    QTimer * sendStartTimer;

private:

    // Executes on pow tx routine.
    void onPowTx(QString);

    // Executes dbm to watt routine.
    QString dbmToWatt(QString val);
    // Executes watt to dbm routine.
    QString wattToDbm(QString val);
    int lnaStatus{};
    QStringList sendingToJoshan;
    bool isDeviceConnected{false};

private slots:
    // Event/Signal handler for on  btn lna off clicked.
    void on_BtnLnaOff_clicked();
    // Event/Signal handler for on  btn lna on clicked.
    void on_BtnLnaOn_clicked();
    // Event/Signal handler for on  rdb watt clicked.
    void on_RdbWatt_clicked();
    // Event/Signal handler for on  rdb dbm clicked.
    void on_RdbDbm_clicked();
    // Transmits send start slot command/packet over communication interface.
    void sendStartSlot();


public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray response);
    // Executes joshan func data slot routine.
    void joshanFuncDataSlot();
    // Executes joshan status data slot routine.
    void joshanStatusDataSlot();
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);
    // Executes joshan control data slot routine.
    void joshanControlDataSlot(QJsonObject);


signals:
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
    // Transmits send command to object signal command/packet over communication interface.
    void sendCommandToObjectSignal(QString);
    // Transmits send start signal command/packet over communication interface.
    void sendStartSignal(QByteArray);
//    void sendRffDataToJoshanSignal(QString);
    void sendFuncDataToJoshanSignal(QJsonObject);
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal(QJsonObject);

    

};

#endif // RFF_H
