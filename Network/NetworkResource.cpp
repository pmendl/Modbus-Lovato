#include "NetworkResource.h"

#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimerEvent>
#include <QBuffer>

/**
 * @brief NetworkResource constructor
 *
 * @todo Doxygenize
 * @param url
 * @param basicPeriod
 * @param retryPeriod
 * @param parent
 */
NetworkResource::NetworkResource(QObject *parent, QString url, quint64 basicPeriod, quint64 retryPeriod) :
	QObject(parent),
	_url(url),
	_basicPeriod(basicPeriod),
	_retryPeriod(retryPeriod),
	_reply(0),
	_buffer(0)
{
	qDebug() << "CONSTRUCTOR: NetworkResource::NetworkResource";
	doRequest();
}

NetworkResource::NetworkResource(QString url, quint64 basicPeriod, quint64 retryPeriod) :
	NetworkResource(0, url, basicPeriod, retryPeriod)
{}

void NetworkResource::doRequest(void) {
	_oldReply=_reply;
	_reply = networkAccessManager()->get(QNetworkRequest(_url));
	qDebug() << "***** NETWORK REQUEST SENT:" << _reply << _url;

	connect(_reply, &QNetworkReply::finished,
			this, &NetworkResource::onReplyFinished);
	_timer.start(_retryPeriod, this);
}

void NetworkResource::onReplyFinished(void)
{
	qDebug() << "***** NETWORK REPLY FINISHED" << _reply << _reply->url().toString();
	if(_reply->error() != QNetworkReply::NetworkError::NoError) {
		qDebug() << "NETWORK ERROR:" << _reply->errorString();
		emit resourceStalled();
		_timer.start(_retryPeriod, this);
	}
	else {
		_buffer = new QBuffer(_reply);
		_buffer->setData(_reply->readAll());

		if(_oldReply) _oldReply->deleteLater();
		emit resourceChanged();
		_timer.start(_basicPeriod, this);
	}
}

void NetworkResource::timerEvent(QTimerEvent *event) {
	if(event->timerId() == _timer.timerId()) {
		doRequest();
	}
	else
		QObject::timerEvent(event);
}

QBuffer *NetworkResource::buffer() const
{
	return _buffer;
}

QNetworkReply *NetworkResource::oldReply() const
{
	return _oldReply;
}

QNetworkReply *NetworkResource::reply() const
{
	return _reply;
}

NetworkResource::~NetworkResource()
{
	qDebug() << "CALL NetworkResource::~NetworkResource()";

}
