#include <QCoreApplication>

#include <QStorageInfo>

#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QDateTime>
#include <QNetworkReply>
#include <QSharedPointer>

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
#include "Log/LogCopier.h"
#include "Commands/CommandsProcessor.h"
#include "Commands/CommandsList.h"

ProcessingManager *processingManager;
KeyboardScanner keyboardScanner;
CommandsProcessor commandsProcessor;

// Early declaration of various signal-processing functions, defined below main()
void onKeypress(char c);
void onHTTPreply(QSharedPointer<QNetworkReply> reply);
void onCommandReceived(CommandDescriptor descriptor);

int main(int argc, char *argv[])
{
	qDebug() << "Modbus application initializing:";

	/// @warning The code assumes Linux OS to be used, as QSettings::setDefaultFormat(...INI...)
	/// does not behave properly - at least it reads no groups/values on construction.
	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

	processingManager = new ProcessingManager(&a);

	QObject::connect(&keyboardScanner, &KeyboardScanner::KeyPressed, &a, &onKeypress);
	QObject::connect(&keyboardScanner, &KeyboardScanner::finished, &a, &QCoreApplication::quit);
	QObject::connect(NetworkSender::commandsDistributor(), &CommandsDistributor::commandReplyReceived,
					 &onHTTPreply);
	QObject::connect(NetworkSender::commandsDistributor(), &CommandsDistributor::commandReplyReceived,
					 &commandsProcessor, &CommandsProcessor::processHttpReply);
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 &onCommandReceived);

	if(Q_BYTE_ORDER == Q_BIG_ENDIAN)
		qDebug() << "\tHost uses big endianness.";
	else
		qDebug() << "\tHost uses little endianness.";

	if(std::numeric_limits<float>::is_iec559)
		qDebug() << "\tHost complies to IEEE 754.";
	else
		qDebug() << "\tHost does not comply to IEEE 754.";

	qDebug() << "\nMounted filesystems:";
	foreach (QStorageInfo storageInfo, QStorageInfo::mountedVolumes()) {
		qDebug() << "\tStorage" << storageInfo.displayName() << "(" << storageInfo.fileSystemType() << ")";
		qDebug() << "\tFree space: " << storageInfo.bytesAvailable() << "bytes\n";
	}


	keyboardScanner.start();
	qDebug() << "Modbus application started...";
	int result = a.exec();
	qDebug() << "Modbus application quited...\n";
	return result;
}


void onHTTPreply(QSharedPointer<QNetworkReply> reply) {
	qDebug() << "HTTP response received" << reply.data();
	if(reply->error() != 0)
		qDebug() << "\tURL" << reply->url() << "SENDING ERROR: " << reply->errorString();
   else {
		qDebug() << "\tURL" << reply->url() << "SENT WITH RESULT" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
/// @todo Consider more sophisticated processing above - even signalling uplink
		qDebug() << "\tNetworkSender HEADERS:";
		foreach (QNetworkReply::RawHeaderPair header, reply->rawHeaderPairs()) {
			qDebug() << "\t\t" << header.first << "=" << header.second;
		}
//		 qDebug() << "\tDATA:\n" << reply->readAll();
		qDebug() << "\tDATA SIZE:" << reply->bytesAvailable();

		qDebug() << "End of HTTP response data";

	}

}

void onCommandReceived(CommandDescriptor descriptor) {
	qDebug() << "\n--------------- COMMAND:" << descriptor.value(QStringLiteral(COMMAND_NAME)).toUpper() << "---------------";

	qDebug() << "\tFull command descriptor:"<< descriptor;

	if(descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_LOG_VALUE)) {
		// --- LOG COMMAND ---
		bool postFileContent(false);
		const QSet<QString> trueCandidates = {
			QStringLiteral("1"),
			QStringLiteral("true"),
			QStringLiteral("yes")
		};
		if(trueCandidates.contains(descriptor.value(QStringLiteral(COMMAND_LOG_PARAMETER_POSTCONTENT_NAME)).toLower())) {
				postFileContent=true;
		}

		new LogReader(descriptor.originatorUrl,
					  processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))),
					  postFileContent,
					  descriptor.value(QStringLiteral(COMMAND_LOG_PARAMETER_ID_NAME)),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_FROM_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TO_NAME))),
					  descriptor.value(QStringLiteral(COMMAND_PARAMETER_GROUP_NAME))
					  );
	} else if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_COPY_VALUE)) {
		// --- COPY COMMAND ---
		new LogCopier(processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))),
					  processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TARGET_FILE_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_FROM_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TO_NAME))),
					  descriptor.value(QStringLiteral(COMMAND_PARAMETER_GROUP_NAME))
					  );
	} else if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_REPLACE_VALUE)) {
		// --- REPLACE COMMAND ---
		QString source(processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME)))),
				target(processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TARGET_FILE_NAME)))),
				temp(target.section(QChar('.'),0,-2)+QStringLiteral(".old"));

#warning Log locking must get here !!!

		if(QFile::remove(temp))
			qDebug() << "\tRemoved temporary:" << temp;

		if(QFile::exists(target)) {
			if(QFile::rename(target, temp))
				qDebug() << "\tRenamed" << target << "->" << temp;
			else {
				qDebug() << "\tRenaming" << target << "->" << temp << "FAILED!\t\nAborting...";
				return;
			}
		}

		if(QFile::rename(source,target))
			qDebug() << "\tRenamed" << source << "->" << target;
		else {
			qDebug() << "\tRenaming" << source << "->" << target << "FAILED!\t\nAborting...";
			return;
		}

		if(QFile::remove(temp))
			qDebug() << "\tRemoved temporary:" << temp;
		else
			qDebug() << "\tFAILED temporary removal!!! File remains on disk:" << temp;
	}
}

void onKeypress(char c)
{
	qDebug() << "\n--------------- KEYPRESS:" << c << "---------------";
	switch (toupper(c)) {
	case 'Q':
		std::cout << "Quitting...\n";
		keyboardScanner.finish();
		break;

	case 'L': // Log reader test
	{
		QByteArray cmd("\n\
					   Log\n\
					   SOURCE=\"Common.log\"\n\
					   POSTCONTENT=true\n\
					   ID=Keypress L\n\
					   FROM=\"Sun Jul 31 12:00:00 2016 GMT\"\n\
					   TO=\"Sun Jul 31 15:30:00 2016 GMT\"\n\
					   GROUP=\"Gr.*_.?\"\n\
					   ");

		QBuffer buff(&cmd);
		if(!buff.open(QIODevice::ReadOnly)) {
			qDebug() << "Error opening buffer!";
			break;
		}

		CommandsList list("http://mirtes.wz.cz/import.php", &buff);
		commandsProcessor.processCommandsList(&list);

		qDebug() << "Leaving 'L' command...";
		break;
	}

	case 'H': // Log reader "headers" test
	{
		QByteArray cmd("\n\
					   Log\n\
					   SOURCE=\"Common.log\"\n\
"/*					   POSTCONTENT=true\n\
*/"					   ID=Keypress H\n\
					   FROM=\"Sun Jul 31 12:00:00 2016 GMT\"\n\
					   TO=\"Sun Jul 31 15:30:00 2016 GMT\"\n\
					   GROUP=\"Gr.*_.?\"\n\
					   ");

		QBuffer buff(&cmd);
		if(!buff.open(QIODevice::ReadOnly)) {
			qDebug() << "Error opening buffer!";
			break;
		}

		CommandsList list("http://mirtes.wz.cz/import.php", &buff);
		commandsProcessor.processCommandsList(&list);

		qDebug() << "Leaving 'H' command...";
		break;
	}

	case 'E': // Test event loop
	{
		qDebug() << "Entering QEventLoop test...";
		QEventLoop loop;
		QObject::connect(&keyboardScanner, &KeyboardScanner::KeyPressed, &loop, &QEventLoop::quit);
		loop.exec();
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
		QSettings settings;
		QTextStream in(stdin);
		QString str, value;

		keyboardScanner.setDetection(false);
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
		keyboardScanner.setDetection(true);
		break;
	}


	case 'R': // Command Replace
	{
		QByteArray cmd("\n\
REPLACE\n\
SOURCE=\"Test.log\"\n\
TARGET=\"Common.log\"\n\
		");

		QBuffer buff(&cmd);
		if(!buff.open(QIODevice::ReadOnly)) {
			qDebug() << "Error opening buffer!";
			break;
		}

		CommandsList list(&buff);
		commandsProcessor.processCommandsList(&list);
		break;
	}

/*
	case 'R': // Read INI
	{
		QSettings settings;
		QTextStream in(stdin);
		QString str;

		keyboardScanner.setDetection(false);
			qDebug() << "Enter KEY:";
			str=in.readLine();
			qDebug().noquote() << "\tread: " << str << "=" << settings.value(str);
		keyboardScanner.setDetection(true);
		break;
	}
*/

	case 'C': //Log copier test
	{
		QByteArray cmd("\n\
COPY\n\
SOURCE=\"Common.log\"\n\
TARGET=\"CopyOfCommon.log\"\n\
FROM=\"Sun Jul 31 12:00:00 2016 GMT\"\n\
TO=\"Sun Jul 31 15:30:00 2016 GMT\"\n\
GROUP=\"Gr.*_.?\"\n\
		");

		QBuffer buff(&cmd);
		if(!buff.open(QIODevice::ReadOnly)) {
			qDebug() << "Error opening buffer!";
			break;
		}

		CommandsList list(&buff);
		commandsProcessor.processCommandsList(&list);
		break;
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

	}
}

// Test commands processing
/*
Log\n\
param1=A alpha\n\
;comment for parameter 2\n\
param2=B\n\
command=hackery\n\
\n\
;general comment\n\
copY\n\
wrong=parameter=with=equals\n\
right=\"Quoted parameter\"\n\
");
*/

