#include <QCoreApplication>

#include <QStorageInfo>

#include "Debug/DebugMacros.h"
#include <QSettings>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QBuffer>
#include <Globals.h>
#include HTTP_MULTI_PART_INCLUDE
#include <QDateTime>
#include <QNetworkReply>
#include <QSharedPointer>

#include <iostream>
#include <cctype>
#include <limits>
#include <unistd.h>

#include "Console/KeyboardScanner.h"

#include "Globals.h"
#include HTTP_MULTI_PART_INCLUDE
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
#include "Debug/MessageHandler.h"


#define STR(X) #X
#define XSTR(X) STR(X)


ProcessingManager *processingManager;
KeyboardScanner keyboardScanner;
CommandsProcessor commandsProcessor;
//NetworkSender sender(TEST_SERVER_HTTP);
NetworkSender sender;


// Early declaration of various signal-processing functions, defined below main()
void onKeypress(char c);
void onHTTPreply(QNetworkReply *reply);
void onCommandReceived(CommandDescriptor descriptor);
void defaultKeypressFunction(char c);


// Keypress distribution stuff
using keypressReceiverFn = std::function<void(char)>;
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
	DP_INIT("Modbus application initializing:");

#ifdef USE_MESSAGE_HANDLER
	qInstallMessageHandler(Debug::myMessageOutput);
#endif

	/// @warning The code assumes Linux OS to be used, as QSettings::setDefaultFormat(...INI...)
	/// does not behave properly - at least it reads no groups/values on construction.
	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

	if(Q_BYTE_ORDER == Q_BIG_ENDIAN)
		DP_INIT("\tHost uses big endianness.");
	else
		DP_INIT("\tHost uses little endianness.");

	if(std::numeric_limits<float>::is_iec559)
		DP_INIT("\tHost complies to IEEE 754.");
	else
		DP_INIT("\tHost does not comply to IEEE 754.");

	DP_INIT("\nMounted filesystems:");
	foreach (QStorageInfo storageInfo, QStorageInfo::mountedVolumes()) {
		DP_INIT("\tStorage" << storageInfo.displayName() << "(" << storageInfo.fileSystemType() << ")");
		DP_INIT("\tFree space: " << storageInfo.bytesAvailable() << "bytes\n");
	}

	QSettings settings;
	QVariant var(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SERVER_HTTP_KEY)));
	if(var.isValid())
		sender.setDefaultSlotUrl(var.toString());
	DP_INIT("Test server: " << sender.defaultSlotUrl().url());
	DP_INIT("Test files path: " XSTR(TEST_COMMAND_FILES_PATH));

	NetworkAccessBase::readPanicConnections();
	DP_INIT("------------------------------");
	DP_PROCESSING_INIT("Processing manager initializing:");

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

	DP_PROCESSING_INIT("------------------------------");



	if(trueCandidates.contains(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY)).toString().toLower()))
			processingManager->setSuppressPeriodicalRequesting(true);
	// Note can be set elswhere, not only in the line above - so needs to re-read via method
	if(processingManager->suppressPeriodicalRequesting())
#ifdef NO_AUTOMATIC_PROCESSING
		D_P("\nPERIODICAL REQUESTING OF VALUES SUPPRESSED !" \
			"\n(This is debugging tool hardcoded by #define NO_AUTOMATIC_PROCESSING in Globals.h" \
			"\neventually doubled by " DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY " value in .INI file)");
#else
		D_P("\nPERIODICAL REQUESTING OF VALUES SUPPRESSED !"
					"\n(This is debugging tool activated by " DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY "=true value in .INI file)");
#endif

	keyboardScanner.startTimer();


	D_P("Modbus application started...");
	int result = a.exec();
	D_P("Modbus application quited...\n");
	return result;
}


void onHTTPreply(QNetworkReply *reply) {
	DP_NET_HTTP_REPLY("HTTP response received" << reply);
	if(reply->error() != 0)
		DP_NET_HTTP_REPLY_DETAILS("\tURL" << reply->url() << "SENDING ERROR: " << reply->errorString());
   else {
		DP_NET_HTTP_REPLY_DETAILS("\tURL" << reply->url() << "SENT WITH RESULT" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
/// @todo Consider more sophisticated processing above - even signalling uplink
		DP_NET_HTTP_REPLY_DETAILS("\tNetworkSender HEADERS:");
		foreach (QNetworkReply::RawHeaderPair header, reply->rawHeaderPairs()) {
			DP_NET_HTTP_REPLY_DETAILS("\t\t" << header.first << "=" << header.second);
		}
//		 DP_NET_HTTP_REPLY_DETAILS("\tDATA:\n" << reply->readAll());
		DP_NET_HTTP_REPLY_DETAILS("\tDATA SIZE:" << reply->bytesAvailable());

		DP_NET_HTTP_REPLY_DETAILS("End of HTTP response data");

	}

}

void onCommandReceived(CommandDescriptor descriptor) {
	D_P("\n--------------- COMMAND:" << descriptor.value(QStringLiteral(COMMAND_NAME)).toUpper() << "---------------");

	D_P("\tFull command descriptor:"<< descriptor);

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
			D_P("\tSource and/or target parameter missing or invalid:" << source << "->" << target << "\n\tAborting...");
			return;
		}

		QSharedPointer<LogMaintenanceLocker> lock(processingManager->logServer()->fileMaintenanceLocker());

		if(QFile::remove(temp))
			D_P("\tRemoved temporary:" << temp);

		if(QFile::exists(target)) {
			if(QFile::rename(target, temp)) {
				D_P("\tRenamed" << target << "->" << temp);
			}
			else {
				D_P("\tRenaming" << target << "->" << temp << "FAILED!\n\tAborting...");
				return;
			}
		}

		if(QFile::rename(source,target)) {
			D_P("\tRenamed" << source << "->" << target);
		}
		else {
			D_P("\tRenaming" << source << "->" << target << "FAILED!\n\tAborting...");
			return;
		}

		if(QFile::remove(temp)) {
			D_P("\tRemoved temporary:" << temp);
		}
		else {
			D_P("\tFAILED temporary removal!!! File may remain on disk:" << temp);
		}
	} else if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_DELETE_VALUE)) {
		// --- DELETE COMMAND ---
		QString source(processingManager->logServer()->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))));

		if(source.isEmpty()) {
			D_P("\tSource parameter missing or invalid:" << source << "\n\tAborting...");
			return;
		}

		QSharedPointer<LogMaintenanceLocker> lock(processingManager->logServer()->fileMaintenanceLocker());

		if(QFile::remove(source)) {
			D_P("\tRemoved:" << source);
		}
		else {
			D_P("\tRemoval FAILED !!! File may remain on disk:" << source);
		}
	}
}

void onKeypress(char c) {
	actualKeypressReceiver(c);
}

void defaultKeypressFunction(char c)
{
	D_P("\n--------------- KEYPRESS:" << static_cast<char>(QChar::toUpper(c)) << "---------------");

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
			D_P("Error opening buffer!");
			break;
		}

		CommandsList list(sender.defaultSlotUrl().url(), &buff);
		commandsProcessor.processCommandsList(&list);

		D_P("Leaving 'L' command...");
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
			D_P("Error opening buffer!");
			break;
		}

		CommandsList list(sender.defaultSlotUrl().url(), &buff);
		commandsProcessor.processCommandsList(&list);

		D_P("Leaving 'H' command...");
		break;
	}

	case 'E': // Test event loop
	{
		D_P("Entering QEventLoop test...");
		QEventLoop loop;
		QObject::connect(&keyboardScanner, &KeyboardScanner::KeyPressed, &loop, &QEventLoop::quit);
		loop.exec();
		D_P("Exiting QEventLoop test...");
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
			D_P("Enter KEY=VALUE:");
			line=in.readLine();
//			line=keyboardScanner.waitLine();
			if(line.isEmpty())
			break;

			if((match=pattern.match(line)).hasMatch()) {

				settings.setValue(match.captured(1), match.captured(2));
				DEBUG(qDebug().noquote() << "\twritten: " << match.captured(1) << "=" << settings.value(match.captured(1));)
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
			D_P("Error opening buffer!");
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
			D_P("Enter KEY:");
			str=in.readLine();
			D_P("\tread: " << str << "=" << settings.value(str);)
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
			D_P("Error opening buffer!");
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
			D_P("Error opening buffer!");
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
			D_P("Group:" << s);
			group.beginGroup(s);
			D_P(group.childKeys());
			D_P(group.childGroups());
			group.endGroup();
		}
		active.endGroup();

	}
		break;


	case 'F': // Send demo response FILE
	{
		D_P("--- F keypress processing inited ---");

		QDir dir(XSTR(TEST_COMMAND_FILES_PATH));
		D_P("\t" << dir.entryList(QDir::Files, QDir::Name));
		testFilesList = dir.entryInfoList(QDir::Files, QDir::Name);
		testFilesIndex = 0;
//		D_P("\t" << dir.entryInfoList());

		fKeypressEscapeStatus = 0;
		actualKeypressReceiver = fKeypressFunction;

		testFileQueryActual();
	}
		break;


//----------------------------------

	}
}

void testFileQueryActual() {
	D_P("\tSend test file:" << testFilesList.at(testFilesIndex).fileName() << "?");
}

void testFileSendActual() {
	QFile *file(new QFile(testFilesList.at(testFilesIndex).absoluteFilePath()));
	D_P("\tSending:" << file->fileName());

	HTTP_MULTI_PART_USED *multipart(new HTTP_MULTI_PART_USED(QHttpMultiPart::FormDataType));
	file->setParent(multipart);
	if(!file->open(QFile::ReadOnly)) {
		D_P("Opening command file for send FAILED:" << testFilesList.at(testFilesIndex).absoluteFilePath());
	}

	multipart->appendFile("experiment",file);

	sender.sendMultipart(multipart);
}

void fKeypressFunction(char c) {
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
			D_P("Leaving keypress 'F'...");
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
			D_P("UP ARROW");
			if(++testFilesIndex == testFilesList.size())
				--testFilesIndex;
			testFileQueryActual();
			break;

		case 'B':
			D_P("DOWN ARROW");
			if(testFilesIndex > 0)
				--testFilesIndex;
			testFileQueryActual();
			break;

		case 'C':
			D_P("RIGHT ARROW");
			testFileSendActual();
			break;

		case 'D':
			D_P("LEFT ARROW");
			break;

		case 'H':
			D_P("HOME");
			break;

		case 'F':
			D_P("END");
			break;
		}

	}
}

