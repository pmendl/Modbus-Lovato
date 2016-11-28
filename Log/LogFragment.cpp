#include "LogFragment.h"

#include <QFile>
#include <QRegularExpression>

#include "Debug/DebugMacros.h"
#include "Globals.h"
#include "System/Reset.h"

LogFragment::LogFragment(QSharedPointer<QFile> logfile, bool postFileContent,
		QDateTime from, QDateTime to,
						 QString group, QObject *parent, QThread *workingThread) :
	LogFragment(logfile, postFileContent, QString(), from, to, group, parent, workingThread)
{}

LogFragment::LogFragment(QSharedPointer<QFile> logfile, bool postFileContent,
		QString id,
		QString group, QObject *parent, QThread *workingThread) :
	LogFragment(logfile, postFileContent, id, QDateTime(), QDateTime(), group, parent, workingThread)
{}

LogFragment::LogFragment(QSharedPointer<QFile> logfile, bool postFileContent, QString id,
						 QDateTime from, QDateTime to,
						 QString group, QObject *parent, QThread *workingThread) :
	QBuffer(parent),
	_logFile(logfile),
	_postFileContent(postFileContent),
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
	_workingThread(workingThread),
	_valid(false)

{
	if(!_logFile->isReadable())
		_logFile->open(QIODevice::ReadOnly);


	if(!_logFile->isOpen()) {
		DP_CMD_LOG_FRAGMENT_ERROR("LogFragment aborted as log file could not get opened...");
		return;
	}

	if(!System::startResetSensitiveProcess(RESET_PRIORITY_BULK)) {
		DP_RESET_ERROR("NOT CONSTRUCTING NEW LOG FRAGMENT: Reset protection in progress.");
		_lastFragment=true;
		return;
	}

	if(_workingThread != 0) {
		DP_CMD_LOG_FRAGMENT("LogFragment moving to thread" << _workingThread);
		moveToThread(_workingThread);
	}
	_valid=true;
}

LogFragment::~LogFragment() {
	CHECKPOINT("LogFragment::~LogFragment()");
	System::endResetSensitiveProcess(RESET_PRIORITY_BULK);
}

bool LogFragment::isValid() const
{
	return _valid;
}

void LogFragment::fillFragment(void)
{
	DP_CMD_LOG_FRAGMENT_ERROR("LogFragment starts filling up...");

	const QRegularExpression recordRegexp(QStringLiteral("(.*?)\\|VALUES (.*?)\\|(.*)"));
	QString record;
	_logFile->seek(_startIndex);
	do {
		_endIndex = _logFile->pos();

		record = _logFile->readLine(LOG_MAX_BUFFER_SIZE);
		QRegularExpressionMatch match(recordRegexp.match(record));
		if(match.hasMatch()) {
			if(_from.isValid() && (QDateTime::fromString(match.captured(1)) < _from))
				continue;
			if(_to.isValid() && (QDateTime::fromString(match.captured(1)) > _to))
				continue;
			if((!_group.isEmpty()) &&
					(!QRegularExpression(_group).match(match.captured(2)).hasMatch())
					)
				continue;

			if((size() + record.size()) < LOG_MAX_BUFFER_SIZE) {
//				DP_CMD_LOG_FRAGMENT_DETAILS(record);
				buffer().append(record);
				++_recordCnt;
				if(_firstFound < 0)
					_firstFound = _endIndex;
				_lastFound = _endIndex + record.size();

				DP_CMD_LOG_FRAGMENT_DETAILS("\tLogFragment recordCnt=" << _recordCnt << ", size=" << size());
				continue;
			}
			else {
				DP_CMD_LOG_FRAGMENT("LogFragment completed partial fill up. Moving back to thread" << _parentThread);
				moveToThread(_parentThread);
				emit fragmentReady(this);
				return;
			}
		}

	} while (!record.isEmpty());
	_lastFragment = true;
	DP_CMD_LOG_FRAGMENT("LogFragment finished filling up. Moving back to thread" << _parentThread);
	moveToThread(_parentThread);
	emit fragmentReady(this);
	return;
}

LogFragment *LogFragment::nextFragment(QThread *workingThread) {
	if(_lastFragment)
		return 0;

	LogFragment *fragment(
				new LogFragment(_logFile,
								_postFileContent,
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
