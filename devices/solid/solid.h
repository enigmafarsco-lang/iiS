/**
 * @file solid.h
 * @brief Subsystem module for solid in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef SOLID_H
#define SOLID_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QMovie>
#include <QJsonObject>
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"


namespace Ui {
/**
 * @class Solid
 * @brief High-level controller class for Solid.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Solid;
}

/**
 * @class Solid
 * @brief High-level controller class for Solid.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Solid : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new Solid instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit Solid(QWidget *parent = nullptr);
    /**
     * @brief Destructor for Solid, releasing allocated hardware resources, timers, and buffers.
     */
    ~Solid();

public slots:
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray);
    /**
     * @brief Executes the joshan func data slot procedure for Solid.
     */
    void joshanFuncDataSlot();
    /**
     * @brief Executes the joshan status data slot procedure for Solid.
     */
    void joshanStatusDataSlot();


signals:
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);

public:
    /**
     * @brief Executes the handle info procedure for Solid.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void handle_info(QByteArray);


signals:
    /**
     * @brief Transmits send command to other object signal command/packet over the communication bus.
     * @param QString Input parameter specifying QString (QString).
     */
    void sendCommandToOtherObjectSignal(QString);

public slots:
    /**
     * @brief Reads, parses, and decodes incoming receive command from other object slot data buffer.
     * @param QString Input parameter specifying QString (QString).
     */
    void receiveCommandFromOtherObjectSlot(QString);
    /**
     * @brief Retrieves the current evice connected slot value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isDeviceConnectedSlot(bool);
    /**
     * @brief Executes the joshan control data slot procedure for Solid.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void joshanControlDataSlot(QJsonObject);

    /**
     * @brief Slot handler responding to timeout event.
     */
    void timeout();

private slots:
    /**
     * @brief Slot handler responding to on chk alc mode clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chkAlcMode_clicked(bool checked);

    /**
     * @brief Slot handler responding to on btn enable clicked event.
     */
    void on_btnEnable_clicked();

    /**
     * @brief Slot handler responding to on btn disable clicked event.
     */
    void on_btnDisable_clicked();

    /**
     * @brief Slot handler responding to on btn reset clicked event.
     */
    void on_btnReset_clicked();

    /**
     * @brief Slot handler responding to on btn solid status clicked event.
     */
    void on_btnSolidStatus_clicked();

    /**
     * @brief Slot handler responding to on rdb high clicked event.
     */
    void on_rdbHigh_clicked();

    /**
     * @brief Slot handler responding to on rdb medium clicked event.
     */
    void on_rdbMedium_clicked();

    /**
     * @brief Slot handler responding to on rdb low clicked event.
     */
    void on_rdbLow_clicked();


    /**
     * @brief Slot handler responding to on chk err lacth mode clicked event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_chkErrLacthMode_clicked(bool checked);

signals:
    /**
     * @brief Transmits send status to outer node signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusToOuterNodeSignal(QJsonObject);
    /**
     * @brief Transmits send func data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendFuncDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send status data to joshan signal command/packet over the communication bus.
     * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
     */
    void sendStatusDataToJoshanSignal(QJsonObject);
    /**
     * @brief Transmits send status temp signal command/packet over the communication bus.
     * @param int Input parameter specifying int (int).
     */
    void sendStatusTempSignal(int);

private:
    Ui::Solid *ui;

    QString m_commandString = "";
    QByteArray m_commandBytes;

    //============
    //    - White (text): #eff0f1
    //    - Dark black (main bg): #272a2d
    //    - Lighter dark black (menu bar): #373b3f
    //    - Dark blue: #18465d
    //    - Gray: #76797C
    //    - Red (accents): #db1c49
    //    -Purpel : rgb(85, 85, 127)
    //======================
    QString orginal_color;

    QString purpel_color = "background-color: rgb(85, 85, 127)";
    QString gray_color = "background-color: #76797C";
    QString yellow_color = "background-color: yellow";

    QString silver_color = "background-color: silver";
    QString white_color = "background-color: white; color: blue";
    QString warningStyle ="background-color:yellow;color: black";
    QString dangerStyle ="background-color:"+QString(CRIMSON)+";color: "+QString(DARK_OLIVE_GREEN );
    QTimer blinkingTimer;

    QString m_state = "UNKNOWN";

    QString m_defaultStyleSheet = "";
    bool isDeviceConnected{false};

    QMovie *movie;
};

#endif // SOLID_H
