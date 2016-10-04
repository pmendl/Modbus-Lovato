#include "ModbusSerialMaster.h"

#include <QDebug>
#include <QtGlobal>
#include <QSettings>

#include <time.h>

#include "DataUnits.h"
#include "Modbus/CrcPolynomial.h"

ModbusSerialMaster::ModbusSerialMaster(QString device, QObject *parent, qint32 baudRate) :
	QSerialPort(device, parent)

{
	setBaudRate(baudRate);
	if(false && QSerialPort::open(QIODevice::ReadWrite)) {
		qDebug() << "\tModbusSerialMaster" << device << "opened at speed" << baudRate << "Bd";
	}
	else {
		qDebug() << "\tModbusSerialMaster " << device << "open failed ! (error=" << errorString() << ")";
	}
}

ADUSharedPtr_t ModbusSerialMaster::process(ADUSharedPtr_t request)
{
	QSettings settings;
	ADUSharedPtr_t response(new ApplicationDataUnitSerial());
	int retries(settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXRETRIES_KEY), MODBUS_MAXRETRIES_DEFAULT).toInt());
	int timeout;

	if(!isOpen()) {
		qDebug() <<	"\tModbusSerialMaster: Device not opened - aborting request processing";
		return response;
	}

	while(retries--) {
		clearError();
		response->clear();
		timeout = settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_INITIALREADTIMEOUT_KEY), MODBUS_INITIALREADTIMEOUT_DEFAULT).toInt();
		write(*request);
		if(error()) {
			qDebug() <<	"\tModbusSerialMaster: Error: " << errorString();
			response->append(static_cast<char>(0));
			response->append(error());
			return response;
		}
		clearError();
		qDebug() << "*** CHECKPOINT ALPHA";
		while(waitForReadyRead(timeout)) {
			qDebug() << "*** CHECKPOINT BRAVO";
			timeout=settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_CONSEQUENTREADTIMEOUT_KEY), MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT).toInt();
			response->append(read(256));
			if(response->isValid()) {
				return response;
			}
		}
	}
	qDebug() << "FAILED after " << settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXRETRIES_KEY), MODBUS_MAXRETRIES_DEFAULT).toInt() << "retries !!!";
	return ADUSharedPtr_t();
}
