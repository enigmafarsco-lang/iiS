/**
 * @file settings.cpp
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "settings.h"
#include <receiver/datafiles.h>
#include <qfile.h>
#include <QFileInfo>
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
    QFile file(dataFilePath(QStringLiteral("settings.txt")));

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
    QString lastFilter = jsonData["LastFilter"].toString();
    if (!lastFilter.isEmpty() && !QFileInfo::exists(lastFilter))
        lastFilter.clear();
    setLastFilter(lastFilter);

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
    QFile file(dataFilePath(QStringLiteral("settings.txt")));

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

/**
 * @brief Sets the ip parameter and updates internal state.
 * @param newIp Input parameter specifying newIp (const QString).
 */
void Settings::setIp(const QString &newIp)
{
    Ip = newIp;
}

const QString &Settings::getPort() const
{
    return Port;
}

/**
 * @brief Sets the port parameter and updates internal state.
 * @param newPort Input parameter specifying newPort (const QString).
 */
void Settings::setPort(const QString &newPort)
{
    Port = newPort;
}

const QString &Settings::getMode() const
{
    return Mode;
}

/**
 * @brief Sets the mode parameter and updates internal state.
 * @param newMode Input parameter specifying newMode (const QString).
 */
void Settings::setMode(const QString &newMode)
{
    Mode = newMode;
}

/**
 * @brief Retrieves the current last filter value.
 * @return Output value of type QString.
 */
QString Settings::getLastFilter() const
{
    return LastFilter;
}

/**
 * @brief Sets the last filter parameter and updates internal state.
 * @param newLastFilter Input parameter specifying newLastFilter (const QString).
 */
void Settings::setLastFilter(const QString &newLastFilter)
{
    LastFilter = newLastFilter;
}

#pragma endregion }
