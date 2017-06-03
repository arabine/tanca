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

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc

QT       += core gui sql widgets quickwidgets

RC_FILE = assets/icon.rc
TARGET = Tanca
TEMPLATE = app


VPATH += src src/ui
INCLUDEPATH += src src/ui

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
    DatePickerWindow.cpp \
    Log.cpp \
    Util.cpp \
    UniqueId.cpp \
    GameWindow.cpp \
    EventWindow.cpp \
    TableHelper.cpp \
    SelectionWindow.cpp \
    ScoreWindow.cpp \
    Tournament.cpp \
    Brackets.cpp \
    tests/test_tournament.cpp

HEADERS  += MainWindow.h \
    DbManager.h \
    PlayerWindow.h \
    TeamWindow.h \
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
    Tournament.h \
    Brackets.h

FORMS    += MainWindow.ui \
    PlayerWindow.ui \
    SelectionWindow.ui \
    EventWindow.ui \
    ScoreWindow.ui \
    AboutWindow.ui \
    RewardWindow.ui

OTHER_FILES += README.md LICENSE

RESOURCES += assets/tanca.qrc

DISTFILES += \
    assets/brackets.qml
