#ifndef TARGETDETECTOR_H
#define TARGETDETECTOR_H

#include <QWidget>
#include "utils.h"
#include "../../usertype.h"
namespace Ui {
class TargetDetector;
}

class TargetDetector : public QWidget
{
    Q_OBJECT

public:
    explicit TargetDetector(QWidget *parent = nullptr);
    ~TargetDetector();

private slots:
    void on_chbAutoSet_clicked(bool checked);

public slots:
    void receiveCommandFromObjectSlot(QString);
    void udpDeviceResponseSlot(QByteArray);
    void tcpDeviceResponseSlot(QByteArray);

signals:
    void sendCommandToObjectSignal(QString);
    void sendCommandToDeviceSignal(QByteArray);


private:
    Ui::TargetDetector *ui;
    double BearingDegree = -1;
    double ElevationDegree = -1;
    double elevationDegree = -1;
};


#endif // TARGETDETECTOR_H
