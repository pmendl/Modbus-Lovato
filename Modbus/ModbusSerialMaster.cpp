#include "ModbusSerialMaster.h"


#include <QDebug>
#include <QtGlobal>
#include <QtSerialPort/QSerialPort>

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
//	connect(this, &QSerialPort::readyRead, this, &ModbusSerialMaster::onReadyRead);
}

ApplicationDataUnitSerial *ModbusSerialMaster::process(ApplicationDataUnitSerial &request)
{
	QMetaObject::Connection con=connect(this, &QIODevice::readyRead, this, &ModbusSerialMaster::onReadyRead);
	response = new ApplicationDataUnitSerial();

	clearError();
	qDebug() << "Request sent: " << request.toHex()
			 << "(" << write(request) << "bytes)";
	if(error()) {
		qDebug() <<	"Error: " << errorString();
		disconnect(con);
		return 0;
	}

	forever {
		if(!waitForReadyRead(1000)) {
			qint16 r;
			qDebug() << "No data to read while " << (r=response->bytesToRead()) << "remaining";
			qDebug() << response->toHex();
			if(r <=0 ) break;
		}
	}

	qDebug() << "Collected" << response->size() << "bytes: " << response->toHex();
	disconnect(con);
	delete response;
	response = 0;
	return 0;
}

void ModbusSerialMaster::onReadyRead()
{
	if(!response) return;
	qDebug() << "@";
	response->append(read(256));
}


/*

void UmbServer::adjustAnswer(UmbAnswer *answer, UmbRequest *request) {
	(void)request; // Request used in base class implementation
	UmbProcessorInterface::adjustAnswer(answer, request);
	connect(this, &QIODevice::readyRead, answer, &UmbAnswer::onReadyRead);
	connect(answer, &UmbAnswer::answerReady, this, &UmbServer::onAnswerReady);
}
*/
