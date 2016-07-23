#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QtNetwork/QNetworkReply>

#include "Globals.h"
#include "Network/NetworkAccessBase.h"

class NetworkSender : public QObject, public NetworkAccessBase
{
	Q_OBJECT

public:
	explicit NetworkSender(QObject *parent, QString url, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QString url, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);

signals:
	void error(QNetworkReply::NetworkError);
	void finished();

public slots:
//	void send(channelsSet_t channels);

public:
	const QNetworkReply *reply(void) const;

private:
	QUrl _url;
	QNetworkReply *_reply;
	quint64 _lastSent;
	quint64 _timeout;
};

#endif // NETWORKSENDER_H
