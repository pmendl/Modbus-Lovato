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

public:
	LogReader(QString url, QString pathname, bool postFileContent,
			  QString id,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogReader(QString url, QString pathname, bool postFileContent = true,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogReader(QString url, QString pathname, bool postFileContent,
			  QString id,
			  QString group, QObject *parent = 0);
	virtual ~LogReader();

	bool postFileContent() const;

protected slots:
	void onFragmentReady(LogFragment *fragment);
	void checkSending();

protected:
	void processFragment(LogFragment *fragment);
	void sendReadyFragment();

private:
	NetworkSender _sender;
	LogFragment *_readyFragment;
	bool _lastFragment;
	QSharedPointer<QNetworkReply> _runningReply;
	bool _postFileContent;
};

#endif // LOGREADER_H
