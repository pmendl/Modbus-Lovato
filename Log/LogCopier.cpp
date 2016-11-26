#include "LogCopier.h"

#include <QMetaObject>
#include "Debug/DebugMacros.h"
#include <QFile>
#include <QRegularExpression>
#include <QBuffer>

#include "Globals.h"

LogCopier::LogCopier(QString sourceFile, QString targetFile,
		  QString group, QObject *parent) :
	LogCopier( sourceFile,  targetFile,  QDateTime(),  QDateTime(),  group,  parent)
{}


LogCopier::LogCopier(QString sourceFile, QString targetFile, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	QThread(parent),
	_targetFile(targetFile)


{
	if(!_targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		DP_CMD_LOG_COPIER_ERROR("LogCopier aborted as it could not open target file" \
				 << targetFile << "for write. ERROR");
		deleteLater();
	}

	start();

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(sourceFile)), true, from, to, group, 0, this));
	DP_CMD_LOG_COPIER("LogCopier "<< sourceFile << "->" << targetFile << "constructed.");
}

void LogCopier::processFragment(LogFragment *fragment) {
	if(!fragment || fragment->bytesAvailable() <= 0) {
		DP_CMD_LOG_READER_DETAILS("\tNo more fragments to process...");
		deleteLater();
		return;
	}
	DP_CMD_LOG_COPIER_DETAILS("\tStarting processing of new fragment...");
	connect(fragment, &LogFragment::fragmentReady, this, &LogCopier::onFragmentReady);
/*
	connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
		DP_CMD_LOG_COPIER_ERROR("LogCopier detected fragmentFailed on" << fragment);
		fragment->deleteLater();
		deleteLater();
	});
*/
	QMetaObject::invokeMethod(fragment, "fillFragment");
}

LogCopier::~LogCopier() {
	quit();
	wait();
	DP_CMD_LOG_COPIER("LogCopier destroyed. (target=" << _targetFile.fileName() << ")");
	if(_targetFile.isOpen())
		_targetFile.close();
}

void LogCopier::onFragmentReady(LogFragment *fragment)
{
	DP_EVENTS_START(onFragmentReady)
	DP_CMD_LOG_COPIER_DETAILS("LogCopier::onFragmentReady starts file writting...");
	if(fragment == 0) {
		DP_CMD_LOG_COPIER_DETAILS("LogCopier::onFragmentReady called with zero pointer! ERROR!");
		DP_EVENTS_END("LogCopier::onFragmentReady called with zero pointer! ERROR!");
		return;
	}

	if(!fragment->open(QIODevice::ReadOnly)) {
		DP_CMD_LOG_COPIER_DETAILS("LogCopier::onFragmentReady aborts as it can not open fragment for reading...");
		fragment->deleteLater();
		DP_EVENTS_END("LogCopier::onFragmentReady aborts as it can not open fragment for reading...");
		return;
	}

	while (fragment->bytesAvailable() > 0)
		_targetFile.write(fragment->read(LOG_MAX_BUFFER_SIZE));

	DP_CMD_LOG_COPIER_DETAILS("LogCopier finished file writting...");

	processFragment((fragment->nextFragment()));
	DP_EVENTS_END("")
}
