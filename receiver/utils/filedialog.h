// ============================================================================
// File: filedialog.h
// Subsystem: eLynxSDR utils
// Description: Subsystem implementation and interface guidance for filedialog.h.
// ============================================================================

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QtCore>
#include <QtGui>

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor: Initializes FileDialog subsystem components, default parameters, and state.
    explicit FileDialog(QWidget *parent = nullptr);

    // Destructor: Cleans up FileDialog resources, timers, and allocated memory.
    ~FileDialog();

private slots:
    // Event/Signal handler for on tree view clicked.
    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::FileDialog *ui;

    // For show directories
    QFileSystemModel *dirmodel;
    // For show files
    QFileSystemModel *filemodel;
};

#endif // FILEDIALOG_H
