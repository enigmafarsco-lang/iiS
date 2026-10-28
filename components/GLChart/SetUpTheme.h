// ============================================================================
// File: SetUpTheme.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for SetUpTheme.h.
// ============================================================================

#ifndef SETUPTHEME_H
#define SETUPTHEME_H

#include <QWidget>
#include "QGroupBox"
#include "QPushButton"
#include "QRadioButton"
#include "QVBoxLayout"
#include "QFile"

#include "QDesktopWidget"
#include "QApplication"
class SetUpTheme : public QWidget
{
    Q_OBJECT
private:
    QRadioButton* rdbDefault;
    QRadioButton* rdbAMOLED;
    QRadioButton* rdbAqua;
    QRadioButton* rdbAref;
    QRadioButton* rdbConsoleStyle;
    QRadioButton* rdbElegantDark;
    QRadioButton* rdbManjaroMix;
    QRadioButton* rdbUbuntu;
    QRadioButton* rdbFa;

    QPushButton* btnStart;

    QGroupBox* grbThem;

    QApplication* a;
    QString qssAddress;

private:
    // Initializes init radio button for SetUpTheme.
    void initRadioButton();
    // Initializes init push button for SetUpTheme.
    void initPushButton();
    // Initializes init group box for SetUpTheme.
    void initGroupBox();

    // Sets group box them value and updates internal subsystem configuration.
    void setGroupBoxThem();
    // Executes se form routine.
    void seForm();
    // Sets style for application value and updates internal subsystem configuration.
    void setStyleForApplication();
    // Sets connection value and updates internal subsystem configuration.
    void setConnection();

public:
    // Constructor: Initializes SetUpTheme subsystem components, default parameters, and state.
    explicit SetUpTheme(QWidget *parent = nullptr);

    // Retrieves current instance state.
    static SetUpTheme* getInstance(QWidget *parent = nullptr);

    // Sets a value and updates internal subsystem configuration.
    void setA(QApplication *value);

private slots:
    // Executes slot start routine.
    void slot_start();

    // Sets default value and updates internal subsystem configuration.
    void set_Default();
    // Sets a m o l e d value and updates internal subsystem configuration.
    void set_AMOLED();
    // Sets aqua value and updates internal subsystem configuration.
    void set_Aqua();
    // Sets aref value and updates internal subsystem configuration.
    void set_Aref();
    // Sets console style value and updates internal subsystem configuration.
    void set_ConsoleStyle();
    // Sets elegant dark value and updates internal subsystem configuration.
    void set_ElegantDark();
    // Sets manjaro mix value and updates internal subsystem configuration.
    void set_ManjaroMix();
    // Sets ubuntu value and updates internal subsystem configuration.
    void set_Ubuntu();
    // Sets fa value and updates internal subsystem configuration.
    void set_Fa();

public slots:
    // Executes slot show form routine.
    void slotShowForm();

};

#endif // SETUPTHEME_H
