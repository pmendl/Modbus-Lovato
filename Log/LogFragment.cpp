#include "LogFragment.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>

#include "Globals.h"

LogFragment::LogFragment(QSharedPointer<QFile> logfile,
		QDateTime from, QDateTime to,
						 QString group, QObject *parent, QThread *workingThread) :
	LogFragment(logfile, QString(), from, to, group, parent, workingThread)
{}

LogFragment::LogFragment(QSharedPointer<QFile> logfile,
		QString id,
		QString group, QObject *parent, QThread *workingThread) :
	LogFragment(logfile, id, QDateTime(), QDateTime(), group, parent, workingThread)
{}

LogFragment::LogFragment(QSharedPointer<QFile> logfile, QString id,
						 QDateTime from, QDateTime to,
						 QString group, QObject *parent, QThread *workingThread) :
	QBuffer(parent),
	_logFile(logfile),
	_from(from),
	_to(to),
	_group(group),
	_id(id),
	_startIndex(0),
	_endIndex(0),
	_firstFound(-1),
	_lastFound(-1),
	_recordCnt(0),
	_lastFragment(false),
	_parentThread(thread()),
	_workingThread(workingThread)

{
	if(!_logFile->isReadable())
		_logFile->open(QIODevice::ReadOnly);


	if(!_logFile->isOpen()) {
		qDebug() << "LogFragment aborted as log file could not get opened...";
		emit fragmentFailed(this);
		return;
	}

	if(_workingThread != 0) {
		qDebug() << "LogFragment moving to thread" << _workingThread;
		moveToThread(_workingThread);
	}

//	fillFragment();
	QMetaObject::invokeMethod(this, "fillFragment");
}

void LogFragment::fillFragment(void)
{
	qDebug() << "LogFragment starts filling up..." << thread();

	const QRegularExpression recordRegexp(QStringLiteral("(.*?)\\|VALUES (.*?)\\|(.*)"));
	QString record;
	_logFile->seek(_startIndex);
	do {
		_endIndex = _logFile->pos();
//		qDebug() << "@@@> " << _logFragment.endIndex;

		record = _logFile->readLine(LOG_MAX_BUFFER_SIZE);
//		qDebug() << _logFile.pos() << record;
		QRegularExpressionMatch match(recordRegexp.match(record));
//		qDebug() << "Matches:" << match.capturedTexts();
		if(match.hasMatch()) {
//***/			qDebug() << match.captured(1) << match.captured(2);
			if(_from.isValid() && (QDateTime::fromString(match.captured(1)) < _from))
				continue;
			if(_to.isValid() && (QDateTime::fromString(match.captured(1)) > _to))
				continue;
			if((!_group.isEmpty()) &&
					(!QRegularExpression(_group).match(match.captured(2)).hasMatch())
					)
				continue;

			if((size() + record.size()) < LOG_MAX_BUFFER_SIZE) {
//				qDebug() << record;
				buffer().append(record);
				++_recordCnt;
				if(_firstFound < 0)
					_firstFound = _endIndex;
				if(_lastFound < 0)
					_lastFound = _endIndex + record.size();

				qDebug() << "\tLogFragment recordCnt=" << _recordCnt << ", size=" << size();
				continue;
			}
			else {
				qDebug() << "LogFragment completed partial fill up. Moving back to thread" << _parentThread;
				moveToThread(_parentThread);
				emit fragmentReady(this);
				return;
			}
		}

	} while (!record.isEmpty());
	_lastFragment = true;
	qDebug() << "LogFragment finished filling up. Moving back to thread" << _parentThread;
	moveToThread(_parentThread);
	emit fragmentReady(this);
	return;
}

LogFragment *LogFragment::nextFragment(QThread *workingThread) {
	if(_lastFragment)
		return 0;

	LogFragment *fragment(
				new LogFragment(_logFile,
								_id,
								_from,
								_to,
								_group,
								parent(),
								((workingThread != 0) ? workingThread : _workingThread)
								)
				);
	fragment->_endIndex = fragment->_startIndex = _endIndex;
	return fragment;
}

qint64 LogFragment::recordCnt() const
{
	return _recordCnt;
}

qint64 LogFragment::lastFound() const
{
	return _lastFound;
}

qint64 LogFragment::firstFound() const
{
	return _firstFound;
}

QSharedPointer<QFile> LogFragment::logfile() const
{
	return _logFile;
}

QDateTime LogFragment::from() const
{
	return _from;
}

QDateTime LogFragment::to() const
{
	return _to;
}

QString LogFragment::group() const
{
	return _group;
}

QString LogFragment::id() const
{
	return _id;
}

bool LogFragment::lastFragment() const
{
	return _lastFragment;
}

qint64 LogFragment::endIndex() const
{
	return _endIndex;
}

qint64 LogFragment::startIndex() const
{
	return _startIndex;
}
