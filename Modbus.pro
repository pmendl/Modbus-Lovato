QT += core network
QT -= gui

CONFIG += c++11

TARGET = Modbus
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /mnt/raspberry-rootfs/usr/include
INCLUDEPATH += /home/pavel/local_work-Pavel/Qt/Raspberry/Compile_from_source/raspberry/qt-everywhere-opensource-src-5.5.1/qtserialport/include
LIBS += -L/mnt/raspberry-rootfs/usr/local/Qt-5.5.1-raspberry/lib/ -lQt5SerialPort

target.path = /home/pavel/Modbus
INSTALLS += target

SOURCES += main.cpp \
    Network/NetworkResource.cpp \
    Network/NetworkAccessBase.cpp \
    Console/KeyboardScanner.cpp \
    Modbus/ModbusSerialMaster.cpp \
    Modbus/DataUnits.cpp \
    Modbus/CrcPolynomial.cpp

HEADERS += \
    Network/NetworkResource.h \
    Network/NetworkAccessBase.h \
    Console/KeyboardScanner.h \
    Modbus/ModbusSerialMaster.h \
    Modbus/DataUnits.h \
    Globals.h \
    Modbus/CrcPolynomial.h
