/**
 * @file main.cpp
 * @brief Application Entry Point for the eLynxSDR Software Defined Radio Suite.
 *
 * eLynxSDR is a high-performance graphical workstation application for controlling
 * wideband RF transceivers (ADRV9009 / AD9371), managing Digital Radio Frequency
 * Memory (DRFM) radar target simulation pipelines, and generating multi-waveform
 * exciter signals (CW, Spot noise, Sweeps, Pulses, WideBand noise, Multi-Target, LFM).
 *
 * Architecture Summary:
 * - Single-window unified GUI: The Exciter subsystem is integrated directly into
 *   the Receiver main form as a primary tab, providing streamlined operational workflows.
 * - Global QSS stylesheet ("aref.qss") styling providing dark-theme contrast.
 * - Multi-threaded signal processing, asynchronous TCP/IP telemetry, and libiio DMA streaming.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 * @date 2026-10-28
 */

#include "mainwindow.h"
#include <errno.h>
#include <glib.h>
#include <iio.h>
#include <QSplashScreen>
#include <QPixmap>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QFile>

/**
 * @brief Application entry point.
 * @param argc Command-line argument count.
 * @param argv Command-line argument vector.
 * @return Exit code returned by QApplication::exec().
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("eLynxSDR");
    a.setApplicationVersion("v0.0.2");

    QFile file(":/resources/QSS-master/aref.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QString::fromLatin1(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
    }

    MainWindow w;
    // The Transmitter window is not shown: the exciter moved into the
    // receiver form (see MainWindow::MainWindow -> addExciterTab) and
    // the form is the only window of eLynxSDR now.

    return a.exec();
}
