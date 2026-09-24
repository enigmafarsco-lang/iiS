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
    explicit RFF(QWidget *parent = nullptr);
    void handle_info(QString);

    ~RFF();


private:
    Ui::RFF *ui;

    bool m_powerUnitDbm = false;
    QTimer * sendStartTimer;

private:

    void onPowTx(QString);

    QString dbmToWatt(QString val);
    QString wattToDbm(QString val);
    int lnaStatus{};
    QStringList sendingToJoshan;
    bool isDeviceConnected{false};

private slots:
    void on_BtnLnaOff_clicked();
    void on_BtnLnaOn_clicked();
    void on_RdbWatt_clicked();
    void on_RdbDbm_clicked();
    void sendStartSlot();


public slots:
    void on_DeviceResponseSlot(QByteArray response);
    void joshanFuncDataSlot();
    void joshanStatusDataSlot();
    void isDeviceConnectedSlot(bool);
    void joshanControlDataSlot(QJsonObject);


signals:
    void sendCommandToDeviceSignal(QByteArray);
    void sendCommandToObjectSignal(QString);
    void sendStartSignal(QByteArray);
//    void sendRffDataToJoshanSignal(QString);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);

    

};

#endif // RFF_H
