#-------------------------------------------------
# Tanca Petanque contest manager
#-------------------------------------------------

BASE_DIR = $${PWD}

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build-tanca/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build-tanca/release
}

# ------------------------------------------------------------------------------
# Where to find header files
# ------------------------------------------------------------------------------

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc


# ------------------------------------------------------------------------------
# ICL Configuration
# ------------------------------------------------------------------------------

CONFIG += icl_zip icl_database

ICL_DIR = $$BASE_DIR/src/icl
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
QT       += core gui sql widgets
RC_FILE = assets/icon.rc
TEMPLATE = app


VPATH += src src/ui
INCLUDEPATH += src src/ui


windows {
    DEFINES += USE_WINDOWS_OS


    *-g++* {
        # MinGW
        QMAKE_CXXFLAGS += -std=c++11
        LIBS +=  -lws2_32 -lpsapi
    }

    *-msvc* {
        # MSVC
        QMAKE_LIBS += ws2_32.lib #psapi.lib setupapi.lib cfgmgr32.lib advapi32.lib
    }

} else {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl
}


TARGET = Tanca


# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------
SOURCES += main.cpp MainWindow.cpp \
    DbManager.cpp \
    PlayerWindow.cpp \
    TeamWindow.cpp \
    DatePickerWindow.cpp \
    GameWindow.cpp \
    EventWindow.cpp \
    TableHelper.cpp \
    SelectionWindow.cpp \
    ScoreWindow.cpp \
    Tournament.cpp \
    Server.cpp
    tests/test_tournament.cpp

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    TeamWindow.h \
    DatePickerWindow.h \
    Observer.h \
    GameWindow.h \
    EventWindow.h \
    TableHelper.h \
    SelectionWindow.h \
    ScoreWindow.h \
    Tournament.h \
    Server.h \
    IDataBase.h

FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    SelectionWindow.ui \
    EventWindow.ui \
    ScoreWindow.ui \
    AboutWindow.ui \
    RewardWindow.ui

OTHER_FILES += README.md LICENSE

RESOURCES += assets/tanca.qrc

# End of project file
