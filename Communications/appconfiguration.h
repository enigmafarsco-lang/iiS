#ifndef APPCONFIGURATION_H
#define APPCONFIGURATION_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include "usertype.h"

class  AppConfiguration : public QObject
{
    Q_OBJECT
private:
    void updatAttributesFromUserType();
public:
    AppConfiguration();
    ~AppConfiguration();

    bool readSettingFromFile();
    bool WriteSettingToFile();
    bool defaultSetting();
    QByteArray makeJson();

public:
    QString       m_settingFilename;
    AppSettings * m_setting;

signals:
    void initAppSettingSignal();


};

#endif // APPCONFIGURATION_H
