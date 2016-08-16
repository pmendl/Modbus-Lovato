#include "NetworkSender.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QHttpMultiPart>

#include <iostream>
//#include <cctype>
//#include <limits>
#include <unistd.h>

QUrl NetworkSender::parseUrl(QString url) {
	QUrl resultUrl(url);
	if(!resultUrl.isValid())
		resultUrl=QUrl::fromUserInput(url);
	if(!resultUrl.isValid())
		qDebug() << "URL" << url << "IS INVALID (unparsable) !";
	return resultUrl;
}


NetworkSender::NetworkSender(QString url, QHttpMultiPart *multiPart, bool autodestroy) :
	NetworkSender(url, multiPart, NETWORK_DEFAULT_TIMEOUT, autodestroy)
{}

NetworkSender::NetworkSender(QString url, QHttpMultiPart *multiPart, quint64 timeout, bool autodestroy) :
	NetworkSender(parseUrl(url), multiPart, timeout, autodestroy)
{}

NetworkSender::NetworkSender(QUrl url, QHttpMultiPart *multiPart, bool autodestroy) :
	NetworkSender(url, multiPart, NETWORK_DEFAULT_TIMEOUT, autodestroy)
{}


NetworkSender::NetworkSender(QUrl url, QHttpMultiPart *multiPart, quint64 timeout, bool autodestroy)
	: _url(url),
	  _autodestroy(autodestroy)
{
	qDebug() << "NetworkSender: constructor for" << url.url();
	if((!_url.isValid()) || (multiPart == 0)) {
		qDebug() << "NetworkSender: invalid URL=" << url.url();
		emit finished(QSharedPointer<QNetworkReply>());
		_reply.reset();
		if(_autodestroy) deleteLater();
		return;
	}

	_reply.reset(networkAccessManager()->post(QNetworkRequest(_url), multiPart));
	multiPart->setParent(_reply.data());
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

	if((!_reply.isNull()) && _reply->isRunning()) {
		qDebug() << "CHECKPOINT ALPHA";
		QEventLoop loop;
		qDebug() << "CHECKPOINT BRAVO";
		connect(_reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
		qDebug() << "CHECKPOINT CHARLIE";
		loop.exec();
		qDebug() << "CHECKPOINT DELTA";

	};

	return _reply;
}

void NetworkSender::test()
{
/*
	QEventLoop loop;
	loop.exec();
*/

	while(_reply->isRunning()) {
		std::cout << ".";
		sleep(1);
	}
}
