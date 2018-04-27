QT -= gui

TARGET = server
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

CONFIG(debug, debug|release): DEFINES += _DEBUG
CONFIG(release, debug|release): DEFINES += NDEBUG

CONFIG(release, debug|release): DESTDIR = $$PWD/../build/MOBILE_SERVER/release
CONFIG(debug, debug|release): DESTDIR = $$PWD/../build/MOBILE_SERVER/debugs

INCLUDEPATH += $$PWD/../common

LIBS += -lpqxx \
        -lconfig++ \
        -lcrypto \
        -lssl \
        -lprotobuf

SOURCES += main.cpp \
    ../common/Worker.cpp \
    Server.cpp \
    Session.cpp \
    ../common/define.cpp \
    ../common/Helper.cpp \
    RRManager.cpp \
    ClientChannel.cpp \
    Config/ConfigManager.cpp \
    Config/GlobalParams.cpp \
    $$PWD/Protobuf/Message.pb.h

HEADERS += \
    ../common/Worker.h \
    Server.h \
    Session.h \
    ../common/define.h \
    ../common/Helper.h \
    RRManager.h \
    ClientChannel.h \
    Config/ConfigManager.h \
    Config/GlobalParams.h \
    $$PWD/Protobuf/Message.pb.cc

system(cp $$PWD/Config/config.cfg $PWD/../build/MOBILE_SERVER/debugs)
system(cp $$PWD/SSL/* $PWD/../build/MOBILE_SERVER/debugs)
system($$PWD/../mobileoperator_proto/protoc.sh):HAS_BIN=FALSE
system(mkdir Protobuf)
system(cp $$PWD/../mobileoperator_proto/sourse/* $$PWD/Protobuf)

DISTFILES += \
    ../dump_db.sql \
    Config/config.cfg
