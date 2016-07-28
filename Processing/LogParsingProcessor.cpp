#include "LogParsingProcessor.h"

#include <QDebug>
#include <QSettings>
#include <QDir>

#include "Globals.h"
#include "Processing/RequestManager.h"

LogParsingProcessor::LogParsingProcessor(QSettings *settings, QSharedPointer<LogServer> logServer) :
	_logServer(logServer)
{
	QDir testDir(settings->value(REQUEST_PARSING_LOG_PATH_KEY, REQUEST_PARSING_LOG_PATH_DEFAULT).toString());
	qDebug() << "\t\tLOG QDir.isAbsolute()=" << testDir.isAbsolute();
	qDebug() << "\t\tLOG QDir.path()=" << testDir.path();
	qDebug() << "\t\tLOG QDir.absolutePath()=" << testDir.absolutePath();


}

bool LogParsingProcessor::isValid() {
#warning TESTING ONLY
	return true;
}

void LogParsingProcessor::process(RequestManager *rm){
	_logServer->log("test.log",QStringLiteral("TEST: ")+rm->groupName());
}
