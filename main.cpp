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
     QFile file(":/resources/QSS-master/aref.qss");
      file.open(QFile::ReadOnly);
       QString styleSheet = QString::fromLatin1(file.readAll());
        a.setStyleSheet(styleSheet);

         MainWindow w;
          w.show();

           return a.exec();
}
