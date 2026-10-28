/**
 * @file menupage.cpp
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "menupage.h"
#include "ui_menupage.h"

MenuPage::MenuPage(QWidget *parent) :
    QMenuBar(parent),
    ui(new Ui::MenuPage)
{
    ui->setupUi(this);

    initToolBar();
}


/**
 * @brief Executes the show menu option slot procedure for MenuPage.
 * @param isEnable Input parameter specifying isEnable (bool).
 */
void MenuPage::showMenuOptionSlot(bool isEnable)
{
    menuSimulateRadar->setEnabled(isEnable);
    menuSimulateAllRadars->setEnabled(isEnable);
    menuGenerateRadar->setEnabled(isEnable);
    menuUpload->setEnabled(isEnable);
}

//Creating menu
void MenuPage::initFunction()
{
    connect(actionClose,  &QAction::triggered, this, &QWidget::close);

    menuFileMenu = this->addMenu(tr("&File"));
    menuFileMenu->addAction(actionNewProject);
    menuFileMenu->addAction(actionSaveProject);
    menuFileMenu->addAction(actionSaveProjectAs);
    menuFileMenu->addAction(actionLoadProject);
    menuFileMenu->addSeparator();
    menuFileMenu->addAction(actionClose);

    menuSimulateRadar = this->addMenu(tr("Simulate Radar"));
    menuSimulateAllRadars = this->addMenu(tr("Simulate All Radars"));

    menuGenerateRadar = this->addMenu(tr("Generate radar"));
    menuGenerateRadar->addAction(generateRadarAction);

    menuUpload = this->addMenu(tr("Upload"));


    //--------------------------------------------

    showMenuOptionSlot(false);
}

//Menu configuration (File)
void MenuPage::initToolBar()
{
    actionNewProject    = new QAction();
    actionSaveProject   = new QAction();
    actionSaveProjectAs = new QAction();
    actionLoadProject   = new QAction();
    actionClose         = new QAction();

    generateRadarAction       = new QAction();

    const QIcon openIcon   = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    const QIcon saveIcon   = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    const QIcon newIcon    = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    const QIcon exitIcon   = QIcon::fromTheme("application-exit");

    actionNewProject   ->setIcon(newIcon);
    actionSaveProject  ->setIcon(saveIcon);
    actionSaveProjectAs->setIcon(saveAsIcon);
    actionLoadProject  ->setIcon(openIcon);
    actionClose        ->setIcon(exitIcon);

    actionNewProject    ->setText(tr("Add New Raddar"));
    actionSaveProject   ->setText(tr("Save Project"));
    actionSaveProjectAs ->setText(tr("Save Project As"));
    actionLoadProject   ->setText(tr("Load Project"));
    actionClose         ->setText(tr("Close"));
    generateRadarAction ->setText("generate");

    actionNewProject    -> setShortcuts(QKeySequence::New);
    actionSaveProject   -> setShortcuts(QKeySequence::Save);
    actionSaveProjectAs -> setShortcuts(QKeySequence::SaveAs);
    actionLoadProject   -> setShortcuts(QKeySequence::Open);
    actionClose         -> setShortcuts(QKeySequence::Quit);

    connect(actionSaveProject   ,  &QAction::triggered, this, &MenuPage::saveProject  );
    connect(actionSaveProjectAs ,  &QAction::triggered, this, &MenuPage::saveProjectAs);
    connect(actionLoadProject   ,  &QAction::triggered, this, &MenuPage::loadProject  );
    connect(generateRadarAction ,  &QAction::triggered, this, &MenuPage::generateRadar);
    initFunction();
}

//Adding new radar
void MenuPage::newProject()
{

}

/**
 * @brief Executes the save project procedure for MenuPage.
 */
void MenuPage::saveProject()
{
    if (curFile.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("There Is Nothing To Save, Add New Radar.");
        msgBox.setInformativeText("");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else
    {
        saveFile();
    }

}

/**
 * @brief Executes the save file procedure for MenuPage.
 * @return Output value of type bool.
 */
bool MenuPage::saveFile()
{
    //curFile is the neme of our file and it must be selected by user
    QFile saveFile(curFile);

    if ( (!saveFile.open(QIODevice::WriteOnly)) ) {
        qWarning("Couldn't open save file.");
        return false;
    }

    // the file must complete and here we pass it to doc
    //    QByteArray doc = makeJson();
    //    saveFile.write(doc);
    return true;
}

void MenuPage::generateRadar()
{

    menuGenerateSignal();
}

/**
 * @brief Executes the save project as procedure for MenuPage.
 */
void MenuPage::saveProjectAs()
{
    std::cout<<" this works" << std::endl;
}

/**
 * @brief Executes the load project procedure for MenuPage.
 * @return Output value of type bool.
 */
bool MenuPage::loadProject()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (!fileName.isEmpty())
    {
        QFile loadFile(fileName);

        if (!loadFile.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), loadFile.errorString()));
            return false;
        }

        std::cout<<" this still works" << std::endl;
        QByteArray saveData = loadFile.readAll();
        //    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        //    QJsonObject json = loadDoc.object();

        return  true;
    }


    else {
        return  false;
    }

}

/**
 * @brief Executes the close procedure for MenuPage.
 */
void MenuPage::close()
{
    std::cout<<" this works" << std::endl;
}

/**
 * @brief Destructor for MenuPage, releasing allocated hardware resources, timers, and buffers.
 */
MenuPage::~MenuPage()
{
    delete ui;
}
