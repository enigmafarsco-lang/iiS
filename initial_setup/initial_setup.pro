# Initial Setup - the small settings app beside the main project.
# Run it once, fill the form (IP / profile / TX1) and press Save: it
# writes files/initial_setup.ini.  The main software reads that file at
# startup and asserts the settings.
QT += core gui widgets
TARGET = initial_setup
TEMPLATE = app
CONFIG += c++11
SOURCES += main.cpp
# The MAIN PROJECT files folder (next to the project .pro).
DEFINES += PROJECT_FILES_DIR=\\\"$$PWD/../files\\\"
