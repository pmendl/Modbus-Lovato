#ifndef LOGREADER_H
#define LOGREADER_H

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include <QSharedPointer>


class LogReader : public QThread
{
public:
	LogReader(QString url, QString pathname, QString id,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogReader(QString url, QString pathname,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	virtual ~LogReader();
	bool isValid();
	virtual void run();


private:
	QString _url;
	QFile _logFile;
	bool _opened;
	qint64 _startIndex, _endIndex;
	QDateTime _from, _to;
	QString _group, _id;
	QByteArray _logOutput;
	QBuffer _logBuffer;
	QSharedPointer<class NetworkSender> _sender;
};

#endif // LOGREADER_H
