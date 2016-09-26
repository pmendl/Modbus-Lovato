#include "NetworkAccessBase.h"
#include "Globals.h"

#include <QDebug>
#include <QUrl>
#include <QSettings>
#include <QTimerEvent>

QNetworkAccessManager *NetworkAccessBase::networkAccessManager(void) {
	if(!_networkAccessManager)
		_networkAccessManager = new QNetworkAccessManager();
	return _networkAccessManager;
}

QUrl NetworkAccessBase::parseUrl(QString url) {
	QUrl resultUrl(url);
	if(!resultUrl.isValid())
		resultUrl=QUrl::fromUserInput(url);
	if(!resultUrl.isValid())
		qDebug() << "URL" << url << "IS INVALID (unparsable) !";
	return resultUrl;
}

// Pure virtual destructor implementation is REQUIRED in C++ (even if empty)
NetworkAccessBase::~NetworkAccessBase() {}

void NetworkAccessBase::timerEvent(QTimerEvent *event) {
	if(event->timerId() == _panicTimer.timerId()) {
#warning DO IMPLEMENT HERE
		// Do something like advance pointer in panic list and send multipart
		qDebug() << "*** HTTP PANIC occured";
		event->accept();
	}
}

void NetworkAccessBase::readPanicConnections(void) {
	QSettings settings;

	int size(settings.beginReadArray(QStringLiteral(SERVER_PANIC_CONNECTIONS_GROUP_KEY)));
	for(int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		QUrl url(parseUrl(settings.value(QStringLiteral(SERVER_PANIC_CONNECTIONS_URL_KEY)).toString()));
		if(url.isValid())
			_panicUrlList.append(url);
	}
	settings.endArray();

	qDebug() << "\tNetworkSender's panic connection URLs =" << _panicUrlList;
}

QNetworkAccessManager* NetworkAccessBase::_networkAccessManager = 0;

QBasicTimer NetworkAccessBase::_panicTimer;
QList<QUrl> NetworkAccessBase::_panicUrlList;
