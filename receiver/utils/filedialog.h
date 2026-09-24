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
    explicit FileDialog(QWidget *parent = nullptr);

    ~FileDialog();

private slots:
    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::FileDialog *ui;

    // For show directories
    QFileSystemModel *dirmodel;
    // For show files
    QFileSystemModel *filemodel;
};

#endif // FILEDIALOG_H
