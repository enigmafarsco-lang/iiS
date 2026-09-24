#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QRandomGenerator>
#include <QStringLiteral>

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
//#include "components/adrv/ADRV.h"
#include "utils.h"
#include <cstdlib>
#include <cstdint>
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void receiveDataFromSubSystemSlot(QString);

private:
    void connections();
    void init();
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
