#include <QCoreApplication>

#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QDateTime>
#include <QNetworkReply>

#include <iostream>
#include <cctype>
#include <limits>
#include <unistd.h>

#include "Console/KeyboardScanner.h"

#include "Globals.h"
#include "Modbus/DataUnits.h"
#include "Modbus/ModbusSerialMaster.h"
#include "Processing/ProcessingManager.h"
#include "Processing/RequestManager.h"
#include "Network/NetworkAccessBase.h"
#include "Network/NetworkSender.h"
#include "Log/LogReader.h"

void postFile(void) {
	QBuffer *demoFile = new QBuffer();
	demoFile->setData(QStringLiteral("Testovací log:\nŘádek 1\nŘádek 2\n").toUtf8());
	demoFile->open(QIODevice::ReadOnly);

	QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain"));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant("form-data; name=\"logFile\"; filename=\"Test.log\""));
//	part.setRawHeader("Expires", QDateTime::currentDateTimeUtc().toString().toUtf8());
	part.setBodyDevice(demoFile);
	multipart->append(part);

/* OLD VERSION
	QNetworkReply *reply = NetworkAccessBase::networkAccessManager()->
						   post(QNetworkRequest(QUrl("http://www.contes.cz/mendl/import.php")), multipart);
*/
// NEW VERSION
	NetworkSender *ns(new NetworkSender("http://www.contes.cz/mendl/import.php", multipart, true));
	QNetworkReply *reply = ns->reply().data();
/**/
	reply->setParent(QCoreApplication::instance());
	multipart->setParent(reply);
	demoFile->setParent(reply);

	QObject::connect(reply, &QNetworkReply::finished, [reply](){
		qDebug() << "POST finished with result" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		qDebug() << "HEADERS:";
		foreach (QNetworkReply::RawHeaderPair header, reply->rawHeaderPairs()) {
			qDebug() << "\t" << header.first << "=" << header.second;
		}
//		qDebug() << "DATA:\n" << reply->readAll();
		qDebug() << "DATA SIZE:" << reply->bytesAvailable();
		reply->deleteLater();
	});

	qDebug() << "Leaving postFile()...";
}

int main(int argc, char *argv[])
{
//	ProtocolDataUnit u({1,2,3});

	/// @warning The code assumes Linux OS to be used, as QSettings::setDefaultFormat(...INI...)
	/// does not behave properly - at least it reads no groups/values on construction.
	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

	// Make relative paths start in the application folder
//	QDir::setCurrent(QCoreApplication::applicationDirPath());

	std::cout << "\nConstructing ProcessingManager object...\n";
	ProcessingManager processingManager;

	KeyboardScanner ks;
	QObject::connect(&ks, &KeyboardScanner::KeyPressed, &a, [&](char c){
		qDebug() << c;
		switch (toupper(c)) {
		case 'Q':
			std::cout << "Quitting...\n";
			ks.finish();
			break;

		case 'P': // Post file test
			postFile();
			break;

		case 'L': // Log reader test
		{
			LogReader *lr(new LogReader("http://www.contes.cz/mendl/import.php",
										processingManager.logServer()->pathname("Common.log"),
										QDateTime::fromString("Sun Jul 31 12:00:00 2016 GMT"),
//										QDateTime::fromString(""),
										QDateTime::fromString("Sun Jul 31 15:30:00 2016 GMT"),
										"Gr.*_.?"
										)
						  );
			lr->start();

			qDebug() << "Leaving 'L' command...";
			break;
		}

		case 'E': // Test event loop
		{
			qDebug() << "Entering QEventLoop test...";
//*
			QEventLoop loop;
			QObject::connect(&ks, &KeyboardScanner::KeyPressed, &loop, &QEventLoop::quit);
			loop.exec();
//*/
/*
			NetworkSender sender(":::", QSharedPointer<QHttpMultiPart>());
			sender.test();
*/
			qDebug() << "Exiting QEventLoop test...";
			break;
		}

		/* IMPORTANT !!!
		 *
		 * As following command empirically confirmed, root of system settings of Qt
		 * on Raspberry Pi lies NOT on / but rather on /usr/local/Qt-5.5.1-raspberry
		 * i.e. master Modbus cfg file is the
		 * /usr/local/Qt-5.5.1-raspberry/etc/xdg/PMCS/LovatoModbus.conf
		 */

		case 'W': // Write INI
		{
//			QSettings settings(QSettings::SystemScope,"PMCS", "LovatoModbus");
			QSettings settings;
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

		case 'R': // Read INI
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

		case 'T': // Test various settings
		{

			QSettings settings;

			settings.setValue("Test/key","test1");
			settings.setValue("Test/arrayFromKey/size","2");
			settings.setValue("Test/arrayFromKey/1/item","item");
			settings.setValue("Test/arrayFromKey/2/subArray/size","1");
			settings.setValue("Test/arrayFromKey/2/subArray/1/item","subitem");
			settings.setValue("Test2/path/key","test2");

//			qDebug() << "\nallKeys =" << settings.allKeys();

			settings.beginGroup("Test");
			qDebug() << "\nTest/key=" << settings.value("key").toString();
			settings.beginReadArray("arrayFromKey");
			settings.setArrayIndex(2);
			foreach(QString key, settings.childKeys()) {
				qDebug() << key << "=" << settings.value(key).toString();
			}

			settings.beginReadArray("subArray");
			qDebug() << "\nsubArray:";
			foreach(QString key, settings.childKeys()) {
				qDebug() << key << "=" << settings.value(key).toString();
			}
		}


		// Check the ~/.config/PMCS/LovatoModbus.conf generated by this command
		case 'S': // Settings
		{
			QSettings settings;
			settings.beginGroup("Test");
			settings.setValue("true",true);
			settings.setValue("false",false);
			settings.endGroup();
		}
			break;

		case 'G': // Groups
		{
			QSettings active, group;
			active.beginGroup("Active");
			for(QString s : active.childKeys()) {
				qDebug() << "Group:" << s;
				group.beginGroup(s);
				qDebug() <<  group.childKeys();
				qDebug() <<  group.childGroups();
				group.endGroup();
			}
			active.endGroup();

		}
			break;


//----------------------------------

		qDebug() << "\nCOMMAND:";
		}

	}, Qt::QueuedConnection);
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	if(Q_BYTE_ORDER == Q_BIG_ENDIAN)
		qDebug() << "Host uses big endianness.";
	else
		qDebug() << "Host uses little endianness.";

	if(std::numeric_limits<float>::is_iec559)
		qDebug() << "Host complies to IEEE 754.";
	else
		qDebug() << "Host does not comply to IEEE 754.";

	std::cout << "\nModbus application started...\n\n";
	int result = a.exec();
	std::cout << "Modbus application quited...\n";
	return result;
}
