/**
 * @file SetUpTheme.cpp
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "SetUpTheme.h"

/**
 * @brief Constructs a new SetUpTheme instance and initializes subsystem components and UI state.
 * @param parent Input parameter specifying parent (QWidget).
 */
SetUpTheme::SetUpTheme(QWidget *parent) : QWidget(parent)
{
    initRadioButton();
    initPushButton();
    initGroupBox();
    setGroupBoxThem();
    seForm();
    setConnection();

    rdbAref->clicked(true);
    rdbAref->setChecked(true);
}

SetUpTheme *SetUpTheme::getInstance(QWidget *parent)
{
    static SetUpTheme *instance = nullptr;
    if(instance == nullptr)
    {
        instance = new SetUpTheme(parent);

    }
    return instance;
}

/**
 * @brief Sets the a parameter and updates internal state.
 * @param value Input parameter specifying value (QApplication).
 */
void SetUpTheme::setA(QApplication *value)
{
    a = value;
}

/**
 * @brief Initializes and configures the init radio button for the SetUpTheme subsystem.
 */
void SetUpTheme::initRadioButton()
{
    rdbDefault      = new QRadioButton("Default");
    rdbAMOLED       = new QRadioButton("AMOLED");
    rdbAqua         = new QRadioButton("Aqua");
    rdbAref         = new QRadioButton("Aref");
    rdbConsoleStyle = new QRadioButton("Console Style");
    rdbElegantDark  = new QRadioButton("Elegant Dark");
    rdbManjaroMix   = new QRadioButton("Manjaro Mix");
    rdbUbuntu       = new QRadioButton("Ubuntu");
    rdbFa           = new QRadioButton("Fa");
}

/**
 * @brief Initializes and configures the init push button for the SetUpTheme subsystem.
 */
void SetUpTheme::initPushButton()
{
    btnStart = new QPushButton("Next");
    btnStart->setCursor(Qt::PointingHandCursor);
}

/**
 * @brief Initializes and configures the init group box for the SetUpTheme subsystem.
 */
void SetUpTheme::initGroupBox()
{
    grbThem = new QGroupBox("Select Them");
}


/**
 * @brief Sets the group box them parameter and updates internal state.
 */
void SetUpTheme::setGroupBoxThem()
{
    QVBoxLayout* vblThem = new QVBoxLayout();
    vblThem->addWidget(rdbDefault);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbAMOLED);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbAqua);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbAref);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbConsoleStyle);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbElegantDark);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbManjaroMix);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbUbuntu);
    vblThem->addSpacing(10);
    vblThem->addWidget(rdbFa);

    grbThem->setLayout(vblThem);
}



/**
 * @brief Executes the se form procedure for SetUpTheme.
 */
void SetUpTheme::seForm()
{
    QVBoxLayout* vblForm = new QVBoxLayout();
    vblForm->addWidget(grbThem);
    vblForm->addSpacing(20);
    vblForm->addWidget(btnStart);
    this->setLayout(vblForm);
}

/**
 * @brief Sets the style for application parameter and updates internal state.
 */
void SetUpTheme::setStyleForApplication()
{
    QFile styleFile(qssAddress);
    styleFile.open(QFile::ReadOnly);

    QString style = QLatin1String(styleFile.readAll());
    a->setStyleSheet(style);
    this->setStyleSheet(style);
}

/**
 * @brief Sets the connection parameter and updates internal state.
 */
void SetUpTheme::setConnection()
{
    connect(btnStart       , SIGNAL(clicked()), this, SLOT(slot_start()));
    connect(rdbDefault     , SIGNAL(clicked()), this, SLOT(set_Default()));
    connect(rdbAMOLED      , SIGNAL(clicked()), this, SLOT(set_AMOLED()));
    connect(rdbAqua        , SIGNAL(clicked()), this, SLOT(set_Aqua()));
    connect(rdbAref        , SIGNAL(clicked()), this, SLOT(set_Aref()));
    connect(rdbConsoleStyle, SIGNAL(clicked()), this, SLOT(set_ConsoleStyle()));
    connect(rdbElegantDark , SIGNAL(clicked()), this, SLOT(set_ElegantDark()));
    connect(rdbManjaroMix  , SIGNAL(clicked()), this, SLOT(set_ManjaroMix()));
    connect(rdbUbuntu      , SIGNAL(clicked()), this, SLOT(set_Ubuntu()));
    connect(rdbFa          , SIGNAL(clicked()), this, SLOT(set_Fa()));

}


/**
 * @brief Executes the slot start procedure for SetUpTheme.
 */
void SetUpTheme::slot_start()
{
    this->close();
}

/**
 * @brief Sets the default parameter and updates internal state.
 */
void SetUpTheme::set_Default()
{
    qssAddress = "";
    setStyleForApplication();
}

/**
 * @brief Sets the a m o l e d parameter and updates internal state.
 */
void SetUpTheme::set_AMOLED()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/AMOLED.qss";
    setStyleForApplication();

}

/**
 * @brief Sets the aqua parameter and updates internal state.
 */
void SetUpTheme::set_Aqua()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/Aqua.qss";
    setStyleForApplication();

}

/**
 * @brief Sets the aref parameter and updates internal state.
 */
void SetUpTheme::set_Aref()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/aref.qss";
    setStyleForApplication();
}

/**
 * @brief Sets the console style parameter and updates internal state.
 */
void SetUpTheme::set_ConsoleStyle()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/ConsoleStyle.qss";
    setStyleForApplication();
}

/**
 * @brief Sets the elegant dark parameter and updates internal state.
 */
void SetUpTheme::set_ElegantDark()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/MaterialDark.qss";
    setStyleForApplication();
}

/**
 * @brief Sets the manjaro mix parameter and updates internal state.
 */
void SetUpTheme::set_ManjaroMix()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/ManjaroMix.qss";
    setStyleForApplication();
}

/**
 * @brief Sets the ubuntu parameter and updates internal state.
 */
void SetUpTheme::set_Ubuntu()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/Ubuntu.qss";
    setStyleForApplication();
}

/**
 * @brief Sets the fa parameter and updates internal state.
 */
void SetUpTheme::set_Fa()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/fa.qss";
    setStyleForApplication();
}

/**
 * @brief Executes the slot show form procedure for SetUpTheme.
 */
void SetUpTheme::slotShowForm()
{
    this->setWindowTitle("Select Them");
    QDesktopWidget desktop;
    int monitorWidth  = desktop.width();
    int monitorHeight = desktop.height();
    this->resize(monitorWidth * .3, monitorHeight * .3);
    this->show();
}
