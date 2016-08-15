#ifndef LOGSERVER_H
#define LOGSERVER_H

#include <QSharedPointer>
#include <QDir>


class LogServer : public QObject
{
	Q_OBJECT
public:
	explicit LogServer(QString defaultLogPath, QObject *parent = 0);
	void log(QString filename, QString record);
	QString pathname(QString filename) const;

	bool isValid() const;

signals:

public slots:

private:
	QDir _logDir;
	bool _isValid;
};

#endif // LOGSERVER_H
