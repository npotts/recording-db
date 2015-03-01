#-------------------------------------------------
#
# Project created by QtCreator 2013-11-02T21:00:29
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = "Recording-DB"
TEMPLATE = app


SOURCES += main.cpp recording_db.cpp

HEADERS  += recording_db.h

FORMS    += recording_db.ui

RESOURCES += resources.qrc
ICON = resources/icon.icns

OTHER_FILES = resources/icon.icns resources/windows-icon.rc

DEFINES += "__GITREV__=\"\\\"$$system(git describe --tags --abbrev=0)\\\"\""

win32:RC_FILE = resources/windows-icon.rc
win32:QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++ -lpthread
