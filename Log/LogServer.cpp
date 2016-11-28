#include "LogServer.h"

#include <QCoreApplication>
#include <QThread>
#include <QDateTime>

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Debug/InstanceCounterBase.h"
#include "System/Reset.h"

class LogWritter : public QThread
{
	Q_OBJECT

public:
	LogWritter(QString pathname, QString record, QObject *parent = 0);

	virtual ~LogWritter() {}

public slots:
	void run();

private:
	QString _pathname, _record;
};

LogServer::LogServer(QString defaultLogPath, QObject *parent) : QObject(parent)
{
	if(QDir::isAbsolutePath(defaultLogPath))
		_logDir.setPath(defaultLogPath);
	else
		_logDir.setPath(QCoreApplication::applicationDirPath() + QStringLiteral("/")  + defaultLogPath);

	_logDir.makeAbsolute();

	DP_LOGGING_INIT("\tLogs directory = " << _logDir.canonicalPath());

	if(!(_isValid = _logDir.mkpath(_logDir.canonicalPath()))) {
		DP_LOGGING_INIT("\tCould NOT create requested directory !");
		return;
	}
}

void LogServer::log(QString filename, QString record) {
	LogMaintenanceLocker locker(this);

	if(!System::startResetSensitiveProcess(RESET_PRIORITY_IMMEDIATE)) {
		DP_LOGGING_ERROR("NOT WRITTING LOG RECORD: Reset protection in progress.");
		return;
	}

	LogWritter *writter = new LogWritter(pathname(filename), record, this);
	connect(writter, &LogWritter::finished, writter, &QObject::deleteLater);
	writter->start();
}

QString LogServer::pathname(QString filename) const {
	return _logDir.absoluteFilePath(filename);
}

bool LogServer::isValid() const
{
	return _isValid;
}

LogWritter::LogWritter(QString pathname, QString record, QObject *parent) :
	QThread(parent),
	_pathname(pathname),
	_record(record)
{
	connect(this, &LogWritter::finished, this, &LogWritter::deleteLater);
}


void LogWritter::run() {
	LogMaintenanceLocker locker(dynamic_cast<LogServer *>(parent()));
	QFile file(_pathname);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
		file.write((QDateTime::currentDateTimeUtc().toString()
					+ QStringLiteral(LOG_SEPARATOR_ITEMS)
					+ _record
					+ QStringLiteral(LOG_SEPARATOR_RECORD)).toUtf8());
		file.close();
		file.flush();
		DP_LOGGING_ACTION("Log WRITTEN:" << _pathname);
	}
	else
		DP_LOGGING_ERROR(_pathname << file.errorString());
	System::endResetSensitiveProcess(RESET_PRIORITY_IMMEDIATE);
}

LogMaintenanceLocker::LogMaintenanceLocker(LogServer *logServer):
	_logServer(logServer)
{
	_logServer->lockForMaintenance(+1);
}

LogMaintenanceLocker::~LogMaintenanceLocker() {
	_logServer->lockForMaintenance(-1);
}
void LogServer::lockForMaintenance(int locksCountChange) {
	QMutexLocker lock(&_fileLockMutex);

	if(_lockedForFileMaintenanceCount + locksCountChange > 0)
		_lockedForFileMaintenanceCount += locksCountChange;
	else
		_lockedForFileMaintenanceCount = 0;
}

bool LogServer::isLockedForMaintenance() const {
	QMutexLocker lock(&_fileLockMutex);
	return _lockedForFileMaintenanceCount > 0;
}

QSharedPointer<LogMaintenanceLocker> LogServer::fileMaintenanceLocker() {
	return QSharedPointer<LogMaintenanceLocker>(new LogMaintenanceLocker(this));
}

QMutex LogServer::_fileLockMutex;
int LogServer::_lockedForFileMaintenanceCount;


#include "LogServer.moc"
