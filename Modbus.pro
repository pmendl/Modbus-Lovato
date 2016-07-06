QT += core network
QT -= gui

CONFIG += c++11

TARGET = Modbus
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /mnt/raspberry-rootfs/usr/include

target.path = /home/pavel/Modbus
INSTALLS += target

SOURCES += main.cpp \
    Network/NetworkResource.cpp \
    Test.cpp \
    Network/NetworkAccessBase.cpp

HEADERS += \
    Network/NetworkResource.h \
    Test.h \
    Network/NetworkAccessBase.h
