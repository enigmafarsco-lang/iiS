#ifndef DATAFILES_H
#define DATAFILES_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>

// Writable per-user copy of the application's runtime files. The package
// installs its read-only defaults under /opt/eLynxSDR/files; the first time
// a file is requested, dataFilePath() seeds it here so settings, calibration,
// generated waveforms and logs never try to write into /opt or the source tree.
inline QString userFilesDirectory()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appData.isEmpty())
        appData = QDir::homePath() + QStringLiteral("/.local/share/eLynxSDR");

    const QString filesDir = QDir(appData).filePath(QStringLiteral("files"));
    QDir().mkpath(filesDir);
    return QDir::cleanPath(filesDir);
}

// Find the shipped, read-only files directory. A packaged build deliberately
// does not compile in the builder's absolute checkout path; it discovers the
// assets next to the installed executable instead. Developer builds retain
// PROJECT_FILES_DIR as their first choice for Qt Creator shadow builds.
inline QString resourceFilesDirectory()
{
    QStringList dirs;
#ifndef ELYNXSDR_PACKAGED
#  ifdef PROJECT_FILES_DIR
    dirs << QStringLiteral(PROJECT_FILES_DIR);
#  endif
#endif

    const QString appDir = QCoreApplication::applicationDirPath();
    dirs << QDir(appDir).filePath(QStringLiteral("files"))
         << QDir(appDir).filePath(QStringLiteral("../files"))
         << QDir(appDir).filePath(QStringLiteral("../share/eLynxSDR/files"))
         << QDir::current().filePath(QStringLiteral("files"));

    for (const QString &dir : dirs) {
        if (QFileInfo(dir).isDir())
            return QDir(dir).absolutePath();
    }

    // This is also the install layout used by the Ubuntu package. Returning
    // the expected path lets callers report a useful missing-file location.
    return QDir(appDir).absoluteFilePath(QStringLiteral("../files"));
}

inline QString resourceFilePath(const QString &fileName)
{
    return QDir(resourceFilesDirectory()).absoluteFilePath(fileName);
}

// Return the writable per-user path for every runtime file. Existing files
// are preserved between app upgrades; shipped defaults are copied on first
// use, and new files get a writable parent directory automatically.
inline QString dataFilePath(const QString &fileName)
{
    const QString userPath = QDir(userFilesDirectory()).absoluteFilePath(fileName);
    if (QFileInfo::exists(userPath))
        return userPath;

    const QString packagedPath = resourceFilePath(fileName);
    if (QFileInfo::exists(packagedPath)) {
        QDir().mkpath(QFileInfo(userPath).absolutePath());
        if (QFile::copy(packagedPath, userPath))
            QFile::setPermissions(userPath, QFileInfo(packagedPath).permissions());
    } else {
        QDir().mkpath(QFileInfo(userPath).absolutePath());
    }

    return userPath;
}

#endif // DATAFILES_H
