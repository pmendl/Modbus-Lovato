#include <QCoreApplication>

#include <QDebug>

#include <iostream>
#include <cctype>

#include "Network/NetworkResource.h"
#include "Console/KeyboardScanner.h"

#include "Modbus/DataUnits.h"

int main(int argc, char *argv[])
{
	ProtocolDataUnit u({1,2,3});

	QCoreApplication a(argc, argv);
	KeyboardScanner ks;
	QObject::connect(&ks, &KeyboardScanner::KeyPressed, [&ks, &a](char c){
		std::cout << c << "\n";
		switch (toupper(c)) {
		case 'Q':
			std::cout << "Quitting...\n";
			ks.finish();
			break;

		case 'M':
		{
			ProtocolDataUnit pdu({2,3});
			ApplicationDataUnitSerial adu(1, pdu), adu2({'a','b','c'});

			qDebug() << adu << adu2;
		}

		}
	});
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	std::cout << "Modbus started...\n";
	int result = a.exec();
	std::cout << "Modbus quited...\n";
	return result;
}
