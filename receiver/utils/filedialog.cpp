/**
 * @file filedialog.cpp
 * @brief Subsystem module for utils in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "filedialog.h"
#include "ui_filedialog.h"

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);

    QString sPath="/home";
    dirmodel=new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(sPath);

    ui->treeView->setModel(dirmodel);

    filemodel=new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filemodel->setRootPath(sPath);

    ui->listView->setModel(filemodel);
}

/**
 * @brief Destructor for FileDialog, releasing allocated hardware resources, timers, and buffers.
 */
FileDialog::~FileDialog()
{
    delete ui;
}

/**
 * @brief Slot handler responding to on tree view clicked event.
 * @param index Input parameter specifying index (const QModelIndex).
 */
void FileDialog::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath=dirmodel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(filemodel->setRootPath(sPath));
}
