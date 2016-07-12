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
//	QMetaObject::Connection con=connect(this, &QIODevice::readyRead, this, &ModbusSerialMaster::onReadyRead);
	response = new ApplicationDataUnitSerial();

	clearError();
	qDebug() << "Request sent: " << request.toHex()
			 << "(" << write(request) << "bytes)";
	if(error()) {
		qDebug() <<	"Error: " << errorString();
//		disconnect(con);
		return 0;
	}

	waitForReadyRead(100);
	int s(0), l(-1);
	forever {
		if(!waitForReadyRead(2)) {
			qint16 r;
			qDebug() << "No data to read while " << (r=response->bytesToRead()) << "remaining";
			if((r <= 0) || (r == l) ) break;
			l = r;
			qDebug() << response->toHex();
			if(waitForReadyRead(100)) l=r;

		}
		else {
//		QByteArray a=read(256);
//		s+=a.size();

			response->append(read(256));
		}
	}
//	qDebug() << "Collected" << s << "bytes...";

	qDebug() << "Collected" << response->size() << "bytes: " << response->toHex();
//	disconnect(con);
	delete response;
	response = 0;
	return 0;
}

/*
void ModbusSerialMaster::onReadyRead()
{
	if(!response) return;
	qDebug() << "@";
	response->append(read(256));
}
*/

/*

void UmbServer::adjustAnswer(UmbAnswer *answer, UmbRequest *request) {
	(void)request; // Request used in base class implementation
	UmbProcessorInterface::adjustAnswer(answer, request);
	connect(this, &QIODevice::readyRead, answer, &UmbAnswer::onReadyRead);
	connect(answer, &UmbAnswer::answerReady, this, &UmbServer::onAnswerReady);
}
*/
