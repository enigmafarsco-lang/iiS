// ============================================================================
// File: SetUpTheme.cpp
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for SetUpTheme.cpp.
// ============================================================================

#include "SetUpTheme.h"

// Constructor: Initializes SetUpTheme subsystem components, state, and UI bindings.
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

// Sets a parameter and updates internal configuration.
void SetUpTheme::setA(QApplication *value)
{
    a = value;
}

// Initializes init radio button for SetUpTheme.
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

// Initializes init push button for SetUpTheme.
void SetUpTheme::initPushButton()
{
    btnStart = new QPushButton("Next");
    btnStart->setCursor(Qt::PointingHandCursor);
}

// Initializes init group box for SetUpTheme.
void SetUpTheme::initGroupBox()
{
    grbThem = new QGroupBox("Select Them");
}


// Sets group box them parameter and updates internal configuration.
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



// Executes se form routine.
void SetUpTheme::seForm()
{
    QVBoxLayout* vblForm = new QVBoxLayout();
    vblForm->addWidget(grbThem);
    vblForm->addSpacing(20);
    vblForm->addWidget(btnStart);
    this->setLayout(vblForm);
}

// Sets style for application parameter and updates internal configuration.
void SetUpTheme::setStyleForApplication()
{
    QFile styleFile(qssAddress);
    styleFile.open(QFile::ReadOnly);

    QString style = QLatin1String(styleFile.readAll());
    a->setStyleSheet(style);
    this->setStyleSheet(style);
}

// Sets connection parameter and updates internal configuration.
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


// Executes slot start routine.
void SetUpTheme::slot_start()
{
    this->close();
}

// Sets default parameter and updates internal configuration.
void SetUpTheme::set_Default()
{
    qssAddress = "";
    setStyleForApplication();
}

// Sets a m o l e d parameter and updates internal configuration.
void SetUpTheme::set_AMOLED()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/AMOLED.qss";
    setStyleForApplication();

}

// Sets aqua parameter and updates internal configuration.
void SetUpTheme::set_Aqua()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/Aqua.qss";
    setStyleForApplication();

}

// Sets aref parameter and updates internal configuration.
void SetUpTheme::set_Aref()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/aref.qss";
    setStyleForApplication();
}

// Sets console style parameter and updates internal configuration.
void SetUpTheme::set_ConsoleStyle()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/ConsoleStyle.qss";
    setStyleForApplication();
}

// Sets elegant dark parameter and updates internal configuration.
void SetUpTheme::set_ElegantDark()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/MaterialDark.qss";
    setStyleForApplication();
}

// Sets manjaro mix parameter and updates internal configuration.
void SetUpTheme::set_ManjaroMix()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/ManjaroMix.qss";
    setStyleForApplication();
}

// Sets ubuntu parameter and updates internal configuration.
void SetUpTheme::set_Ubuntu()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/Ubuntu.qss";
    setStyleForApplication();
}

// Sets fa parameter and updates internal configuration.
void SetUpTheme::set_Fa()
{
    qssAddress = ":/Resources/Resources_Folder/QSS-master/fa.qss";
    setStyleForApplication();
}

// Executes slot show form routine.
void SetUpTheme::slotShowForm()
{
    this->setWindowTitle("Select Them");
    QDesktopWidget desktop;
    int monitorWidth  = desktop.width();
    int monitorHeight = desktop.height();
    this->resize(monitorWidth * .3, monitorHeight * .3);
    this->show();
}
