#include "settings.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

// ---- The Initial Setup settings (files/initial_setup.ini) ----

static QString initialSetupPath()
{
    QStringList dirs;
#ifdef PROJECT_FILES_DIR
    dirs << QStringLiteral(PROJECT_FILES_DIR);
#endif
    dirs << QDir::currentPath() + "/files"
         << QCoreApplication::applicationDirPath() + "/files";
    for (const QString &d : dirs) {
        const QString pth = QDir(d).absoluteFilePath("initial_setup.ini");
        if (QFileInfo::exists(pth))
            return pth;
    }
    return QDir(dirs.first()).absoluteFilePath("initial_setup.ini");
}

static QString initialSetupValue(const QString &key)
{
    QFile file(initialSetupPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine()).trimmed();
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq <= 0)
            continue;
        if (line.left(eq).trimmed() == key)
            return line.mid(eq + 1).trimmed();
    }
    return QString();
}

QString InitialSetup::ip()
{
    const QString v = initialSetupValue("ip").trimmed();
    return v.isEmpty() ? QStringLiteral("192.168.1.10") : v;
}

int InitialSetup::profile()
{
    const QString v = initialSetupValue("profile").trimmed();
    if (v == "100")
        return 100;
    if (v == "400")
        return 400;
    return 200; // 200 is the default
}

bool InitialSetup::tx1()
{
    const QString v = initialSetupValue("tx1").trimmed();
    return v == "1" || v.compare("on", Qt::CaseInsensitive) == 0;
}
#include <qfile.h>
#include<qfiledialog.h>
#include<qtextstream.h>
#include<QMessageBox>

//Page settings
Settings::Settings()
{
}

//ReadSettingFile method
void Settings::ReadSettingFile()
{
    //Read the settings file
    QFile file("settings.txt");

    //Open the file
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    //Read all file information as an array
    QByteArray data=file.readAll();

    //Close file
    file.close();

    //Read or write Jason information
    QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

    //Put read information in Jason's object
    QJsonObject jsonData=jsonDoc.object();

    //  Put the information you got from the file in its place in the settings
    setMode(jsonData["Mode"].toString());
    setIp(jsonData["Ip"].toString());
    setPort(jsonData["Port"].toString());
    setLastFilter(jsonData["LastFilter"].toString());

}

//SaveToFile method
void Settings::SaveToFile()
{
    //Put information in the form of a Jason object
    QJsonObject obj=QJsonObject();
    obj["Ip"]=Ip;
    obj["Port"]=Port;
    obj["Mode"]=Mode;
    obj["LastFilter"]=LastFilter;

    //Convert qjsonObject to String
    QJsonDocument doc(obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    //Read text file
    QFile file("settings.txt");

    //Open the file in write-only
    if(file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        //Write data received as a string
        file.write(strJson.toStdString().c_str());

        //Close file
        file.close();
    }

}


#pragma region Getter and Setter {


const QString &Settings::getIp() const
{
    return Ip;
}

void Settings::setIp(const QString &newIp)
{
    Ip = newIp;
}

const QString &Settings::getPort() const
{
    return Port;
}

void Settings::setPort(const QString &newPort)
{
    Port = newPort;
}

const QString &Settings::getMode() const
{
    return Mode;
}

void Settings::setMode(const QString &newMode)
{
    Mode = newMode;
}

QString Settings::getLastFilter() const
{
    return LastFilter;
}

void Settings::setLastFilter(const QString &newLastFilter)
{
    LastFilter = newLastFilter;
}

#pragma endregion }
