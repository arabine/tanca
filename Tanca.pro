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
    RoundWindow.cpp \
    TeamWindow.cpp \
    MatchWindow.cpp

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    RoundWindow.h \
    TeamWindow.h \
    MatchWindow.h
FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    Matchwindow.ui \
    RoundWindow.ui
