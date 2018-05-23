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
    LehaStar.cpp \
    Protobuf/MessageType.pb.cc \
    Crypto/CryptoHelper.cpp \
    ClientView.cpp \
    AdminView.cpp

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
    LehaStar.h \
    Protobuf/MessageType.pb.h \
    Crypto/CryptoHelper.h \
    ClientView.h \
    AdminView.h

system(cp $$PWD/SSL/* $PWD/../build/MOBILE_CLIENT/debugs)
system(cp $$PWD/Config/config.cfg $PWD/../build/MOBILE_CLIENT/debugs)
system(rm -rf Protobuf && mkdir Protobuf)

PROTOPATH = $$PWD/../mobileoperator_proto/protoSourse
system(protoc --proto_path=$${PROTOPATH} --cpp_out=./Protobuf $${PROTOPATH}/Message.proto)
system(protoc --proto_path=$${PROTOPATH} --cpp_out=./Protobuf $${PROTOPATH}/MessageType.proto)


DISTFILES += \
    Config/config.cfg \
    ../mobileoperator_proto/protoSourse/MessageType.proto \
    ../mobileoperator_proto/protoSourse/Message.proto

FORMS += \
    LehaStar.ui \
    clientview.ui \
    adminview.ui
