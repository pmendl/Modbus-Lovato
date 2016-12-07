#include <QCoreApplication>

#include <QStorageInfo>

#include <QSettings>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QBuffer>
#include <Globals.h>
#include <QDateTime>
#include <QNetworkReply>
#include <QSharedPointer>

#include <iostream>
#include <cctype>
#include <limits>
#include <unistd.h>


#include "Debug/DebugMacros.h"
#include "Globals.h"
#include HTTP_MULTI_PART_INCLUDE
#include "Console/KeyboardScanner.h"
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
#include "Commands/CommandLogFilter.h"
#include "Commands/CommandCopyFilter.h"
#include "Commands/CommandReplaceFilter.h"
#include "Commands/CommandDeleteFilter.h"
#include "Commands/CommandResetFilter.h"
#include "System/Reset.h"
#include "System/TrueCandidates.h"
#include "System/WatchdogController.h"
#include "System/SignalisationController.h"


#define STR(X) #X
#define XSTR(X) STR(X)


QSharedPointer<ProcessingManager> processingManager;
KeyboardScanner keyboardScanner;
NetworkSender sender;

CommandsProcessor commandsProcessor;

// Early declaration of various signal-processing functions, defined below main()
void onKeypress(char c);
void onHTTPreply(QNetworkReply *reply);
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

#warning Adapt later according to http://stackoverflow.com/questions/40652599/invalid-conversion-from-const-parameter-to-parameter-seems-to-be-nonsense
void printCommandReceived(CommandDescriptor &descriptor)
{
	DP_COMMAND("\n--------------- COMMAND:" << descriptor.value(QStringLiteral(COMMAND_NAME)).toUpper() << "---------------");
	DP_COMMAND("\tFull command descriptor:"<< descriptor);
}

// Has to be called (exactly) once, after processingManager
// and commandsProcessor got initialized.
void constructCommandFilters(QObject *parent)
{
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 new CommandLogFilter(processingManager->logServer(), parent,
										  printCommandReceived),
					 &CommandLogFilter::onCommandReceived);
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 new CommandCopyFilter(processingManager->logServer(), parent,
										   printCommandReceived),
					 &CommandCopyFilter::onCommandReceived);
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 new CommandReplaceFilter(processingManager->logServer(), parent,
											  printCommandReceived),
					 &CommandReplaceFilter::onCommandReceived);
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 new CommandDeleteFilter(processingManager->logServer(), parent,
											  printCommandReceived),
					 &CommandDeleteFilter::onCommandReceived);
	QObject::connect(&commandsProcessor, &CommandsProcessor::commandReceived,
					 new CommandResetFilter(parent,
											  printCommandReceived),
					 &CommandResetFilter::onCommandReceived);

}

int main(int argc, char *argv[])
{
	DP_INIT("Modbus application initializing:");

	/// @warning The code assumes Linux OS to be used, as QSettings::setDefaultFormat(...INI...)
	/// does not behave properly - at least it reads no groups/values on construction.
	QCoreApplication a(argc, argv);
	QCoreApplication::setOrganizationName("PMCS");
	QCoreApplication::setOrganizationDomain("mendl.info");
	QCoreApplication::setApplicationName("LovatoModbus");

	InitiateResetEventFilter resetFilter;
	qApp->installEventFilter(&resetFilter);

	if(Q_BYTE_ORDER == Q_BIG_ENDIAN)
		DP_INIT("\tHost uses big endianness.");
	else
		DP_INIT("\tHost uses little endianness.");

	if(std::numeric_limits<float>::is_iec559)
		DP_INIT("\tHost complies to IEEE 754.");
	else
		DP_INIT("\tHost does not comply to IEEE 754.");

	new WatchdogController(&a);
	new SignalisationController(&a);

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
	processingManager.reset(new ProcessingManager(true));
#else
	processingManager.reset(new ProcessingManager());
#endif
	qApp->installEventFilter(processingManager.data());

	constructCommandFilters(&a);

	QObject::connect(&keyboardScanner, &KeyboardScanner::KeyPressed, &a, &onKeypress);
	QObject::connect(NetworkSender::commandsDistributor(), &CommandsDistributor::commandReplyReceived,
					 &onHTTPreply);
	QObject::connect(NetworkSender::commandsDistributor(), &CommandsDistributor::commandReplyReceived,
					 &commandsProcessor, &CommandsProcessor::processHttpReply);

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

	case 'I': // Initiate reset
	{
		System::resetInitiate(System::normalReset, "Keypress \"I\"");
	}
		break;


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

