#include "LogFragment.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>

#include "Globals.h"

LogFragment::LogFragment(QSharedPointer<QFile> logfile,
		QDateTime from, QDateTime to,
						 QString group, QObject *parent) :
	LogFragment(logfile, QString(), from, to, group, parent)
{}

LogFragment::LogFragment(QSharedPointer<QFile> logfile,
		QString id,
		QString group, QObject *parent) :
	LogFragment(logfile, id, QDateTime(), QDateTime(), group, parent)
{}

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

LogFragment::LogFragment(QSharedPointer<QFile> logfile, QString id,
						 QDateTime from, QDateTime to,
						 QString group, QObject *parent) :
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
	_lastFragment(false)

{
	if(!_logFile->isReadable())
		_logFile->open(QIODevice::ReadOnly);
}
/*
_logFile.open(QIODevice::ReadOnly);
qDebug() << "LogReader" << pathname << "isOpen() = " << _logFile.isOpen();


	_logFile.close();
	qDebug() << "LogReader destroyed.";

*/

void LogFragment::fillFragment(void)
{
	qDebug() << "LogFragment starts filling up...";
	if(!_logFile->isOpen()) {
		qDebug() << "LogFragment aborted as log file could not get opened...";
		emit fragmentFailed(this);
		return;
	}

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
				qDebug() << "\tLogFragment recordCnt=" << _recordCnt << ", size=" << size();
				continue;
			}
			else {
				qDebug() << "LogFragment completed partial fill up ...";
				emit fragmentReady(this);
				return;
			}
		}

	} while (!record.isEmpty());
	_lastFragment = true;
	emit fragmentReady(this);
	qDebug() << "LogFragment finished filling up...";
	return;
}

/*
QBuffer *LogFragment::pullBuffer() {
	if(lastFragment)
		return 0;

	QBuffer *result = new QBuffer(&buffer());
	startIndex=endIndex;
	buffer().clear();
	return result;
}
*/
LogFragment *LogFragment::nextFragment() {
	if(_lastFragment)
		return 0;

	LogFragment *fragment( new LogFragment(_logFile,
						  _id,
						  _from,
						  _to,
						  _group,
						  this
						  ));
	fragment->_endIndex = fragment->_startIndex = _endIndex;
	return fragment;
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

