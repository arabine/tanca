#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T15:15:59
#
#-------------------------------------------------

QT       += core gui sql widgets

TARGET = Tanca
TEMPLATE = app

SOURCES += main.cpp MainWindow.cpp \
    DbManager.cpp \
    PlayerWindow.cpp \
    TeamWindow.cpp \
    BracketWindow.cpp \
    DatePickerWindow.cpp

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    TeamWindow.h \
    BracketWindow.h \
    DatePickerWindow.h

FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    TeamWindow.ui \
    ResultWindow.ui

OTHER_FILES += README.md LICENSE

RESOURCES += \
    tanca.qrc
