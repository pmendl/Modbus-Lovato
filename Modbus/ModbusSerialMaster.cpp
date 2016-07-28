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
	if(QSerialPort::open(QIODevice::ReadWrite)) {
		qDebug() << "\tModbusSerialMaster" << device << "opened at speed=" << baudRate;
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

	while(retries--) {
		clearError();
		response->clear();
		timeout = settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_INITIALREADTIMEOUT_KEY), MODBUS_INITIALREADTIMEOUT_DEFAULT).toInt();
		write(*request);
		if(error()) {
			qDebug() <<	"Error: " << errorString();
			return response;
		}
		clearError();
		while(waitForReadyRead(timeout)) {
			timeout=settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_CONSEQUENTREADTIMEOUT_KEY), MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT).toInt();
			response->append(read(256));
			if(response->isValid()) {
//				qDebug() << "Collected" << response->size() << "bytes: " << response->toHex();
				return response;
			}
		}
	}
	qDebug() << "FAILED after " << settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXRETRIES_KEY), MODBUS_MAXRETRIES_DEFAULT).toInt() << "retries !!!";
	return ADUSharedPtr_t();
}
