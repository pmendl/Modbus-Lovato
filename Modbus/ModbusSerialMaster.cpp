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
		qDebug() << "Opened at speed=" << baudRate;
	}
	else {
		qDebug() << "Open " << device << " failed ! (error=" << errorString() << ")";
	}
}

ADUSharedPtr_t ModbusSerialMaster::process(ApplicationDataUnitSerial &request)
{
	QSettings settings;
	ADUSharedPtr_t response(new ApplicationDataUnitSerial());
	int retries(settings.value("Modbus/MaxRetries", MODBUS_MAXRETRIES_DEFAULT).toInt());
	int timeout;

	while(retries--) {
		clearError();
		response->clear();
		timeout = settings.value("Modbus/InitialReadTimeout", MODBUS_INITIALREADTIMEOUT_DEFAULT).toInt();
		write(request);
		if(error()) {
			qDebug() <<	"Error: " << errorString();
			return response;
		}
		clearError();
		while(waitForReadyRead(timeout)) {
			timeout=settings.value("Modbus/ConsequentReadTimeout", MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT).toInt();
			response->append(read(256));
			if(response->isValid()) {
				qDebug() << "Collected" << response->size() << "bytes: " << response->toHex();
				return response;
			}
		}
	}
	qDebug() << "FAILED after " << settings.value("Modbus/MaxRetries", MODBUS_MAXRETRIES_DEFAULT).toInt() << "retries !!!";
	return ADUSharedPtr_t();
}
