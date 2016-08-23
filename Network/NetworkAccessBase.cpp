#include "NetworkAccessBase.h"

#include <QDebug>

QNetworkAccessManager *NetworkAccessBase::networkAccessManager(void) {
	if(!_networkAccessManager)
		_networkAccessManager = new QNetworkAccessManager();
	return _networkAccessManager;
}

// Pure virtual destructor implementation is REQUIRED in C++ (even if empty)
NetworkAccessBase::~NetworkAccessBase() {}

QNetworkAccessManager* NetworkAccessBase::_networkAccessManager = 0;
