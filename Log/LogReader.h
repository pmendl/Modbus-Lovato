#ifndef LOGREADER_H
#define LOGREADER_H

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QSharedPointer>

#include "Globals.h"
#include HTTP_MULTI_PART_INCLUDE
#include "Network/NetworkSender.h"
#include "Log/LogFragment.h"

class HTTP_MULTI_PART_USED;

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
	void onMultipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply);
	void onReplyFinished();

protected:
	void processFragment(LogFragment *fragment);
	void checkSending();
	void sendReadyFragment();

private:
	NetworkSender _sender;
	LogFragment *_readyFragment, *_sendPendingFragment;
	bool _lastFragment;
	HTTP_MULTI_PART_USED *_multiPart;
//	bool _sendPending;
	bool _postFileContent;
};

#endif // LOGREADER_H
