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

bool NetworkSender::send(QString url, QHttpMultiPart *multiPart, quint64 timeout) {
	return send(parseUrl(url), multiPart, timeout);
}

bool NetworkSender::send(QUrl url, QHttpMultiPart *multiPart, quint64 timeout) {
	qDebug() << "\tNetworkSender::send(" << url.url() << ")";

	if((!url.isValid()) || (multiPart == 0)) {
		qDebug() << "\tNetworkSender: invalid request (URL=" << url.url() << ", multipart=" << multiPart;
		emit finished(QSharedPointer<QNetworkReply>());
		_reply.reset();
		return false;
	}

	qDebug() << "CHECKPOINT MIKE" << multiPart << networkAccessManager();
	_reply.reset(networkAccessManager()->post(QNetworkRequest(url), multiPart));
	qDebug() << "CHECKPOINT NOVEMBER";
	multiPart->setParent(_reply.data());
	qDebug() << "\tNetworkSender: transmitted to " << url << "reply.isRunnung()=" << _reply->isRunning();
//	connect(_reply.data(), &QNetworkReply::finished, this, &NetworkSender::onFinished, Qt::UniqueConnection);
	connect(_reply.data(), &QNetworkReply::finished, this, &NetworkSender::onFinished);
	_timer.start(timeout, this);
	return _reply->isRunning();
}

void NetworkSender::onFinished() {
	_timer.stop();
	if(_reply->error() != 0)
		qDebug() << "URL" << _reply->url() << "SENDING ERROR: " << _reply->errorString();
   else {
		qDebug() << "URL" << _reply->url() << "SENT WITH RESULT" << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
/// @todo Consider more sophisticated processing above - even signalling uplink

		qDebug() << "NetworkSender HEADERS:";
		foreach (QNetworkReply::RawHeaderPair header, _reply->rawHeaderPairs()) {
			qDebug() << "\t" << header.first << "=" << header.second;
		}
//		 qDebug() << "DATA:\n" << _reply->readAll();
		qDebug() << "DATA SIZE:" << _reply->bytesAvailable();
		emit finished(_reply);
	}
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
