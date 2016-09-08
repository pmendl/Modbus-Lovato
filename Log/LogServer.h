#ifndef LOGSERVER_H
#define LOGSERVER_H

#include <QSharedPointer>
#include <QDir>
#include <QMutexLocker>


class LogServer;

class LogMaintenanceLocker
{
public:
	LogMaintenanceLocker(LogServer	*_logServer);
	~LogMaintenanceLocker();

private:
	LogServer * const _logServer;
};

class LogServer : public QObject
{
	Q_OBJECT

friend class LogMaintenanceLocker;

public:
	explicit LogServer(QString defaultLogPath, QObject *parent = 0);
	void log(QString filename, QString record);
	QString pathname(QString filename) const;
	bool isValid() const;
	QSharedPointer<LogMaintenanceLocker> fileMaintenanceLocker();

signals:

public slots:

protected:
	void lockForMaintenance(int locksCountChange);
	bool isLockedForMaintenance() const;

private:
	QDir _logDir;
	bool _isValid;
	static QMutex _fileLockMutex;
	static int _lockedForFileMaintenanceCount;
};

#endif // LOGSERVER_H
