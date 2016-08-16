#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QSharedPointer>
#include <QBasicTimer>
#include <QUrl>

#include "Globals.h"
#include "Network/NetworkAccessBase.h"

class NetworkSender : public QObject, public NetworkAccessBase
{
	Q_OBJECT

public:
	explicit NetworkSender(QString url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT, bool autodestroy = false);
	explicit NetworkSender(QString url, QHttpMultiPart *multiPart, bool autodestroy);
	explicit NetworkSender(QUrl url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT, bool autodestroy = false);
	explicit NetworkSender(QUrl url, QHttpMultiPart *multiPart, bool autodestroy);
	virtual ~NetworkSender() {}
	static QUrl parseUrl(QString url);

	QSharedPointer<QNetworkReply> reply() const;
	QSharedPointer<QNetworkReply> wait();

	void test();

signals:
	void finished(QSharedPointer<class QNetworkReply> reply);

protected slots:
	void onFinished();

protected:
	void timerEvent(QTimerEvent *);

private:
	QUrl _url;
	QSharedPointer<class QNetworkReply> _reply;
	bool _autodestroy;
	QBasicTimer _timer;

};

#endif // NETWORKSENDER_H
