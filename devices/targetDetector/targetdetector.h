// ============================================================================
// File: targetdetector.h
// Subsystem: eLynxSDR targetDetector
// Description: Subsystem implementation and interface guidance for targetdetector.h.
// ============================================================================

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
    // Constructor: Initializes TargetDetector subsystem components, default parameters, and state.
    explicit TargetDetector(QWidget *parent = nullptr);
    // Destructor: Cleans up TargetDetector resources, timers, and allocated memory.
    ~TargetDetector();

private slots:
    // Event/Signal handler for on chb auto set clicked.
    void on_chbAutoSet_clicked(bool checked);

public slots:
    // Reads and parses incoming receive command from object slot data.
    void receiveCommandFromObjectSlot(QString);
    // Executes udp device response slot routine.
    void udpDeviceResponseSlot(QByteArray);
    // Executes tcp device response slot routine.
    void tcpDeviceResponseSlot(QByteArray);

signals:
    // Transmits send command to object signal command/packet over communication interface.
    void sendCommandToObjectSignal(QString);
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);


private:
    Ui::TargetDetector *ui;
    double BearingDegree = -1;
    double ElevationDegree = -1;
    double elevationDegree = -1;
};


#endif // TARGETDETECTOR_H
