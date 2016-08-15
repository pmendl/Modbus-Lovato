#include "LogServer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QDateTime>

#include "Globals.h"

class LogWritter : public QThread {
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

	qDebug() << "\tLogs directory = " << _logDir.canonicalPath();

	if(!(_isValid = _logDir.mkpath(_logDir.canonicalPath()))) {
		qDebug() << "\tCould NOT create requested directory !";
		return;
	}
}

void LogServer::log(QString filename, QString record) {
	LogWritter *writter = new LogWritter(pathname(filename), record, this);
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
	QFile file(_pathname);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
		file.write((QDateTime::currentDateTimeUtc().toString()
					+ QStringLiteral(LOG_SEPARATOR_ITEMS)
					+ _record
					+ QStringLiteral(LOG_SEPARATOR_RECORD)).toUtf8());
		file.close();
		file.flush();
		qDebug() << "WRITTEN" << _pathname;
	}
	else
		qDebug() << _pathname << file.errorString();
//	deleteLater();
}

#include "LogServer.moc"
