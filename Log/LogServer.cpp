#include "LogServer.h"

#include "Debug/DebugMacros.h"
#include <QCoreApplication>
#include <QThread>
#include <QDateTime>

#include "Globals.h"

class LogWritter : public QThread {
	Q_OBJECT

public:
	LogWritter(QString pathname, QString record, QObject *parent = 0);

	virtual ~LogWritter();

public slots:
	void run();

private:
	QString _pathname, _record;
	static int _instanceCount;
};

int LogWritter::_instanceCount = 0;

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

	LogWritter *writter = new LogWritter(pathname(filename), record, this);

//#warning DEBUG ONLY !!!!!!!!!!!!!!!!!!!!!!!
	writter->start();
//	delete writter;
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
	if(!connect(this, &LogWritter::finished, this, &LogWritter::deleteLater)) {
		DP_LOGWRITER_ERROR("CONNECT FAILED LogWritter::finished");
	};
	DP_LOGWRITTER_INSTANCES("(" << ++_instanceCount << ")");
/*
#warning DEBUG ONLY
	connect(this, &LogWritter::finished, this, [this]() {
		DP_LOGGING_DEBUG("-LogWritter finished-" << this);
	});
*/
}

LogWritter::~LogWritter()
{
	DP_LOGWRITTER_INSTANCES("~(" <<--_instanceCount << ")~");
}

void LogWritter::run() {
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
	else {
		DP_LOGGING_ERROR("Log ERROR:" << _pathname << file.errorString());
	}
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
