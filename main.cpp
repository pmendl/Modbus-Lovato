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


#define STR(X) #X
#define XSTR(X) STR(X)


ProcessingManager *processingManager;
KeyboardScanner keyboardScanner;
CommandsProcessor commandsProcessor;
//NetworkSender sender(TEST_SERVER_HTTP);
NetworkSender sender;


// Early declaration of various signal-processing functions, defined below main()
void onKeypress(char c);
void onHTTPreply(QSharedPointer<QNetworkReply> reply);
void onCommandReceived(CommandDescriptor descriptor);
void defaultKeypressFunction(char c);


// Keypress distribution stuff
#warning Continue implementation of this concept
using keypressReceiverFn = std::function<void(char)>;
// keypressReceiverFn const defaultKeypressReceiver(userKeypressFunction);
keypressReceiverFn actualKeypressReceiver(defaultKeypressFunction);
char fKeypressEscapeStatus;
void fKeypressFunction(char c);
void testFileQueryActual();

// Test files selection stuff
QFileInfoList testFilesList;
int testFilesIndex;

// Used for .INI to bool transformations/checking
const QSet<QString> trueCandidates = {
	QStringLiteral("1"),
	QStringLiteral("true"),
	QStringLiteral("yes")
};


int main(int argc, char *argv[])
{
	qDebug() << "Modbus application initializing:";

	/// @warning The code assumes Linux OS to be used, as QSettings::setDefaultFormat(...INI...)
	/// does not behave properly - at least it reads no groups/values on construction.
	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

#ifdef NO_AUTOMATIC_PROCESSING
	#warning TESTING ONLY - managed by setting NO_AUTOMATIC_PROCESSING in Globals.h
	processingManager = new ProcessingManager(&a, true);
#else
	processingManager = new ProcessingManager(&a);
#endif

	QObject::connect(&keyboardScanner, &KeyboardScanner::KeyPressed, &a, &onKeypress);
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

	QSettings settings;
	QVariant var(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SERVER_HTTP_KEY)));
	if(var.isValid())
		sender.setDefaultSlotUrl(var.toString());
	qDebug() << "Test server: " << sender.defaultSlotUrl().url();
	qDebug() << "Test files path: " XSTR(TEST_COMMAND_FILES_PATH);

	if(trueCandidates.contains(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY)).toString().toLower()))
			processingManager->setSuppressPeriodicalRequesting(true);
	// Note can be set elswhere, not only in the line above - so needs to re-read via method
	if(processingManager->suppressPeriodicalRequesting())
#ifdef NO_AUTOMATIC_PROCESSING
		qDebug() << "\nPERIODICAL REQUESTING OF VALUES SUPPRESSED !"
					"\n(This is debugging tool hardcoded by #define NO_AUTOMATIC_PROCESSING in Globals.h"
					"\neventually doubled by " DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY " value in .INI file)";
#else
		qDebug() << "\nPERIODICAL REQUESTING OF VALUES SUPPRESSED !"
					"\n(This is debugging tool activated by " DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY "=true value in .INI file)";
#endif

	keyboardScanner.startTimer();


	qDebug() << "\nModbus application started...";
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
		QSharedPointer<LogMaintenanceLocker> lock(processingManager->logServer()->fileMaintenanceLocker());

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

		if((source.isEmpty() || target.isEmpty())) {
			qDebug() << "\tSource and/or target parameter missing or invalid:" << source << "->" << target << "\n\tAborting...";
			return;
		}

		QSharedPointer<LogMaintenanceLocker> lock(processingManager->logServer()->fileMaintenanceLocker());

		if(QFile::remove(temp))
			qDebug() << "\tRemoved temporary:" << temp;

		if(QFile::exists(target)) {
			if(QFile::rename(target, temp))
				qDebug() << "\tRenamed" << target << "->" << temp;
			else {
				qDebug() << "\tRenaming" << target << "->" << temp << "FAILED!\n\tAborting...";
				return;
			}
		}

		if(QFile::rename(source,target))
			qDebug() << "\tRenamed" << source << "->" << target;
		else {
			qDebug() << "\tRenaming" << source << "->" << target << "FAILED!\n\tAborting...";
			return;
		}

		if(QFile::remove(temp))
			qDebug() << "\tRemoved temporary:" << temp;
		else
			qDebug() << "\tFAILED temporary removal!!! File may remain on disk:" << temp;
	} else if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_DELETE_VALUE)) {
		// --- DELETE COMMAND ---
		QString source(processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))));

		if(source.isEmpty()) {
			qDebug() << "\tSource parameter missing or invalid:" << source << "\n\tAborting...";
			return;
		}

		QSharedPointer<LogMaintenanceLocker> lock(processingManager->logServer()->fileMaintenanceLocker());

		if(QFile::remove(source))
			qDebug() << "\tRemoved:" << source;
		else
			qDebug() << "\tRemoval FAILED !!! File may remain on disk:" << source;
	}
}

void onKeypress(char c) {
	actualKeypressReceiver(c);
}

void defaultKeypressFunction(char c)
{
	qDebug() << "\n--------------- KEYPRESS:" << static_cast<char>(QChar::toUpper(c)) << "---------------";

	switch (toupper(c)) {
	case 'Q':
		std::cout << "Quitting...\n";
		QCoreApplication::instance()->quit();
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

		CommandsList list(sender.defaultSlotUrl().url(), &buff);
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

		CommandsList list(sender.defaultSlotUrl().url(), &buff);
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
//		QString str, value;
		QString line;

		keyboardScanner.setMode(KeyboardScanner::echoMode);
		const QRegularExpression pattern("(.*)=(.*)");
		QRegularExpressionMatch match;
		forever {
			qDebug() << "Enter KEY=VALUE:";
			line=in.readLine();
//			line=keyboardScanner.waitLine();
			if(line.isEmpty())
			break;

			if((match=pattern.match(line)).hasMatch()) {

				settings.setValue(match.captured(1), match.captured(2));
				qDebug().noquote() << "\twritten: " << match.captured(1) << "=" << settings.value(match.captured(1));
			}
		}
//		keyboardScanner.setMode(true);
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

	case 'D': // Command Delete
	{
		QByteArray cmd("\n\
DELETE\n\
SOURCE=\"Test.log\"\n\
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


	case 'F': // Send demo response FILE
	{
		qDebug() << "--- F keypress processing inited ---";

		QDir dir(XSTR(TEST_COMMAND_FILES_PATH));
		qDebug() << "\t" << dir.entryList(QDir::Files, QDir::Name);
		testFilesList = dir.entryInfoList(QDir::Files, QDir::Name);
		testFilesIndex = 0;
//		qDebug() << "\t" << dir.entryInfoList();

		fKeypressEscapeStatus = 0;
		actualKeypressReceiver = fKeypressFunction;

		testFileQueryActual();


/*
		QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType));
		fragment->setParent(multipart);

		QHttpPart part;
		if(_postFileContent) {
			part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain; charset=utf-8"));
			part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QVariant(
							   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
							   .arg(POST_ELEMENT_LOG_FILE_NAME)
							   .arg(fragment->logfile()->fileName())
							   )
						   );
			part.setBodyDevice(fragment);
			multipart->append(part);
*/

//		keyboardScanner.setDetection(true);
	}
		break;


//----------------------------------

	}
}

void testFileQueryActual() {
	qDebug() << "\tSend test file:" << testFilesList.at(testFilesIndex).fileName() << "?";
}

void testFileSendActual() {
	QFile *file(new QFile(testFilesList.at(testFilesIndex).absoluteFilePath()));
	qDebug() << "\tSending:" << file->fileName();

	QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType));
	file->setParent(multipart);
	if(!file->open(QFile::ReadOnly)) {
		qDebug() << "Opening command file for send FAILED:" << testFilesList.at(testFilesIndex).absoluteFilePath();
	}

	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain; charset=utf-8"));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant(
					   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
					   .arg(TEST_FILE_ITEM_KEY)
					   .arg(file->fileName())
					   )
				   );
	part.setBodyDevice(file);
	multipart->append(part);

	sender.sendMultipart(multipart);
}

void fKeypressFunction(char c) {
//	qDebug() << "*** fKeypressFunction received:" << c << (int)c;

	switch(fKeypressEscapeStatus) {
	case 0:	// NO ESCAPE INITED
		switch(c) {
		case 27: // INITIAL ESC
			fKeypressEscapeStatus = 27;
			break;

		case '\n':
			testFileSendActual();

		case 'q':
		case 'Q':
			qDebug() << "Leaving keypress 'F'...";
			actualKeypressReceiver = defaultKeypressFunction;
			break;
		}
		break;

	case 27: // After initial ESC
		switch(c) {
		case '[':
			fKeypressEscapeStatus = '[';
			break;
		}
		break;

	case '[':
		fKeypressEscapeStatus=0;
		switch(c) {
		case 'A':
			qDebug() << "UP ARROW";
			if(++testFilesIndex == testFilesList.size())
				--testFilesIndex;
			testFileQueryActual();
			break;

		case 'B':
			qDebug() << "DOWN ARROW";
			if(testFilesIndex > 0)
				--testFilesIndex;
			testFileQueryActual();
			break;

		case 'C':
			qDebug() << "RIGHT ARROW";
			testFileSendActual();
			break;

		case 'D':
			qDebug() << "LEFT ARROW";
			break;

		case 'H':
			qDebug() << "HOME";
			break;

		case 'F':
			qDebug() << "END";
			break;
		}

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

