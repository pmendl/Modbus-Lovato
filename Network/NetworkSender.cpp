#include "NetworkSender.h"

#include <QDebug>
#include <QUrl>
#include <QDateTime>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QHttpMultiPart>

#include "Processing/RequestManager.h"

NetworkSender::NetworkSender(QObject *parent, QString url, quint64 timeout) :
	QObject(parent),
	_url(QUrl(url)),
	_reply(0),
	_timeout(timeout)
{}

NetworkSender::NetworkSender(QString url, quint64 timeout) :
	NetworkSender(0, url, timeout)
{}

/*
void NetworkSender::send(channelsSet_t channels) {
	if(_reply) {
		if(!_reply->isFinished()) {
			if ((QDateTime::currentMSecsSinceEpoch() - _lastSent) < _timeout) {
				qDebug() << "@@@ STILL WAITING FOR RESPONSE !!!";
				return;
			}
			else {
				qDebug() << "@@@ NETWORK SENDING TIMEOUT - ABORTING !!!";
				_reply->abort();
				_reply->deleteLater();
				_reply = 0;
				return;
			}
		}
		else {
			qDebug() << "@@@ SENT WITH RESULT" << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
#warning Consider more sophisticated processing here
			_reply->deleteLater();
			_reply = 0;
		}
	}

	if(!_channels.empty()) {
		for (UmbChannel ch : channels) {
			_channels.remove(ch.nr());
			_channels.insert(ch);
		}
	}
	else {
		_channels = channels;
	}

	if((QDateTime::currentMSecsSinceEpoch() - _lastSent) < _period) {
		qDebug() << "@@@ NOT SENDING BEFORE PERIOD";
		return;
	}

	// Adapted from http://doc.qt.io/qt-5/qhttpmultipart.html#details
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart textPart;
	for (UmbChannel ch : _channels) {
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString(QStringLiteral("form-data; name="))+ch.id()));
		textPart.setBody(ch.toByteArray());
		multiPart->append(textPart);
	}

	QNetworkRequest request(_url);
	// here connect signals etc.
	// --------------------------

	if(_reply) _reply->deleteLater();
	_lastSent = QDateTime::currentMSecsSinceEpoch();
	_reply=networkAccessManager()->post(request, multiPart);
	multiPart->setParent(_reply); // delete the multiPart with the reply
	connect(_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &NetworkSender::error);
	connect(_reply, &QNetworkReply::finished, this, &NetworkSender::finished);
}
*/

const QNetworkReply *NetworkSender::reply(void) const {
	return _reply;
}
