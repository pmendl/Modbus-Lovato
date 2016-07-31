#include "NetworkSender.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>

NetworkSender::NetworkSender(QString url, QHttpMultiPart *multiPart, quint64 timeout)
{


	if(!_url.isValid())
		_url = QUrl::fromUserInput(url);

	if(!_url.isValid()) {
		emit finished(QSharedPointer<QNetworkReply>());
		qDebug() << "URL" << _url.url() << "IS INVALID (unparsable) !";
		deleteLater();
	}

	_reply.reset(networkAccessManager()->post(QNetworkRequest(_url), multiPart));
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
		deleteLater();
	}
}

void NetworkSender::timerEvent(QTimerEvent *) {
   qDebug() << "URL" << _url.url() << "SENDING TIMEOUT - ABORTING !!!";
   _reply->abort();
   emit finished(QSharedPointer<QNetworkReply>());
   deleteLater();
}
