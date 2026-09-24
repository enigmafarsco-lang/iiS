#include "settings.h"
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
