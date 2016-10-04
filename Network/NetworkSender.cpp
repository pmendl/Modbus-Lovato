#include "NetworkSender.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QHttpMultiPart>
#include <QTimerEvent>
#include <QSettings>

#include "Network/DebugHttpMultiPart.h"

NetworkSender::NetworkSender(QObject * parent, QString defaultSlotUrl, quint64 defaultSlotTimeout) :
	NetworkAccessBase(parent),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(parseUrl(defaultSlotUrl))
{}

NetworkSender::NetworkSender(QObject * parent, QUrl defaultSlotUrl, quint64 defaultSlotTimeout) :
	NetworkAccessBase(parent),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}


NetworkSender::NetworkSender(QObject * parent, quint64 defaultSlotTimeout) :
	NetworkAccessBase(parent),
	_defaultSlotTimeout(defaultSlotTimeout)
{}



NetworkSender::NetworkSender(QString defaultSlotUrl, quint64 defaultSlotTimeout)  :
	NetworkAccessBase(0),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}


NetworkSender::NetworkSender(QUrl defaultSlotUrl, quint64 defaultSlotTimeout)  :
	NetworkAccessBase(0),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}


NetworkSender::NetworkSender(quint64 defaultSlotTimeout) :
	NetworkAccessBase(0),
	_defaultSlotTimeout(defaultSlotTimeout)
{
	readPanicConnections();
}

QNetworkReply *NetworkSender::sendMultipart(QHttpMultiPart *multiPart) {
	return sendToUrl(_defaultSlotUrl, multiPart);
}

QNetworkReply *NetworkSender::sendWithTimeout(QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithTimeout(_defaultSlotUrl, multiPart, timeout);
}

QNetworkReply *NetworkSender::sendWithObject(QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObject(_defaultSlotUrl, multiPart, originatingObject);
}

QNetworkReply *NetworkSender::sendWithObjectAndTimeout(QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(_defaultSlotUrl, multiPart, originatingObject, timeout);
}

QNetworkReply *NetworkSender::sendToString(QString url, QHttpMultiPart *multiPart) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, sender(), _defaultSlotTimeout);
}

QNetworkReply *NetworkSender::sendToUrl(QUrl url, QHttpMultiPart *multiPart) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, sender(), _defaultSlotTimeout);
}


QNetworkReply *NetworkSender::sendToStringWithTimeout(QString url, QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, sender(), timeout);
}

QNetworkReply *NetworkSender::sendToUrlWithTimeout(QUrl url, QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, sender(), timeout);
}


QNetworkReply *NetworkSender::sendToStringWithObject(QString url, QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, originatingObject, _defaultSlotTimeout);
}

QNetworkReply *NetworkSender::sendToUrlWithObject(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, originatingObject, _defaultSlotTimeout);

}


QNetworkReply *NetworkSender::sendToStringWithObjectAndTimeout(QString url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, originatingObject, timeout);
}

QNetworkReply *NetworkSender::sendToUrlWithObjectAndTimeout(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	QNetworkRequest req(url);
	req.setOriginatingObject(originatingObject);
	return send(req, multiPart, timeout);
}


QNetworkReply *NetworkSender::send(QString url, QHttpMultiPart *multiPart, quint64 timeout) {
	return send(parseUrl(url), multiPart, timeout);
}

QNetworkReply *NetworkSender::send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout) {
	return send(QNetworkRequest(url), multiPart, timeout);
}


QNetworkReply *NetworkSender::send(QNetworkRequest request, QHttpMultiPart *multiPart, quint64 timeout) {
/*
	HTTP_MULTI_PART_USED *multiPart(dynamic_cast<HTTP_MULTI_PART_USED *>(xmultiPart));
	if(multiPart == 0) {
		qDebug() << "*** NON-DEBUG MULTIPART REQUESTED TO BE SENT *** ABORTING ***";
		return 0;
	}
*/
	qDebug() << "\tNetworkSender::send(" << multiPart << request.url() << ")";

	/*
	qDebug() << "*** DEBUG ONLY multipart destruction";
//	delete multiPart;
	QObject *parent(new QObject);
	multiPart->setParent(parent);
	delete parent;
	return 0;
*/
	if((!request.url().isValid()) || (multiPart == 0)) {
		qDebug() << "\tNetworkSender: invalid request (URL=" << request.url() << ", multipart=" << multiPart;
		return 0;
	}

	QNetworkReply *reply(networkAccessManager()->post(request, multiPart));
	reply->setParent(this);
	multiPart->setParent(reply);
	qDebug() << "\tNetworkSender: transmitted to " << request.url() << "; reply.isRunnung()=" << reply->isRunning();
	connect(reply, &QNetworkReply::finished, this, &NetworkSender::onReplyFinished);
	_timerIds.insert(reply, startTimer(timeout));
	emit multipartSent(multiPart, reply);
	return reply;
}

void NetworkSender::onReplyFinished() {
	QNetworkReply *reply(dynamic_cast<QNetworkReply *>(sender()));
	if(reply != 0) {
		if(_timerIds.contains(reply)) {
			killTimer(_timerIds.value(reply));
			_timerIds.remove(reply);
		}

		_commandsDistributor.emitCommandReply(reply);

		if((reply->parent() == 0) || (reply->parent() == this)) {
			reply->deleteLater();
		}
	}
}

void NetworkSender::timerEvent(QTimerEvent *event) {
	NetworkAccessBase::timerEvent(event);
	if(event->isAccepted())
		return;
	killTimer(event->timerId());
	QNetworkReply *reply(0);
	for (QHash<QNetworkReply *, int>::iterator i = _timerIds.begin(); i != _timerIds.end(); ++i)
		if (i.value() == event->timerId()) {
			reply = i.key();
			_timerIds.erase(i);
			break;
		}
	if(reply != 0) {
		if(reply->isRunning()) {
			qDebug() << "NetworkSender: URL" << reply->url() << "SENDING TIMEOUT - ABORTING !!!";
			reply->abort();
		}
	}
}

const CommandsDistributor *NetworkSender::commandsDistributor()
{
	return &_commandsDistributor;
}

QUrl NetworkSender::defaultSlotUrl() const
{
	return _defaultSlotUrl;
}

void NetworkSender::setDefaultSlotUrl(const QString &defaultSlotUrl) {
	setDefaultSlotUrl(parseUrl(defaultSlotUrl));
}

void NetworkSender::setDefaultSlotUrl(const QUrl &defaultSlotUrl)
{
	_defaultSlotUrl = defaultSlotUrl;
}

quint64 NetworkSender::defaultSlotTimeout() const
{
	return _defaultSlotTimeout;
}

void NetworkSender::setDefaultSlotTimeout(const quint64 &defaultSlotTimeout)
{
	_defaultSlotTimeout = defaultSlotTimeout;
}

CommandsDistributor NetworkSender::_commandsDistributor;
