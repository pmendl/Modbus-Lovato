#include "ModbusSerialMaster.h"

#include <QDebug>

#include "DataUnits.h"

ModbusSerialMaster::ModbusSerialMaster(QString device, QObject *parent, qint32 baudRate) :
	QSerialPort(device, parent)
{
	setBaudRate(baudRate);
//	setBaudRate(baudRate, QSerialPort::Input);
//	setBaudRate(baudRate, QSerialPort::Output);
	if(QSerialPort::open(QIODevice::ReadWrite)) {
		qDebug() << "Opened at speed=" << baudRate;
	}
	else {
		qDebug() << "Open " << device << " failed ! (error=" << errorString() << ")";
	}
	connect(this, &QSerialPort::readyRead, this, &ModbusSerialMaster::onReadyRead);
}

ApplicationDataUnitSerial *ModbusSerialMaster::process(ApplicationDataUnitSerial &request)
{
	/*
	 * THE FOLLOWING IS PREPARATION FOR INTER-CHARACTER DELAY MEASUREMENTS
	 * Abandoned for now to get fast-paced response from Contes HW first
	 *

	if(request.at(1) != 03) return false;

	// Address+Function code+2*N+CRC
	int answerSize = 1+1+2*(request.at(04)*256+request.at(05))+2;

	ApplicationDataUnitSerial *response = new ApplicationDataUnitSerial(answerSize);
*/
	response = new ApplicationDataUnitSerial;
	clearError();
	qDebug() << "Written: " << write(request);
	qDebug() <<	"error() = " << error();
	if(waitForReadyRead(1000)) {
		qDebug() << "DATA READY";
	}
	else {
		qDebug() << "Timeouted while waiting for response !";
	}
	return 0;
}


void ModbusSerialMaster::onReadyRead()
{
	qDebug() << "READY READ DETECTED";
}

/*

void UmbServer::adjustAnswer(UmbAnswer *answer, UmbRequest *request) {
	(void)request; // Request used in base class implementation
	UmbProcessorInterface::adjustAnswer(answer, request);
	connect(this, &QIODevice::readyRead, answer, &UmbAnswer::onReadyRead);
	connect(answer, &UmbAnswer::answerReady, this, &UmbServer::onAnswerReady);
}
*/
