#ifndef DATAFILES_H
#define DATAFILES_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

// Resolve a data/config file to the project files/ folder (the canonical
// location - PROJECT_FILES_DIR, compiled in by eLynxSDR.pro), then to the
// legacy locations (working-dir files/, application files/, working dir).
// The files the app uses at runtime live in files/: Appsetting.json,
// settings.txt, Calibration.txt, upf, font/arial.ttf and the signal/
// generator.  The first existing hit wins (a plain working-dir copy keeps
// working); when nothing exists yet the files/ path is returned, so the
// app CREATES new files (saved settings, user accounts, calibration
// tables) in the project files folder.
inline QString dataFilePath(const QString &fileName)
{
    QStringList dirs;
#ifdef PROJECT_FILES_DIR
    dirs << QStringLiteral(PROJECT_FILES_DIR);
#endif
    dirs << QDir::currentPath() + QStringLiteral("/files")
         << QCoreApplication::applicationDirPath() + QStringLiteral("/files")
         << QDir::currentPath();
    for (const QString &d : dirs) {
        const QString pth = QDir(d).filePath(fileName);
        if (QFileInfo::exists(pth))
            return pth;
    }
#ifdef PROJECT_FILES_DIR
    return QDir(QStringLiteral(PROJECT_FILES_DIR)).filePath(fileName);
#else
    return QDir(QDir::currentPath() + QStringLiteral("/files")).filePath(fileName);
#endif
}

#endif // DATAFILES_H
