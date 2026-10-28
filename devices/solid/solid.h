// ============================================================================
// File: solid.h
// Subsystem: eLynxSDR solid
// Description: Subsystem implementation and interface guidance for solid.h.
// ============================================================================

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
class Solid;
}

class Solid : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes Solid subsystem components, default parameters, and state.
    explicit Solid(QWidget *parent = nullptr);
    // Destructor: Cleans up Solid resources, timers, and allocated memory.
    ~Solid();

public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray);
    // Executes joshan func data slot routine.
    void joshanFuncDataSlot();
    // Executes joshan status data slot routine.
    void joshanStatusDataSlot();


signals:
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);

public:
    // Executes handle info routine.
    void handle_info(QByteArray);


signals:
    // Transmits send command to other object signal command/packet over communication interface.
    void sendCommandToOtherObjectSignal(QString);

public slots:
    // Reads and parses incoming receive command from other object slot data.
    void receiveCommandFromOtherObjectSlot(QString);
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);
    // Executes joshan control data slot routine.
    void joshanControlDataSlot(QJsonObject);

    // Event/Signal handler for timeout.
    void timeout();

private slots:
    // Event/Signal handler for on chk alc mode clicked.
    void on_chkAlcMode_clicked(bool checked);

    // Event/Signal handler for on btn enable clicked.
    void on_btnEnable_clicked();

    // Event/Signal handler for on btn disable clicked.
    void on_btnDisable_clicked();

    // Event/Signal handler for on btn reset clicked.
    void on_btnReset_clicked();

    // Event/Signal handler for on btn solid status clicked.
    void on_btnSolidStatus_clicked();

    // Event/Signal handler for on rdb high clicked.
    void on_rdbHigh_clicked();

    // Event/Signal handler for on rdb medium clicked.
    void on_rdbMedium_clicked();

    // Event/Signal handler for on rdb low clicked.
    void on_rdbLow_clicked();


    // Event/Signal handler for on chk err lacth mode clicked.
    void on_chkErrLacthMode_clicked(bool checked);

signals:
    // Transmits send status to outer node signal command/packet over communication interface.
    void sendStatusToOuterNodeSignal(QJsonObject);
    // Transmits send func data to joshan signal command/packet over communication interface.
    void sendFuncDataToJoshanSignal(QJsonObject);
    // Transmits send status data to joshan signal command/packet over communication interface.
    void sendStatusDataToJoshanSignal(QJsonObject);
    // Transmits send status temp signal command/packet over communication interface.
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
