#include "NetworkSender.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

QUrl NetworkSender::parseUrl(QString url) {
	QUrl resultUrl(url);
	if(!resultUrl.isValid())
		resultUrl=QUrl::fromUserInput(url);
	if(!resultUrl.isValid())
		qDebug() << "URL" << url << "IS INVALID (unparsable) !";
	return resultUrl;
}


NetworkSender::NetworkSender(QString url, QSharedPointer<class QHttpMultiPart> multiPart, bool autodestroy) :
	NetworkSender(url, multiPart, NETWORK_DEFAULT_TIMEOUT, autodestroy)
{}

NetworkSender::NetworkSender(QString url, QSharedPointer<QHttpMultiPart> multiPart, quint64 timeout, bool autodestroy) :
	NetworkSender(parseUrl(url), multiPart, timeout, autodestroy)
{}

NetworkSender::NetworkSender(QUrl url, QSharedPointer<class QHttpMultiPart> multiPart, bool autodestroy) :
	NetworkSender(url, multiPart, NETWORK_DEFAULT_TIMEOUT, autodestroy)
{}


NetworkSender::NetworkSender(QUrl url, QSharedPointer<class QHttpMultiPart> multiPart, quint64 timeout, bool autodestroy)
	: _url(url),
	  _autodestroy(autodestroy)
{
	if(!_url.isValid()) {
		emit finished(QSharedPointer<QNetworkReply>());
		if(_autodestroy) deleteLater();;
	}

	_reply.reset(networkAccessManager()->post(QNetworkRequest(_url), multiPart.data()));
	qDebug() << "NetworkSender transmitted to " << _url << "reply.isRunnung()=" << _reply->isRunning();
	connect(_reply.data(), &QNetworkReply::finished, this, &NetworkSender::onFinished);
	_timer.start(timeout, this);


}

void NetworkSender::onFinished() {
	_timer.stop();
	if(_reply->error() != 0)
		qDebug() << "URL" << _reply->url() << "SENDING ERROR: " << _reply->errorString();
   else {
		qDebug() << "URL" << _reply->url() << "SENT WITH RESULT" << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
/// @todo Consider more sophisticated processing above - even signalling uplink

		qDebug() << "HEADERS:";
		foreach (QNetworkReply::RawHeaderPair header, _reply->rawHeaderPairs()) {
			qDebug() << "\t" << header.first << "=" << header.second;
		}
//		 qDebug() << "DATA:\n" << _reply->readAll();
		qDebug() << "DATA SIZE:" << _reply->bytesAvailable();
		emit finished(_reply);
		if(_autodestroy) deleteLater();;
	}
}

void NetworkSender::timerEvent(QTimerEvent *) {
   qDebug() << "URL" << _url.url() << "SENDING TIMEOUT - ABORTING !!!";
   _reply->abort();
   emit finished(QSharedPointer<QNetworkReply>());
   if(_autodestroy) deleteLater();;
}

QSharedPointer<QNetworkReply> NetworkSender::reply() const
{
	return _reply;
}

QSharedPointer<QNetworkReply> NetworkSender::wait() {
	if(_reply->isRunning()) {
		QEventLoop loop;
		connect(_reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();
	};
	return _reply;
}
