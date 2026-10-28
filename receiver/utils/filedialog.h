/**
 * @file filedialog.h
 * @brief Subsystem module for utils in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QtCore>
#include <QtGui>

namespace Ui {
/**
 * @class FileDialog
 * @brief High-level controller class for FileDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class FileDialog;
}

/**
 * @class FileDialog
 * @brief High-level controller class for FileDialog.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class FileDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new FileDialog instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit FileDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor for FileDialog, releasing allocated hardware resources, timers, and buffers.
     */
    ~FileDialog();

private slots:
    /**
     * @brief Slot handler responding to on tree view clicked event.
     * @param index Input parameter specifying index (const QModelIndex).
     */
    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::FileDialog *ui;

    // For show directories
    QFileSystemModel *dirmodel;
    // For show files
    QFileSystemModel *filemodel;
};

#endif // FILEDIALOG_H
