/**
 * @file mainwindow.h
 * @brief Master Application Window & Subsystem Orchestrator for eLynxSDR.
 *
 * Central coordinator for:
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
#include <cstdlib>
#include <cstdint>
#include <QThread>

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
    MainWindow(QWidget *parent = nullptr);

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
    BTcpLan * mRffLan;
    BTcpLan * mCalib;
    BTcpLan * mTunerLan;
    BTcpLan * mCoolLan;
    BTcpLan * mServoLan;
    BTcpLan * mJoshan;
    UdpLan  * mSolidLan;
    BTcpLan * mTCP_OuterNode;

    bool receiverIsConnected{true};

    Joshan * joshan;
    bool isAdrvConnect{false};
    //start cheraghi
//    UdpLan  * mUDP_OuterNode;
    //end cheraghi
    ReceiverMain * receiverWindow;

    AppConfiguration * mAppConfig;
    SolidDialog      * solidDialog;
    Ui::MainWindow *ui;
// adrv * adrvObj = new adrv();

signals:
    void selectCoolerSignal(QString);
    void sendIpAddressToExciterSignal(const QString );
    void isJoshanConnectSignal(bool);
    void exciterConnectionStatus(bool);
    void dacMsgSignal(QString,QString);
//    void getLimitationValueSignal();
};

#endif // MAINWINDOW_H
