#ifndef MODBUSSERIALMASTER_H
#define MODBUSSERIALMASTER_H

#include <QtSerialPort/QSerialPort>

class ModbusSerialMaster : public QSerialPort
{
public:
	explicit ModbusSerialMaster(QString device, QObject *parent=0, qint32 baudRate=QSerialPort::Baud115200);
	class ApplicationDataUnitSerial *process(ApplicationDataUnitSerial &request);
/*
protected slots:
	void onReadyRead();
*/
private:
	ApplicationDataUnitSerial *response;
};

#endif // MODBUSSERIALMASTER_H
