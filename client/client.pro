QT -= gui

TARGET = client
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

CONFIG(debug, debug|release): DEFINES += _DEBUG
CONFIG(release, debug|release): DEFINES += NDEBUG

CONFIG(release, debug|release): DESTDIR = $$PWD/../build/MOBILE_CLIENT/release
CONFIG(debug, debug|release): DESTDIR = $$PWD/../build/MOBILE_CLIENT/debugs

INCLUDEPATH += $$PWD/../common

LIBS += -lcrypto \
        -lssl \


TEMPLATE = app

SOURCES += main.cpp \
    ../common/Worker.cpp \
    Client.cpp \
    ../common/define.cpp \
    ../common/Helper.cpp \
    ClientChat.cpp \
    MessageManager.cpp

HEADERS += \
    ../common/Worker.h \
    Client.h \
    ../common/define.h \
    ../common/Helper.h \
    ClientChat.h \
    MessageManager.h

system(cp $$PWD/SSL/* $PWD/../build/MOBILE_CLIENT/debugs)
