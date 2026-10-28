/**
 * @file settings.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class Settings
 * @brief High-level controller class for Settings.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Settings;
}

/**
 * @class Settings
 * @brief High-level controller class for Settings.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Settings
{

public:
    /**
     * @brief Constructs a new Settings instance and initializes subsystem components and UI state.
     */
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
    /**
     * @brief Executes the read setting file procedure for Settings.
     */
    void ReadSettingFile();
    /**
     * @brief Executes the read json procedure for Settings.
     */
    void ReadJson();
    /**
     * @brief Executes the save to file procedure for Settings.
     */
    void SaveToFile();

    const QString &getIp() const;
    /**
     * @brief Sets the ip parameter and updates internal state.
     * @param newIp Input parameter specifying newIp (const QString).
     */
    void setIp(const QString &newIp);
    const QString &getPort() const;
    /**
     * @brief Sets the port parameter and updates internal state.
     * @param newPort Input parameter specifying newPort (const QString).
     */
    void setPort(const QString &newPort);
    const QString &getMode() const;
    /**
     * @brief Sets the mode parameter and updates internal state.
     * @param newMode Input parameter specifying newMode (const QString).
     */
    void setMode(const QString &newMode);

    /**
     * @brief Retrieves the current last filter value.
     * @return Output value of type QString.
     */
    QString getLastFilter() const;
    /**
     * @brief Sets the last filter parameter and updates internal state.
     * @param newLastFilter Input parameter specifying newLastFilter (const QString).
     */
    void setLastFilter(const QString &newLastFilter);

private:

    QString Ip;
    QString Port;
    QString Mode;
    QString LastFilter;

};

#endif // SETTINGS_H
