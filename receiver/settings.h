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

// The Initial Setup settings (files/initial_setup.ini - written by the
// small Initial Setup app beside the project).  The main software reads
// them at startup and asserts the settings.
namespace InitialSetup {
    QString ip();      // the board IP  - default "192.168.1.10"
    int profile();     // 100/200/400   - default 200
    bool tx1();        // TX1 on/off    - default false (off)
}

class Settings
{

public:
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
    void ReadSettingFile();
    void ReadJson();
    void SaveToFile();

    const QString &getIp() const;
    void setIp(const QString &newIp);
    const QString &getPort() const;
    void setPort(const QString &newPort);
    const QString &getMode() const;
    void setMode(const QString &newMode);

    QString getLastFilter() const;
    void setLastFilter(const QString &newLastFilter);

private:

    QString Ip;
    QString Port;
    QString Mode;
    QString LastFilter;

};

#endif // SETTINGS_H
