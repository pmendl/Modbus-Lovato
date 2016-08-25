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
	NetworkSender();
	virtual ~NetworkSender() {}
	static QUrl parseUrl(QString url);

	bool send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	bool send(QString url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);

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
	QSharedPointer<class QNetworkReply> _reply;
	QBasicTimer _timer;

};

#endif // NETWORKSENDER_H
