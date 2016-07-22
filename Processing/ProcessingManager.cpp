#include "ProcessingManager.h"

#include <QDebug>
#include <QSettings>

#include "Globals.h"
#include "Processing/RequestManager.h"
#include "Modbus/ModbusSerialMaster.h"

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

void ProcessingManager::onRequest() {
	RequestManager *rm(static_cast<RequestManager*>(sender()));
//	RequestManager *rm(sender());
	qDebug() << "PROCESSING" << rm->objectName() << ":" << rm;
	ADUSharedPtr_t req(new ApplicationDataUnitSerial(rm->device(), rm->request()));
	qDebug() << "\tREQUEST: " << req->toHex();
	ADUSharedPtr_t response(serialMaster->process(req));
	if(response.isNull()) {
		qDebug() << "\tNULL RESPONSE!";
	}
	else {
//		qDebug() << "\tRESPONSE: " << response->toHex();
		rm->onResponse(response);
	};
}
