#ifndef MODBUSSERIALMASTER_H
#define MODBUSSERIALMASTER_H

#include <QtSerialPort/QSerialPort>

class ModbusSerialMaster : public QSerialPort
{
public:
	explicit ModbusSerialMaster(QString device, QObject *parent=0, qint32 baudRate=QSerialPort::Baud115200);
};

#endif // MODBUSSERIALMASTER_H
