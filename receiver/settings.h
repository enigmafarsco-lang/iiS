// ============================================================================
// File: settings.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for settings.h.
// ============================================================================

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QDebug>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class Settings;
}

class Settings
{

public:
    // Constructor: Initializes Settings subsystem components, default parameters, and state.
    explicit Settings();

    //saeid raziani
    enum class modes {
        FRQ,
        PAPEAK,
        PASUM,
        PRI,
        PW1,
        PW2,
        TOA,
        CHANNEL
    } ;
    // Executes read setting file routine.
    void ReadSettingFile();
    // Executes read json routine.
    void ReadJson();
    // Executes save to file routine.
    void SaveToFile();

    const QString &getIp() const;
    // Sets ip value and updates internal subsystem configuration.
    void setIp(const QString &newIp);
    const QString &getPort() const;
    // Sets port value and updates internal subsystem configuration.
    void setPort(const QString &newPort);
    const QString &getMode() const;
    // Sets mode value and updates internal subsystem configuration.
    void setMode(const QString &newMode);

    // Retrieves current last filter state.
    QString getLastFilter() const;
    // Sets last filter value and updates internal subsystem configuration.
    void setLastFilter(const QString &newLastFilter);

private:

    QString Ip;
    QString Port;
    QString Mode;
    QString LastFilter;

};

#endif // SETTINGS_H
