#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QSharedPointer>
#include <QBasicTimer>
#include <QUrl>

#include "Globals.h"
#include "Network/NetworkAccessBase.h"
#include "Network/CommandsDistributor.h"

class NetworkSender : public QObject, public NetworkAccessBase
{
	Q_OBJECT

signals:
	void finished(QSharedPointer<class QNetworkReply> reply);

public slots:
	QSharedPointer<QNetworkReply> sendMultipart(QHttpMultiPart *multiPart);
	QSharedPointer<QNetworkReply> sendMultipartWithTimeout(QHttpMultiPart *multiPart, quint64 timeout);
	QSharedPointer<QNetworkReply> sendMultipartWithObject(QHttpMultiPart *multiPart, QObject *originatingObject);
	QSharedPointer<QNetworkReply> sendMultipartWithObjectAndTimeout(QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);
	QSharedPointer<QNetworkReply> sendToString(QString url, QHttpMultiPart *multiPart);
	QSharedPointer<QNetworkReply> sendToUrl(QUrl url, QHttpMultiPart *multiPart);
	QSharedPointer<QNetworkReply> sendToStringWithTimeout(QString url, QHttpMultiPart *multiPart, quint64 timeout);
	QSharedPointer<QNetworkReply> sendToUrlWithTimeout(QUrl url, QHttpMultiPart *multiPart, quint64 timeout);
	QSharedPointer<QNetworkReply> sendToStringWithObject(QString url, QHttpMultiPart *multiPart, QObject *originatingObject);
	QSharedPointer<QNetworkReply> sendToUrlWithObject(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject);
	QSharedPointer<QNetworkReply> sendToStringWithObjectAndTimeout(QString url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);
	QSharedPointer<QNetworkReply> sendToUrlWithObjectAndTimeout(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout);

public:
	explicit NetworkSender(QObject * parent, QString defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QObject * parent = 0, QUrl defaultSlotUrl = QUrl(), quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QObject * parent, quint64 defaultSlotTimeout);
	explicit NetworkSender(QString defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(QUrl defaultSlotUrl, quint64 defaultSlotTimeout = NETWORK_DEFAULT_TIMEOUT);
	explicit NetworkSender(quint64 defaultSlotTimeout);
	virtual ~NetworkSender() {}
	static QUrl parseUrl(QString url);

	bool send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	bool send(QString url, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	bool send(QNetworkRequest request, QHttpMultiPart *multiPart, quint64 timeout = NETWORK_DEFAULT_TIMEOUT);

//	QSharedPointer<QNetworkReply> reply() const;
//	QSharedPointer<QNetworkReply> wait();

	quint64 defaultSlotTimeout() const;
	void setDefaultSlotTimeout(const quint64 &defaultSlotTimeout);
	QUrl defaultSlotUrl() const;
	void setDefaultSlotUrl(const QString &defaultSlotUrl);
	void setDefaultSlotUrl(const QUrl &defaultSlotUrl);

	static const CommandsDistributor *commandsDistributor();

protected slots:
	void onFinished();

protected:
	void timerEvent(QTimerEvent *);

private:
	quint64 _defaultSlotTimeout;
	QUrl _defaultSlotUrl;
	QSharedPointer<class QNetworkReply> _reply;
	QBasicTimer _timer;
	static CommandsDistributor _commandsDistributor;
};

#endif // NETWORKSENDER_H
