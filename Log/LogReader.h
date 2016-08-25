#ifndef LOGREADER_H
#define LOGREADER_H

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QSharedPointer>

#include "Network/NetworkSender.h"


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

protected slots:
	void onFinished();

private:
	QString _url;
	QFile _logFile;
	bool _opened;
	qint64 _startIndex, _endIndex;
	QDateTime _from, _to;
	QString _group, _id;
	class QBuffer *_logBuffer;
	NetworkSender *_sender;


	void httpTransmit(void);
};

#endif // LOGREADER_H
