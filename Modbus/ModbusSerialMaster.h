#ifndef MODBUSSERIALMASTER_H
#define MODBUSSERIALMASTER_H

#include <QtSerialPort/QSerialPort>
#include <QSharedPointer>

#include "Globals.h"

class ModbusSerialMaster : public QSerialPort
{
public:
	explicit ModbusSerialMaster(QString device, QObject *parent=0, qint32 baudRate=QSerialPort::Baud115200);
	ADUSharedPtr_t process(ApplicationDataUnitSerial &request);
};

#endif // MODBUSSERIALMASTER_H
