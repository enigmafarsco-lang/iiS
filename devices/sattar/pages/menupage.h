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
    explicit MenuPage(QWidget *parent = nullptr);
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
    void initFunction();
    void createMenus();
    void initToolBar();
    void newProject();
    void saveProject();
    void saveProjectAs();
    bool loadProject();
    void close();
    bool saveFile();


private slots:
    void generateRadar();


public slots:
    void showMenuOptionSlot(bool);

signals:
    void menuGenerateSignal();
};

#endif // MENU_H
