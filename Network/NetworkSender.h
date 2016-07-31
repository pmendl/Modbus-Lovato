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
	explicit NetworkSender(QString url, class QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QUrl url, class QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	virtual ~NetworkSender() {}
	static QUrl parseUrl(QString url);

signals:
	void finished(QSharedPointer<class QNetworkReply> reply);

protected slots:
	void onFinished();

protected:
	void timerEvent(QTimerEvent *);

private:
	QUrl _url;
	QSharedPointer<class QNetworkReply> _reply;
	QBasicTimer _timer;

};

#endif // NETWORKSENDER_H
