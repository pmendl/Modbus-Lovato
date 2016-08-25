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

INSTALLS += target config

target.path = /home/pavel/Modbus

config.files = Data/Config/*
config.path = /home/pavel/.config/PMCS

SOURCES += main.cpp \
    Network/NetworkAccessBase.cpp \
    Console/KeyboardScanner.cpp \
    Modbus/ModbusSerialMaster.cpp \
    Modbus/DataUnits.cpp \
    Modbus/CrcPolynomial.cpp \
    Processing/RequestManager.cpp \
    Processing/ProcessingManager.cpp \
    Processing/PostParsingProcessor.cpp \
    Processing/LogParsingProcessor.cpp \
    Log/LogServer.cpp \
    Processing/ParsingProcessor.cpp \
    Log/LogReader.cpp \
    Network/NetworkSender.cpp \
    Log/LogFragment.cpp

HEADERS += \
    Network/NetworkAccessBase.h \
    Console/KeyboardScanner.h \
    Modbus/ModbusSerialMaster.h \
    Modbus/DataUnits.h \
    Globals.h \
    Modbus/CrcPolynomial.h \
    Processing/RequestManager.h \
    Processing/ProcessingManager.h \
    Processing/ParsingProcessor.h \
    Processing/PostParsingProcessor.h \
    Processing/AllParsingProcessors.h \
    Processing/LogParsingProcessor.h \
    Log/LogServer.h \
    Log/LogReader.h \
    Network/NetworkSender.h \
    Log/LogFragment.h

DISTFILES += \
    Data/Config/LovatoModbus.conf \
    Doxygen/MainPage.dox \
    Doxygen/IniFormat.dox
