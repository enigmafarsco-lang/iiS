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

class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidget(QWidget *parent = nullptr);
    ~StatusWidget();
public slots:
    void On_DeviceResponseSlot(QByteArray);
    void connectionStatusSlot(bool);

signals:
    void sendCommandToDeviceSignal(QByteArray);

public:
     void handle_info(QByteArray);


signals:
     void sendCommandToOtherObjectSignal(QString);

public slots:
    void receiveCommandFromOtherObjectSlot(QString);
    void exciterConnectionSlot(bool);


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

    Ui::StatusWidget *ui;

signals:
    void isJoshanStop(bool);
};

#endif // STATUSWIDGET_H
