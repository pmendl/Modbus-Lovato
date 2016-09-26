#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QBasicTimer>
#include <QUrl>
#include <QSet>

#include "Globals.h"
#include "Network/NetworkAccessBase.h"
#include "Network/CommandsDistributor.h"

class QNetworkReply;

class NetworkSender : public NetworkAccessBase
{
	Q_OBJECT

public:
	typedef QSet<QNetworkReply*> networkRepliesSet_t;


signals:
	void multipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply);


public slots:
	QNetworkReply *sendMultipart(QHttpMultiPart *multiPart);
	QNetworkReply *sendWithTimeout(QHttpMultiPart *multiPart, quint64 timeout);
	QNetworkReply *sendWithObject(QHttpMultiPart *multiPart, QObject *originatingObject);
	QNetworkReply *sendWithObjectAndTimeout(QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);
	QNetworkReply *sendToString(QString url, QHttpMultiPart *multiPart);
	QNetworkReply *sendToUrl(QUrl url, QHttpMultiPart *multiPart);
	QNetworkReply *sendToStringWithTimeout(QString url, QHttpMultiPart *multiPart, quint64 timeout);
	QNetworkReply *sendToUrlWithTimeout(QUrl url, QHttpMultiPart *multiPart, quint64 timeout);
	QNetworkReply *sendToStringWithObject(QString url, QHttpMultiPart *multiPart, QObject *originatingObject);
	QNetworkReply *sendToUrlWithObject(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject);
	QNetworkReply *sendToStringWithObjectAndTimeout(QString url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);
	QNetworkReply *sendToUrlWithObjectAndTimeout(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);

public:
	explicit NetworkSender(QObject * parent, QString defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QObject * parent = 0, QUrl defaultSlotUrl = QUrl(), quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QObject * parent, quint64 defaultSlotTimeout);
	explicit NetworkSender(QString defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QUrl defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(quint64 defaultSlotTimeout);
	virtual ~NetworkSender() {}

	QNetworkReply *send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	QNetworkReply *send(QString url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	QNetworkReply *send(QNetworkRequest request, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);

	quint64 defaultSlotTimeout() const;
	void setDefaultSlotTimeout(const quint64 &defaultSlotTimeout);
	QUrl defaultSlotUrl() const;
	void setDefaultSlotUrl(const QString &defaultSlotUrl);
	void setDefaultSlotUrl(const QUrl &defaultSlotUrl);

	static const CommandsDistributor *commandsDistributor();

protected slots:
	void onReplyFinished();

protected:
//	void readPanicConnections(void);
	void timerEvent(QTimerEvent *event);

private:
	quint64 _defaultSlotTimeout;
	QUrl _defaultSlotUrl;
	QHash<QNetworkReply *, int> _timerIds;
	static CommandsDistributor _commandsDistributor;
};

#endif // NETWORKSENDER_H
