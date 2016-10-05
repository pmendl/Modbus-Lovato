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
		qDebug() << "\tModbusSerialMaster" << device << "opened at speed" << baudRate << "Bd";
	}
	else {
		qDebug() << "\tModbusSerialMaster " << device << "open failed ! (error=" << errorString() << ")";
	}
}

/**
 * @brief Send request to Modbus serial line and accept response (or timeout)
 * @param request Valid Modbus request to be sent
 * @return Either of:
 * 1) Valid Modbus response
 * 2) Error status (designated by exception code of 0x00, which is not used in Modbus standard)
 * and containing SerialPortError (Qt enum) values of error received on transmittion,
 * or 0x00 (invalid in this context otherwise) meaning "serial port not opened" in the
 */
ADUSharedPtr_t ModbusSerialMaster::process(ADUSharedPtr_t request)
{
	QSettings settings;
	ADUSharedPtr_t response(new ApplicationDataUnitSerial());
	int retries(settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXRETRIES_KEY), MODBUS_MAXRETRIES_DEFAULT).toInt());
//	volatile int timeout;

	if(!isOpen()) {
		qDebug() <<	"\tModbusSerialMaster: Device not opened - aborting request processing";
		return response;
	}

	while(retries--) {
		clearError();
		response->clear();
		write(*request);
		if(error()) {
			qDebug() <<	"\tModbusSerialMaster: Error: " << errorString();
			response->append(static_cast<char>(0)); // Note - valid only for ApplicationDataUnitSerial, i.e. aduPrefixSize() == 1
			response->append(static_cast<char>(0));
			response->append(error());
			return response;
		}
//		qDebug() << "*** CHECKPOINT ALPHA";
		// Do notice +1 at end of initial value for first attempt
		int consequents(settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXCONSEQUENTS_KEY), MODBUS_MAXCONSEQUENTREADS_DEFAULT).toInt() +1 );
//		qDebug() << "***" << settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_INITIALREADTIMEOUT_KEY), MODBUS_INITIALREADTIMEOUT_DEFAULT).toInt() << settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_CONSEQUENTREADTIMEOUT_KEY), MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT).toInt();
		waitForReadyRead(settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_INITIALREADTIMEOUT_KEY), MODBUS_INITIALREADTIMEOUT_DEFAULT).toInt());
		do {
			response->append(read(256));
			if(response->isValid()) {
				return response;
			}
//			qDebug() << "*** CHECKPOINT BRAVO";
			struct timespec ts = { 0, settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_INCYCLEWAIT_KEY), MODBUS_INCYCLEWAIT_DEFAULT).toInt()};
			nanosleep(&ts, NULL);
		} while((--consequents>0) && waitForReadyRead(settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_CONSEQUENTREADTIMEOUT_KEY), MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT).toInt()));
	}
	qDebug() << "FAILED after " << settings.value(xstr(MODBUS_GROUP_NAME) "/" xstr(MODBUS_MAXRETRIES_KEY), MODBUS_MAXRETRIES_DEFAULT).toInt() << "retries !!!";
	return ADUSharedPtr_t();
}
