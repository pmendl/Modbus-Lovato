#ifndef LOGREADER_H
#define LOGREADER_H

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QSharedPointer>

#include "Network/NetworkSender.h"
#include "Log/LogFragment.h"


class LogReader : public QThread
{
	Q_OBJECT

public slots:
	void onFragmentReady(LogFragment *fragment);

public:
	LogReader(QString url, QString pathname,
			  QString id,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogReader(QString url, QString pathname,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogReader(QString url, QString pathname,
			  QString id,
			  QString group, QObject *parent = 0);
	virtual ~LogReader();
//	bool isValid();
//	virtual void run();

/*
protected slots:
	void onFinished();
*/

protected:
	void processFragment(LogFragment *fragment);

private:
//	QString _url;
//	QFile _logFile;
//	LogFragment *_logFragment;

	NetworkSender _sender;
};

#endif // LOGREADER_H
