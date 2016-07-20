#include "ProcessingManager.h"

#include <QDebug>
#include <QSettings>

#include "Globals.h"
#include "Processing/RequestManager.h"

ProcessingManager::ProcessingManager(QObject *parent) : QObject(parent)
{
	QSettings settings;
//	QList<QSharedPointer<RequestManager>> requestManagers;
	settings.beginGroup(REQUEST_GROUPS_KEY);
	foreach(QString group, settings.childGroups()) {
		qDebug() << "Processing group" << group;
		settings.beginGroup(group);
		RequestManager *rm(new RequestManager(settings, this));
		connect(rm, &RequestManager::requesting, this, &ProcessingManager::onRequest);
		settings.endGroup();
	}
	settings.endGroup();
}

void ProcessingManager::onRequest(PDUSharedPtr_t request) {
	qDebug() << sender() << "->" << request->toHex();
}
