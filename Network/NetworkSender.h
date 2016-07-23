#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QtNetwork/QNetworkReply>

#include "Network/NetworkAccessBase.h"
//#include "UmbProtocol/UmbChannel.h"

class NetworkSender : public QObject, public NetworkAccessBase
{
	Q_OBJECT

public:
	explicit NetworkSender(QObject *parent, QString url, quint64 period, quint64 timeout = 5*60*1000);
	explicit NetworkSender(QString url, quint64 period, quint64 timeout = 5*60*1000);

signals:
	void error(QNetworkReply::NetworkError);
	void finished();

public slots:
//	void send(channelsSet_t channels);

public:
	QNetworkReply *reply(void);

private:
	QUrl _url;
	QNetworkReply *_reply;
	quint64 _lastSent;
	quint64 _period;
	quint64 _timeout;
//	channelsSet_t _channels;
};

#endif // NETWORKSENDER_H
