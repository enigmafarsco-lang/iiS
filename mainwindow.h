/**
 * @file mainwindow.h
 * @brief Master Application Window & Subsystem Orchestrator for eLynxSDR.
 *
 * MainWindow acts as the central coordinator for:
 * 1. Network communication channels (TCP / UDP clients for peripheral hardware).
 * 2. Transceiver receiver window lifecycle (ReceiverMain, Plotting, Spectrum).
 * 3. Multi-waveform exciter integration and DAC DMA signal routing.
 * 4. Peripheral subsystems (Cooling, Servo, Tuner, RFF, Solid-state switch, Joshan).
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QRandomGenerator>
#include <QStringLiteral>
#include <QThread>
#include <cstdlib>
#include <cstdint>

#include "devices/targetDetector/targetdetector.h"
#include "devices/sattar/components/Store.h"
#include "Communications/appconfiguration.h"
#include "devices/sattar/pages/menupage.h"
#include "devices/sattar/components/sattarutils.h"
#include "constants/project_constans.h"
#include "devices/solid/soliddialog.h"
#include "Communications/tcplan.h"
#include "Communications/udplan.h"
#include "receiver/receivermain.h"
#include "devices/joshan/joshan.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Top-level QMainWindow orchestrating hardware links, telemetry, and GUI tabs.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the MainWindow, sets up sub-widgets, network links, and UI styling.
     * @param parent Optional parent QWidget pointer.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor releasing allocated hardware interfaces and communication threads.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot receiving generic string telemetry packets from hardware subsystems.
     * @param data Raw response string payload.
     */
    void receiveDataFromSubSystemSlot(QString data);

private:
    /**
     * @brief Establishes Qt Signal/Slot interconnections across all peripheral devices.
     */
    void connections();

    /**
     * @brief Instantiates peripheral TCP/UDP clients, configures timers, and embeds Exciter tab.
     */
    void init();

    /**
     * @brief Instantiates user actions, shortcuts, and menu triggers.
     */
    void createActions();

private:
    // Peripheral Hardware Communication Handles
    BTcpLan * mRffLan;           ///< TCP client for RF Front-End (RFF) controller.
    BTcpLan * mCalib;            ///< TCP client for RF Calibration unit.
    BTcpLan * mTunerLan;         ///< TCP client for RF Tuner subsystem.
    BTcpLan * mCoolLan;          ///< TCP client for Liquid/Air Cooling controller.
    BTcpLan * mServoLan;         ///< TCP client for Antenna Servo positioning drive.
    BTcpLan * mJoshan;           ///< TCP client for Electronic Protection (Joshan) module.
    UdpLan  * mSolidLan;         ///< UDP client for Solid-State Power Amplifier / Switches.
    BTcpLan * mTCP_OuterNode;    ///< TCP interface for external command-and-control node.

    bool receiverIsConnected{true}; ///< State tracking if receiver hardware link is active.

    Joshan * joshan;             ///< Electronic counter-countermeasures logic controller.
    bool isAdrvConnect{false};   ///< Flag indicating if ADRV9009 transceiver is connected.

    ReceiverMain * receiverWindow; ///< Main receiver widget hosting spectrum analyzer and embedded Exciter.

    AppConfiguration * mAppConfig; ///< Global application network and peripheral configuration repository.
    SolidDialog      * solidDialog;///< Dialog interface for Solid-state amplifier monitoring.
    Ui::MainWindow   * ui;          ///< Qt Designer generated UI elements pointer.

signals:
    /**
     * @brief Emitted to switch cooling system operational profiles.
     * @param mode Target cooling mode identifier string.
     */
    void selectCoolerSignal(QString mode);

    /**
     * @brief Emitted to propagate hardware IP address to Exciter subsystem.
     * @param ip Target IPv4 address string.
     */
    void sendIpAddressToExciterSignal(const QString ip);

    /**
     * @brief Emitted to broadcast connection state changes of Joshan subsystem.
     * @param connected True if connected, false otherwise.
     */
    void isJoshanConnectSignal(bool connected);

    /**
     * @brief Emitted to notify Exciter of master receiver connection state.
     * @param status True if connected.
     */
    void exciterConnectionStatus(bool status);

    /**
     * @brief Emitted to log or display DAC status messages.
     * @param title Message title or channel name.
     * @param msg Message body content.
     */
    void dacMsgSignal(QString title, QString msg);
};

#endif // MAINWINDOW_H
