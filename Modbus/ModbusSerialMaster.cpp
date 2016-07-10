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
	clearError();
	qDebug() << "Request sent: " << request.toHex()
			 << "(" << write(request) << "bytes)";
	if(error()) {
		qDebug() <<	"Error: " << errorString();
		return 0;
	}

	if(!waitForReadyRead(1000)) {
		qDebug() << "Timeouted while waiting for response !";
		return 0;
	}

	ApplicationDataUnitSerial response;
	response.reserve(300);
	char *ptr = response.data();
	int r, s(0);
	while (response.bytesToRead() > 0) {
		waitForReadyRead(5);
		qDebug() << "Reading " << response.bytesToRead() << "->" << (r=read(ptr, response.bytesToRead())) << response.size();
		ptr+=r;
		s+=r;
		response.resize(s);
		qDebug() << s;
	}


//	ApplicationDataUnitSerial response(static_cast<QByteArray>(read(256)));
	qDebug() << "Read" << response.size() << "bytes:" << response.toHex();

/*
		const struct timespec delay({0, 999*1000*1000});
		//const struct timespec delay({1, 0});
		nanosleep(&delay,0);
*/


/*
	QByteArray a(256,0);
	int cnt(0);
	char *ptr = a.data();
	while(waitForReadyRead(100)) {
		 ptr += read(ptr, 256);
	}
	qDebug() << "Read" << ptr -a.data() << "bytes: ";
	qDebug() << "Collected" << a.size() << "bytes: " << a.toHex();
*/


	return 0;
}

/*
void ModbusSerialMaster::onReadyRead()
{
	qDebug() << "READY READ DETECTED";
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
