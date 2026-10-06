#include "mainwindow.h"
#include <errno.h>
#include <glib.h>
#include <iio.h>
#include <QSplashScreen>
#include <QPixmap>
#include <QApplication>
#include <QDebug>
//#include <receiver/connectdialog.h>

#include <QApplication>

#include <QVBoxLayout>

#include <QFile>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setApplicationName("eLynxSDR");
    a.setApplicationVersion("v0.0.2");
     QFile file(":/resources/QSS-master/aref.qss");
      file.open(QFile::ReadOnly);
       QString styleSheet = QString::fromLatin1(file.readAll());
        a.setStyleSheet(styleSheet);

         MainWindow w;
          // The Transmitter window is not shown: the exciter moved into the
          // receiver form (see MainWindow::MainWindow -> addExciterTab) and
          // the form is the only window of eLynxSDR now.

           return a.exec();
}
