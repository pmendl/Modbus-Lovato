#include "ModbusSerialMaster.h"


#include <QDebug>
#include <QtGlobal>
#include <QSettings>

#include <time.h>

#include "DataUnits.h"

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
	int retries(settings.value("Modbus/MaxRetries", 3).toInt());
	int timeout;

	while(retries--) {
		clearError();
		response->clear();
		timeout = settings.value("Modbus/InitialReadTimeout", 100).toInt();
		qDebug() << "Request sent: " << request.toHex();
		write(request);
		if(error()) {
			qDebug() <<	"Error: " << errorString();
			return response;
		}
		clearError();
		while(waitForReadyRead(timeout)) {
			timeout=settings.value("Modbus/ConsequentReadTimeout", 5).toInt();
			response->append(read(256));
			int r;
			if((r=response->bytesToRead()) == 0) {
				qDebug() << "Collected" << response->size() << "bytes: " << response->toHex();
				return response;
			}
			else {
				qDebug().nospace() << "\tRead incomplete: to read = " << r;
				if (error())
					qDebug() << ", error = " << errorString();
			}
		}
		qDebug() << "\tRead attempt" << settings.value("Modbus/MaxRetries", 3).toInt() - retries << "timeouted.";
	}
	return ADUSharedPtr_t();
}
