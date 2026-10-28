// ============================================================================
// File: filedialog.cpp
// Subsystem: eLynxSDR utils
// Description: Subsystem implementation and interface guidance for filedialog.cpp.
// ============================================================================

#include "filedialog.h"
#include "ui_filedialog.h"

// Constructor: Initializes FileDialog subsystem components, default parameters, and state.
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

// Destructor: Cleans up FileDialog allocated resources, timers, and buffers.
FileDialog::~FileDialog()
{
    delete ui;
}

// Signal/Slot handler responding to on tree view clicked event.
void FileDialog::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath=dirmodel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(filemodel->setRootPath(sPath));
}
