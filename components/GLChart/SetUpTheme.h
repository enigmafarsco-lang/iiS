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
    void initRadioButton();
    void initPushButton();
    void initGroupBox();

    void setGroupBoxThem();
    void seForm();
    void setStyleForApplication();
    void setConnection();

public:
    explicit SetUpTheme(QWidget *parent = nullptr);

    static SetUpTheme* getInstance(QWidget *parent = nullptr);

    void setA(QApplication *value);

private slots:
    void slot_start();

    void set_Default();
    void set_AMOLED();
    void set_Aqua();
    void set_Aref();
    void set_ConsoleStyle();
    void set_ElegantDark();
    void set_ManjaroMix();
    void set_Ubuntu();
    void set_Fa();

public slots:
    void slotShowForm();

};

#endif // SETUPTHEME_H
