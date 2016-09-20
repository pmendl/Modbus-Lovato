#include "NetworkSender.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QHttpMultiPart>

QUrl NetworkSender::parseUrl(QString url) {
	QUrl resultUrl(url);
	if(!resultUrl.isValid())
		resultUrl=QUrl::fromUserInput(url);
	if(!resultUrl.isValid())
		qDebug() << "URL" << url << "IS INVALID (unparsable) !";
	return resultUrl;
}

NetworkSender::NetworkSender(QObject * parent, QString defaultSlotUrl, quint64 defaultSlotTimeout) :
	QObject(parent),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(parseUrl(defaultSlotUrl))
{}

NetworkSender::NetworkSender(QObject * parent, QUrl defaultSlotUrl, quint64 defaultSlotTimeout) :
	QObject(parent),
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}

NetworkSender::NetworkSender(QObject * parent, quint64 defaultSlotTimeout) :
	QObject(parent),
	_defaultSlotTimeout(defaultSlotTimeout)
{}


NetworkSender::NetworkSender(QString defaultSlotUrl, quint64 defaultSlotTimeout)  :
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}

NetworkSender::NetworkSender(QUrl defaultSlotUrl, quint64 defaultSlotTimeout)  :
	_defaultSlotTimeout(defaultSlotTimeout),
	_defaultSlotUrl(defaultSlotUrl)
{}

NetworkSender::NetworkSender(quint64 defaultSlotTimeout) :
	_defaultSlotTimeout(defaultSlotTimeout)
{}

QSharedPointer<QNetworkReply> NetworkSender::sendMultipart(QHttpMultiPart *multiPart) {
	qDebug() << "*** _defaultSlotUrl =" << _defaultSlotUrl.url();
	return sendToUrl(_defaultSlotUrl, multiPart);
}

QSharedPointer<QNetworkReply> NetworkSender::sendMultipartWithTimeout(QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithTimeout(_defaultSlotUrl, multiPart, timeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendMultipartWithObject(QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObject(_defaultSlotUrl, multiPart, originatingObject);
}

QSharedPointer<QNetworkReply> NetworkSender::sendMultipartWithObjectAndTimeout(QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(_defaultSlotUrl, multiPart, originatingObject, timeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendToString(QString url, QHttpMultiPart *multiPart) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, sender(), _defaultSlotTimeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendToUrl(QUrl url, QHttpMultiPart *multiPart) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, sender(), _defaultSlotTimeout);
}


QSharedPointer<QNetworkReply> NetworkSender::sendToStringWithTimeout(QString url, QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, sender(), timeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendToUrlWithTimeout(QUrl url, QHttpMultiPart *multiPart, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, sender(), timeout);
}


QSharedPointer<QNetworkReply> NetworkSender::sendToStringWithObject(QString url, QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, originatingObject, _defaultSlotTimeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendToUrlWithObject(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject) {
	return sendToUrlWithObjectAndTimeout(url, multiPart, originatingObject, _defaultSlotTimeout);

}


QSharedPointer<QNetworkReply> NetworkSender::sendToStringWithObjectAndTimeout(QString url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	return sendToUrlWithObjectAndTimeout(parseUrl(url), multiPart, originatingObject, timeout);
}

QSharedPointer<QNetworkReply> NetworkSender::sendToUrlWithObjectAndTimeout(QUrl url, QHttpMultiPart *multiPart, QObject *originatingObject, quint64 timeout) {
	QNetworkRequest req(url);
	req.setOriginatingObject(originatingObject);
	send(req, multiPart, timeout);
	return _reply;
}


bool NetworkSender::send(QString url, QHttpMultiPart *multiPart, quint64 timeout) {
	return send(parseUrl(url), multiPart, timeout);
}

bool NetworkSender::send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout) {
	return send(QNetworkRequest(url), multiPart, timeout);
}


bool NetworkSender::send(QNetworkRequest request, QHttpMultiPart *multiPart, quint64 timeout) {

	qDebug() << "\tNetworkSender::send(" << request.url() << ")";

	if((!request.url().isValid()) || (multiPart == 0)) {
		qDebug() << "\tNetworkSender: invalid request (URL=" << request.url() << ", multipart=" << multiPart;
		emit finished(QSharedPointer<QNetworkReply>());
		return false;
	}

	_reply.reset(networkAccessManager()->post(request, multiPart));
	multiPart->setParent(_reply.data());
	qDebug() << "\tNetworkSender: transmitted to " << request.url() << "reply.isRunnung()=" << _reply->isRunning();
//	connect(_reply.data(), &QNetworkReply::finished, this, &NetworkSender::onFinished, Qt::UniqueConnection);
	connect(_reply.data(), &QNetworkReply::finished, this, &NetworkSender::onFinished);
	_timer.start(timeout, this);
	return _reply->isRunning();
}

void NetworkSender::onFinished() {
	_timer.stop();
	_commandsDistributor.emitCommandReply(_reply);
	emit finished(_reply);
	_reply.reset();
}

void NetworkSender::timerEvent(QTimerEvent *) {
	if(!_reply->isRunning()) {
		_timer.stop();
		return;
	}

   qDebug() << "NetworkSender: URL" << _reply->url() << "SENDING TIMEOUT - ABORTING !!!";
   _reply->abort();
   emit finished(QSharedPointer<QNetworkReply>());
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

/*
QSharedPointer<QNetworkReply> NetworkSender::reply() const
{
	return _reply;
}

QSharedPointer<QNetworkReply> NetworkSender::wait() {

	if((!_reply.isNull()) && _reply->isRunning()) {
		QEventLoop loop;
		connect(_reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(_reply.data(), &QNetworkReply::finished, [](){
			qDebug() << "*** REPLY FINISHED CALLED ***";
		});
//		loop.exec();
		qDebug() << "*** REPLY WAITING LOOP CALLED ***";
		while(_reply->isRunning());
		qDebug() << "*** REPLY WAITING LOOP EXITED ***";

	};
	return _reply;
}
*/

CommandsDistributor NetworkSender::_commandsDistributor;
