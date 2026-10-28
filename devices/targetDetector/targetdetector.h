/**
 * @file targetdetector.h
 * @brief Subsystem module for targetDetector in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TARGETDETECTOR_H
#define TARGETDETECTOR_H

#include <QWidget>
#include "utils.h"
#include "../../usertype.h"
namespace Ui {
/**
 * @class TargetDetector
 * @brief High-level controller class for TargetDetector.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TargetDetector;
}

/**
 * @class TargetDetector
 * @brief High-level controller class for TargetDetector.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TargetDetector : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new TargetDetector instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit TargetDetector(QWidget *parent = nullptr);
    /**
     * @brief Destructor for TargetDetector, releasing allocated hardware resources, timers, and buffers.
     */
    ~TargetDetector();

private slots:
    /**
     * @brief Slot handler responding to on chb auto set clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chbAutoSet_clicked(bool checked);

public slots:
    /**
     * @brief Reads, parses, and decodes incoming receive command from object slot data buffer.
     * @param QString Input parameter specifying QString (QString).
     */
    void receiveCommandFromObjectSlot(QString);
    /**
     * @brief Executes the udp device response slot procedure for TargetDetector.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void udpDeviceResponseSlot(QByteArray);
    /**
     * @brief Executes the tcp device response slot procedure for TargetDetector.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void tcpDeviceResponseSlot(QByteArray);

signals:
    /**
     * @brief Transmits send command to object signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendCommandToObjectSignal(QString);
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);


private:
    Ui::TargetDetector *ui;
    double BearingDegree = -1;
    double ElevationDegree = -1;
    double elevationDegree = -1;
};


#endif // TARGETDETECTOR_H
