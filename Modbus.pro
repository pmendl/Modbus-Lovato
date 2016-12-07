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

INSTALLS += target config services test

#target.path = /home/pavel/Modbus
target.path = /usr/local/bin/Modbus_binaries

config.files = Data/Config/*
config.path = /home/pavel/.config/PMCS

test.files = Data/Test/*
TEST_COMMAND_FILES_PATH = /usr/local/bin/Modbus_binaries/Test
test.path = $$TEST_COMMAND_FILES_PATH
DEFINES += TEST_COMMAND_FILES_PATH=$$TEST_COMMAND_FILES_PATH

services.files = Data/RPi_services/*
services.path = /etc/init.d


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
    Log/LogFragment.cpp \
    Network/CommandsDistributor.cpp \
    Commands/CommandsProcessor.cpp \
    Commands/CommandsList.cpp \
    Commands/CommandDescriptor.cpp \
    Log/LogCopier.cpp \
    Network/DebugHttpMultiPart.cpp \
    Network/ExtendedHttpMultiPart.cpp \
    Processing/MemoryParsingProcessor.cpp \
    Debug/MemoryAnalytics.cpp \
    Debug/InstanceCounterBase.cpp \
    Commands/CommandLogFilter.cpp \
    Commands/CommandCopyFilter.cpp \
    Commands/CommandReplaceFilter.cpp \
    Commands/CommandDeleteFilter.cpp \
    System/Memory.cpp \
    System/Reset.cpp \
    System/PrioritiesCountingHash.cpp \
    System/TrueCandidates.cpp \
    Processing/ResetParsingProcessor.cpp \
    System/WatchdogController.cpp \
    System/GpioPin.cpp \
    System/SignalisationController.cpp \
    Commands/CommandResetFilter.cpp

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
    Log/LogFragment.h \
    Network/CommandsDistributor.h \
    Commands/CommandsProcessor.h \
    Commands/CommandsList.h \
    Commands/CommandDescriptor.h \
    Log/LogCopier.h \
    Network/DebugHttpMultiPart.h \
    Network/ExtendedHttpMultiPart.h \
    Processing/MemoryParsingProcessor.h \
    Debug/DebugMacros.h \
    Debug/MemoryAnalytics.h \
    Debug/InstanceCounterBase.h \
    Commands/CommandLogFilter.h \
    Commands/CommandCopyFilter.h \
    Commands/CommandReplaceFilter.h \
    Commands/CommandDeleteFilter.h \
    System/Memory.h \
    System/Reset.h \
    System/PrioritiesCountingHash.h \
    System/TrueCandidates.h \
    Processing/ResetParsingProcessor.h \
    System/WatchdogController.h \
    System/GpioPin.h \
    System/SignalisationController.h \
    Commands/CommandResetFilter.h

DISTFILES += \
    Data/Config/LovatoModbus.conf \
    Doxygen/MainPage.dox \
    Doxygen/IniFormat.dox \
    Data/Test/Headers.cmd \
    Data/Test/Log.cmd \
    Data/Test/Copy.cmd \
    Data/Test/Delete.cmd \
    Data/Test/Replace.cmd \
    Data/Test/User.ini \
    Data/Test/System.ini \
    Data/RPi_Services/gpio_18_27_userspace.sh \
    Data/Test/Reset.cmd
