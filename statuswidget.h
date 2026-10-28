/**
 * @file statuswidget.h
 * @brief System Status Bar & Peripheral Health Monitor Widget for eLynxSDR.
 *
 * StatusWidget displays real-time connection states, operational status LEDs,
 * link health, and control toggles for all peripheral modules (RFF Box, Solid-state
 * amplifier, Servo, GPS Compass, Power Board, External C2 node, and Joshan).
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QColor>

#include "constants/project_constans.h"
#include "Communications/tcplan.h"
#include "Communications/udplan.h"
#include "Communications/tcpserver.h"

namespace Ui {
class StatusWidget;
}

/**
 * @class StatusWidget
 * @brief Visual status indicator panel monitoring health of connected SDR hardware.
 */
class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs StatusWidget.
     * @param parent Optional parent QWidget pointer.
     */
    explicit StatusWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~StatusWidget();

public slots:
    /**
     * @brief Slot receiving response telemetry from connected hardware.
     * @param data Binary payload.
     */
    void On_DeviceResponseSlot(QByteArray data);

    /**
     * @brief Updates status LED color based on peripheral connection state.
     * @param isConnected True if module is connected.
     */
    void connectionStatusSlot(bool isConnected);

signals:
    /**
     * @brief Emitted to transmit raw command byte array to hardware.
     */
    void sendCommandToDeviceSignal(QByteArray);

public:
    /**
     * @brief Parses and updates status UI fields from received telemetry packet.
     * @param info Inbound status payload.
     */
    void handle_info(QByteArray info);

signals:
    /**
     * @brief Emitted to send string commands to external objects.
     */
    void sendCommandToOtherObjectSignal(QString);

public slots:
    /**
     * @brief Receives string commands from external objects.
     */
    void receiveCommandFromOtherObjectSlot(QString);

    /**
     * @brief Updates Exciter subsystem status LED indicator.
     * @param isConnected True if Exciter is ready.
     */
    void exciterConnectionSlot(bool isConnected);

private slots:
    void on_btnExciterConnectionStatus_clicked(bool checked);
    void on_btnSolidConnectionStatus_clicked(bool checked);
    void on_btnServoConnectionStatus_clicked(bool checked);
    void on_btnGpsCompassConnectionStatus_clicked(bool checked);
    void on_btnPowerControlBoardConnectionStatus_clicked(bool checked);
    void on_btnOuterNodeConnectionStatus_clicked(bool checked);
    void on_btnRffBoxConnectionStatus_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_chbJoshanAllowed_toggled(bool checked);

private:
    QString m_commandString = "";
    QByteArray m_commandBytes;
    bool btnJoshanStatus{true};

    Ui::StatusWidget * ui;

signals:
    /**
     * @brief Emitted when Joshan protection mode is toggled.
     * @param stop True to stop Joshan operation.
     */
    void isJoshanStop(bool stop);
};

#endif // STATUSWIDGET_H
