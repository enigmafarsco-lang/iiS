#ifndef EXCITER_H
#define EXCITER_H

#include <QSet>
#include <QWidget>
#include <unistd.h>
#include <iio.h>

#include "components/adrv/ADRV.h"
#include <QMessageBox>
#include "QJsonObject"
#include <QTimer>
#include <iostream>
#include <fstream>
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"


namespace Ui {
class exciter;
}

class Exciter : public QWidget
{
    Q_OBJECT

public:
    explicit Exciter(QWidget *parent = nullptr);
    ~Exciter();
    QDoubleSpinBox * spnCWPower;
    QDoubleSpinBox * spnSweepPower;
    QDoubleSpinBox * spnimpulsePower;
    QDoubleSpinBox * spnWidePower;
    QDoubleSpinBox * spnSpotPower;
    QDoubleSpinBox * spnImpPri;
    QDoubleSpinBox * spnImpPw;
    QSpinBox       * spnDelay;
    QLabel * statusMsg;

    QDoubleSpinBox * spnWBPower;
    //    QDoubleSpinBox * spnCWPower;


    QDoubleSpinBox * spnCWFrq;
    QDoubleSpinBox * spnSweepStop;
    QDoubleSpinBox * spnSweepStart;
    QDoubleSpinBox * spnSweepStep;
    QDoubleSpinBox * spnimpulseFrq;
    QDoubleSpinBox * spnWideFrq;
    QDoubleSpinBox * spnSpotFrq;
    QCheckBox * powerOn;
    QPushButton * startBtn;
    QPushButton * stopBtn;
    QPushButton * btnDisableSpot;
    QPushButton * btnSetCW;
    QPushButton * btnSetWB;
    QPushButton * btnLoadSpot;
    QPushButton * btnDisableCW;
    QPushButton * btnDisableWB;
    QPushButton * btnStopSweep;
    QPushButton * btnDisImpulse;
    QPushButton * btnStartSweep;
    QPushButton * btnLoadImpulse;

    double minFrqLimit{};
    double maxFrqLimit{};


private:
    Ui::exciter *ui;

    adrv * adrvObj ;
    QString ipAddress{};
    bool isConnect{false};
    bool firstRun{true};
    QString fullPath;
    QString fileName;

    bool isExciterOn{false};
    QSet<QString> activeModes;
    void setModeActive(const QString &mode, bool on);

    // Phase 5: active ADRV9009 profile bandwidth (100/200/400 MHz)
    int profileBw{100};

    enum tabState{
        CW,Spot,Sweep,Impulse,WB
    };

    void createImpulseFile(double, double, QString&, double);
    bool existsFile (const std::string& name);
    bool returnfilePath(QString&);

    tabState currentTabState;
    QTimer exciterConnection;
    bool isUserLoggedIn{};

//    QPushButton * btnPtr ;

private slots:
    void on_btnDisableSpot_clicked();
    void on_btnDisImpulse_clicked();
    void on_btnStopSweep_clicked();
    void getDataSlot();
    void setDataSlot();

    void on_btnDisableCW_clicked(bool checked);
    void on_btnDisableWB_clicked(bool checked);

public slots:
    void joshanFuncDataSlot();
    void joshanStatusDataSlot();

    // Phase 5: active ADRV9009 profile bandwidth (100/200/400 MHz), set from
    // the receiver Profile tab. Selects spot{N}mhz_{P}.txt in the Spot tab.
    void setProfileBw(int bwMHz);
    //    void receiveIpAddressSlot(bool);
    void connectToDevice();
    void initConnection(bool);
    void addingCartElementsToExciter();
    void changingDacMsg(QString,QString);
    void joshanControlDataSlot(QJsonObject);
    void smartNoiseIsActiveSlot();
    void isUserLoggedInSlot(double);



signals:
    void connectionStatusSignal(bool);
    void sendFuncDataToJoshanSignal(QJsonObject);
    void sendStatusDataToJoshanSignal(QJsonObject);
    void sendFrqDataSignal(QString);
    void sendPowerToCart(double);
    void sendSweepToCart();
    void sendStartFrqToCart(double);
    void sendStoptFrqToCart(double);
    void sendStepFrqToCart(double);
    void startHopp();
    void stoptHopp();
    void sendFileToCardSignal(QString, double,QString);
    void changeDacSignal(QString);
    void modeActivitySignal(bool anyModeActive);
    void turnOffSmartNoiseSignal();
};


#endif // EXCITER_H
