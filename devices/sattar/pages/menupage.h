// ============================================================================
// File: menupage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for menupage.h.
// ============================================================================

#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <iostream>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>

#include "../components/sattarutils.h"


namespace Ui {
class MenuPage;
}

class MenuPage : public QMenuBar
{
    Q_OBJECT

public:
    // Constructor: Initializes MenuPage subsystem components, default parameters, and state.
    explicit MenuPage(QWidget *parent = nullptr);
    // Destructor: Cleans up MenuPage resources, timers, and allocated memory.
    ~MenuPage();

    QAction * actionNewProject;
    QAction * actionSaveProject;
    QAction * actionSaveProjectAs;
    QAction * actionLoadProject;
    QAction * actionClose;

    QAction * generateRadarAction;
//    QMenuBar *menuBar;
    QMenu * menuFileMenu;
    QMenu * menuAddNewRadar;
    QMenu * menuSimulateRadar;
    QMenu * menuSimulateAllRadars;
    QMenu * menuGenerateRadar;
    QMenu * menuUpload;


private:
    QPlainTextEdit *textEdit;
    QString curFile="";
    Ui::MenuPage *ui;


private:
    // Initializes init function for MenuPage.
    void initFunction();
    // Executes create menus routine.
    void createMenus();
    // Initializes init tool bar for MenuPage.
    void initToolBar();
    // Executes new project routine.
    void newProject();
    // Executes save project routine.
    void saveProject();
    // Executes save project as routine.
    void saveProjectAs();
    // Executes load project routine.
    bool loadProject();
    // Executes close routine.
    void close();
    // Executes save file routine.
    bool saveFile();


private slots:
    // Executes generate radar routine.
    void generateRadar();


public slots:
    // Executes show menu option slot routine.
    void showMenuOptionSlot(bool);

signals:
    // Executes menu generate signal routine.
    void menuGenerateSignal();
};

#endif // MENU_H
