#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T15:15:59
#
#-------------------------------------------------

BASE_DIR = $${PWD}/..

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build-tanca/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build-tanca/release
}

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc

QT       += core gui sql widgets

RC_FILE = icon.rc
TARGET = Tanca
TEMPLATE = app

LIBS += -lpsapi

DEFINES += USE_WINDOWS_OS

SOURCES += main.cpp MainWindow.cpp \
    DbManager.cpp \
    PlayerWindow.cpp \
    TeamWindow.cpp \
    BracketWindow.cpp \
    DatePickerWindow.cpp \
    Log.cpp Util.cpp \
    GameWindow.cpp

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    TeamWindow.h \
    BracketWindow.h \
    DatePickerWindow.h \
    Log.h Util.h Observer.h \
    GameWindow.h

FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    TeamWindow.ui \
    GameWindow.ui

OTHER_FILES += README.md LICENSE

RESOURCES += \
    tanca.qrc
