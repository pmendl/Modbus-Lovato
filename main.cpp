#include <QCoreApplication>

#include <QDebug>

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
	KeyboardScanner ks;
	ModbusSerialMaster master("/dev/ttyRPC0");
	QObject::connect(&ks, &KeyboardScanner::KeyPressed, &a, [&](char c){
		std::cout << c << "\n";
		switch (toupper(c)) {
		case 'Q':
			std::cout << "Quitting...\n";
			ks.finish();
			break;

		case 'M':
		{
			ProtocolDataUnit pdu({2,3});
			ApplicationDataUnitSerial adu(static_cast<quint8>(1), pdu), adu2({'a','b','c'}),
					request({0x02, 0x03, 0x00, 0x01, 0x00, 0x2A, 0x95, 0xE6});

			qDebug() << adu << adu2;
			qDebug() << request;
			master.process(request);
		}

		}
	}, Qt::QueuedConnection);
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	std::cout << "Modbus started...\n";
	int result = a.exec();
	std::cout << "Modbus quited...\n";
	return result;
}
