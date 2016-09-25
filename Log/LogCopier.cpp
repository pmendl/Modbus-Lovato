#include "LogCopier.h"

#include <QDebug>
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
		qDebug() << "LogCopier aborted as it could not open target file"
				 << targetFile << "for write. ERROR";
		deleteLater();
	}

	start();

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(sourceFile)), true, from, to, group, 0, this));
	qDebug() << "LogCopier "<< sourceFile << "->" << targetFile << "constructed.";
}

void LogCopier::processFragment(LogFragment *fragment) {
	if(!fragment) {
		deleteLater();
		return;
	}
	qDebug() << "\tStarting processing of new fragment...";
	connect(fragment, &LogFragment::fragmentReady, this, &LogCopier::onFragmentReady);
	connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
		qDebug() << "LogCopier detected fragmentFailed on" << fragment;
		deleteLater();
	});
//	fragment->fillFragment();
	QMetaObject::invokeMethod(fragment, "fillFragment");

}

LogCopier::~LogCopier() {
	quit();
	wait();
	if(_targetFile.isOpen())
		_targetFile.close();
	qDebug() << "LogCopier destroyed.";
}

void LogCopier::onFragmentReady(LogFragment *fragment)
{

	qDebug() << "LogCopier::onFragmentReady starts file writting...";
	if(fragment == 0) {
		qDebug() << "LogCopier::onFragmentReady called with zero pointer! ERROR!";
		return;
	}

	if(!fragment->open(QIODevice::ReadOnly)) {
		qDebug() << "LogCopier::onFragmentReady aborts as it can not open fragment for reading...";
		fragment->deleteLater();
		return;
	}

	while (fragment->bytesAvailable() > 0)
		_targetFile.write(fragment->read(LOG_MAX_BUFFER_SIZE));

	qDebug() << "LogCopier finished file writting...";

	processFragment((fragment->nextFragment()));
}
