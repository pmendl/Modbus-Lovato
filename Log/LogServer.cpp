#include "LogServer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QDateTime>

class LogWritter : public QThread {
	Q_OBJECT

public:
	LogWritter(QString pathname, QString record, QObject *parent = 0) :
		QThread(parent),
		_pathname(pathname),
		_record(record)
	{}

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
	LogWritter *writter = new LogWritter(pathname(filename), record);
	writter->start();
}

QString LogServer::pathname(QString filename) {
	return _logDir.absoluteFilePath(filename);
}

bool LogServer::isValid() const
{
	return _isValid;
}

void LogWritter::run() {
	QFile file(_pathname);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
		file.write((QDateTime::currentDateTimeUtc().toString()
					+ QStringLiteral(" | ")
					+ _record
					+ QStringLiteral("\n")).toUtf8());
		file.close();
	}
	deleteLater();
}

#include "LogServer.moc"
