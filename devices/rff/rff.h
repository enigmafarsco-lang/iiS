/**
 * @file rff.h
 * @brief Subsystem module for rff in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef RFF_H
#define RFF_H

#include <QWidget>
#include <QTimer>
#include <QThread>
#include <iostream>
#include "../../usertype.h"
#include "constants/project_constans.h"


namespace Ui {
/**
 * @class RFF
 * @brief High-level controller class for RFF.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class RFF;
}

/**
 * @class RFF
 * @brief High-level controller class for RFF.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class RFF : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new RFF instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit RFF(QWidget *parent = nullptr);
    /**
     * @brief Executes the handle info procedure for RFF.
     * @param QString Input parameter specifying QString (QString).
     */
    void handle_info(QString);

    /**
     * @brief Destructor for RFF, releasing allocated hardware resources, timers, and buffers.
     */
    ~RFF();


private:
    Ui::RFF *ui;

    bool m_powerUnitDbm = false;
    QTimer * sendStartTimer;

private:

    /**
     * @brief Executes the on pow tx procedure for RFF.
     * @param QString Input parameter specifying QString (QString).
     */
    void onPowTx(QString);

    /**
     * @brief Executes the dbm to watt procedure for RFF.
     * @param val Input parameter specifying val (QString).
     * @return Output value of type QString.
     */
    QString dbmToWatt(QString val);
    /**
     * @brief Executes the watt to dbm procedure for RFF.
     * @param val Input parameter specifying val (QString).
     * @return Output value of type QString.
     */
    QString wattToDbm(QString val);
    int lnaStatus{};
    QStringList sendingToJoshan;
    bool isDeviceConnected{false};

private slots:
    /**
     * @brief Slot handler responding to on  btn lna off clicked event.
     */
    void on_BtnLnaOff_clicked();
    /**
     * @brief Slot handler responding to on  btn lna on clicked event.
     */
    void on_BtnLnaOn_clicked();
    /**
     * @brief Slot handler responding to on  rdb watt clicked event.
     */
    void on_RdbWatt_clicked();
    /**
     * @brief Slot handler responding to on  rdb dbm clicked event.
     */
    void on_RdbDbm_clicked();
    /**
     * @brief Transmits send start slot command/packet over the communication bus.
     */
    void sendStartSlot();


public slots:
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param response Input parameter specifying response (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray response);
    /**
     * @brief Executes the joshan func data slot procedure for RFF.
     */
    void joshanFuncDataSlot();
    /**
     * @brief Executes the joshan status data slot procedure for RFF.
     */
    void joshanStatusDataSlot();
    /**
     * @brief Retrieves the current evice connected slot value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isDeviceConnectedSlot(bool);
    /**
     * @brief Executes the joshan control data slot procedure for RFF.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanControlDataSlot(QJsonObject);


signals:
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
    /**
     * @brief Transmits send command to object signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendCommandToObjectSignal(QString);
    /**
     * @brief Transmits send start signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendStartSignal(QByteArray);
//    void sendRffDataToJoshanSignal(QString);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);

    

};

#endif // RFF_H
