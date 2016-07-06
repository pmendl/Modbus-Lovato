#ifndef NETWORKRESOURCE_H
#define NETWORKRESOURCE_H

#include <QObject>
#include <QBasicTimer>
#include <QDateTime>

#include "Network/NetworkAccessBase.h"

class NetworkResource : public QObject, public NetworkAccessBase
{
	Q_OBJECT
public:
	explicit NetworkResource(QObject *parent, QString url, quint64 basicPeriod,
							 quint64 retryPeriod=5*60*1000);
	explicit NetworkResource(QString url, quint64 basicPeriod,
							 quint64 retryPeriod=5*60*1000);
	virtual ~NetworkResource();

signals:
	void resourceChanged(void);
	void resourceStalled(void);

public:
	class QNetworkReply *reply() const;
	class QNetworkReply *oldReply() const;

	class QBuffer *buffer() const;

protected slots:
	virtual void onReplyFinished(void);

protected:
	virtual void doRequest(void);
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

private:
	QString _url;
	QBasicTimer _timer;
//	bool _retry;
	quint64 _basicPeriod, _retryPeriod;
	class QNetworkReply *_reply, *_oldReply;
	class QBuffer *_buffer;
};

#endif // NETWORKRESOURCE_H
