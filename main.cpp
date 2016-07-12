#include <QCoreApplication>

#include <QDebug>
#include <QSettings>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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

		/* IMPORTANT !!!
		 *
		 * As following command empirically confirmed, root of system settings of Qt
		 * on Raspberry Pi lies NOT on / but rather on /usr/local/Qt-5.5.1-raspberry
		 * i.e. master Modbus cfg file is the
		 * /usr/local/Qt-5.5.1-raspberry/etc/xdg/PMCS/LovatoModbus.conf
		 */

		case 'W':
		{
			QSettings settings(QSettings::SystemScope,"PMCS", "LovatoModbus");
			QTextStream in(stdin);
			QString str, value;

			ks.setDetection(false);
			const QRegularExpression pattern("(.*)=(.*)");
			QRegularExpressionMatch match;
			forever {
				qDebug() << "Enter KEY=VALUE:";
				str=in.readLine();
				if(str.isEmpty())
				break;

				if((match=pattern.match(str)).hasMatch()) {

					settings.setValue(match.captured(1), match.captured(2));
					qDebug().noquote() << "\twritten: " << match.captured(1) << "=" << settings.value(match.captured(1));
				}
			}
			ks.setDetection(true);
			break;
		}

		case 'R':
		{
			QSettings settings;
			QTextStream in(stdin);
			QString str;

			ks.setDetection(false);
				qDebug() << "Enter KEY:";
				str=in.readLine();
				qDebug().noquote() << "\tread: " << str << "=" << settings.value(str);
			ks.setDetection(true);
			break;
		}

		qDebug() << "\nCOMMAND:";
		}

	}, Qt::QueuedConnection);
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	std::cout << "Modbus application started...\n\n";
	int result = a.exec();
	std::cout << "Modbus application quited...\n";
	return result;
}
