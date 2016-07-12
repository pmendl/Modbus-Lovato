#include <QCoreApplication>

#include <QDebug>
#include <QSettings>

#include <iostream>
#include <cctype>

#include "Network/NetworkResource.h"
#include "Console/KeyboardScanner.h"

#include "Modbus/DataUnits.h"
#include "Modbus/ModbusSerialMaster.h"

int main(int argc, char *argv[])
{
	ProtocolDataUnit u({1,2,3});

	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

	KeyboardScanner ks;
//	ModbusSerialMaster master("/dev/ttyRPC0", 0, 9600);
	ModbusSerialMaster master("/dev/ttyRPC0");
	QObject::connect(&ks, &KeyboardScanner::KeyPressed, &a, [&](char c){
		qDebug() << c;
		switch (toupper(c)) {
		case 'Q':
			std::cout << "Quitting...\n";
			ks.finish();
			break;

		case 'M':
		{
			ProtocolDataUnit pdu({0x03, 0x00, 0x01, 0x00, 0x2A, 0x95, 0xE6});
			ApplicationDataUnitSerial request(static_cast<quint8>(2), pdu);

//			qDebug() << request;
			master.process(request);
		}
			break;

		case 'W':
			QSettings settings(QSettings::SystemScope,"PMCS", "LovatoModbus");
			QTextStream in(stdin);
			QString key, value;

			ks.setDetection(false);
			in.flush();
			qDebug() << "Enter KEY=VALUE:";
			key=in.readLine();
			qDebug() << "\t--> " << key;
			ks.setDetection(true);
			break;
		}
		qDebug() << "";
	}, Qt::QueuedConnection);
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	std::cout << "Modbus application started...\n\n";
	int result = a.exec();
	std::cout << "Modbus application quited...\n";
	return result;
}
