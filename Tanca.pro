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

RC_FILE = assets/icon.rc
TARGET = Tanca
TEMPLATE = app


VPATH += src
VPATH += src/lua
INCLUDEPATH += src
INCLUDEPATH += src/lua

QMAKE_CXXFLAGS += -std=c++11

win32 {
    DEFINES += USE_WINDOWS_OS
    LIBS += -lpsapi

} else {
    DEFINES += USE_UNIX_OS
}

SOURCES += main.cpp MainWindow.cpp \
    DbManager.cpp \
    PlayerWindow.cpp \
    TeamWindow.cpp \
    BracketWindow.cpp \
    DatePickerWindow.cpp \
    Log.cpp \
    Util.cpp \
    UniqueId.cpp \
    GameWindow.cpp \
    EventWindow.cpp \
    TableHelper.cpp \
    SelectionWindow.cpp \
    ScoreWindow.cpp \
    lundump.c \
    lutf8lib.c \
    lvm.c \
    lzio.c \
    lapi.c \
    lauxlib.c \
    lbaselib.c \
    lbitlib.c \
    lcode.c \
    lcorolib.c \
    lctype.c \
    ldblib.c \
    ldebug.c \
    ldo.c \
    ldump.c \
    lfunc.c \
    lgc.c \
    linit.c \
    liolib.c \
    llex.c \
    lmathlib.c \
    lmem.c \
    loadlib.c \
    lobject.c \
    lopcodes.c \
    loslib.c \
    lparser.c \
    lstate.c \
    lstring.c \
    lstrlib.c \
    ltable.c \
    ltablib.c \
    ltm.c

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    TeamWindow.h \
    BracketWindow.h \
    DatePickerWindow.h \
    Log.h \
    Util.h \
    UniqueId.h \
    Observer.h \
    GameWindow.h \
    EventWindow.h \
    TableHelper.h \
    SelectionWindow.h \
    ScoreWindow.h \
    luaconf.h \
    lualib.h \
    lundump.h \
    lvm.h \
    lzio.h \
    lua.hpp \
    lapi.h \
    lauxlib.h \
    lcode.h \
    lctype.h \
    ldebug.h \
    ldo.h \
    lfunc.h \
    lgc.h \
    llex.h \
    llimits.h \
    lmem.h \
    lobject.h \
    lopcodes.h \
    lparser.h \
    lprefix.h \
    lstate.h \
    lstring.h \
    ltable.h \
    ltm.h

FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    SelectionWindow.ui \
    EventWindow.ui \
    ScoreWindow.ui \
    AboutWindow.ui

OTHER_FILES += README.md LICENSE

RESOURCES += assets/tanca.qrc
