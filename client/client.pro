QT += gui
QT += widgets

TARGET = client
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

CONFIG(debug, debug|release): DEFINES += _DEBUG
CONFIG(release, debug|release): DEFINES += NDEBUG

CONFIG(release, debug|release): DESTDIR = $$PWD/../build/MOBILE_CLIENT/release
CONFIG(debug, debug|release): DESTDIR = $$PWD/../build/MOBILE_CLIENT/debugs

INCLUDEPATH += $$PWD/../common \
               $$PWD/Config

LIBS += -lcrypto \
        -lssl \
        -lconfig++ \
        -lprotobuf


TEMPLATE = app

SOURCES += main.cpp \
    ../common/Worker.cpp \
    Client.cpp \
    ../common/define.cpp \
    ../common/Helper.cpp \
    MessageManager.cpp \
    Config/ConfigManager.cpp \
    Config/GlobalParams.cpp \
    RRManager.cpp \
    $$PWD/Protobuf/Message.pb.cc \
    LehaStar.cpp

HEADERS += \
    ../common/Worker.h \
    Client.h \
    ../common/define.h \
    ../common/Helper.h \
    MessageManager.h \
    Config/ConfigManager.h \
    Config/GlobalParams.h \
    RRManager.h \
    $$PWD/Protobuf/Message.pb.h \
    LehaStar.h

system(cp $$PWD/SSL/* $PWD/../build/MOBILE_CLIENT/debugs)
system(cp $$PWD/Config/config.cfg $PWD/../build/MOBILE_CLIENT/debugs)
system(rm -rf Protobuf && mkdir Protobuf)

PROTOPATH = $$PWD/../mobileoperator_proto/protoSourse
system(protoc --proto_path=$${PROTOPATH} --cpp_out=./Protobuf $${PROTOPATH}/Message.proto)


DISTFILES += \
    Config/config.cfg

FORMS += \
    LehaStar.ui
