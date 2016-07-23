#include "ProcessingManager.h"

#include <QDebug>
#include <QSettings>

#include "Globals.h"
#include "Processing/RequestManager.h"
#include "Processing/ParsingProcessor.h"
#include "Modbus/ModbusSerialMaster.h"

ProcessingManager::ProcessingManager(QObject *parent) : QObject(parent)
{
	if(_serialMaster.isNull())
		_serialMaster.reset(new ModbusSerialMaster("/dev/ttyRPC0"));
	QSettings settings;
//	QList<QSharedPointer<RequestManager>> requestManagers;
	settings.beginGroup(REQUEST_GROUPS_KEY);
	foreach(QString group, settings.childGroups()) {
		qDebug() << "Processing group" << group;
		settings.beginGroup(group);
		RequestManager *rm(new RequestManager(settings, this));
		connect(rm, &RequestManager::requesting, this, &ProcessingManager::onQueryRequest);
		settings.endGroup();
	}
	settings.endGroup();
}

void ProcessingManager::onQueryRequest() {
	RequestManager *rm(static_cast<RequestManager*>(sender()));
	qDebug() << "PROCESSING" << rm->objectName() << ":" << rm;
	ADUSharedPtr_t req(new ApplicationDataUnitSerial(rm->device(), rm->request()));
	qDebug() << "\tREQUEST: " << req->toHex();
	ADUSharedPtr_t response(_serialMaster->process(req));
	if(response.isNull()) {
		qDebug() << "\tNULL RESPONSE!";
		rm->onResponse(response);
	}
	else {
//		qDebug() << "\tRESPONSE: " << response->toHex();
		rm->onResponse(response);
	};
}

QSharedPointer<ParsingProcessor> ProcessingManager::processor(QSettings *settings)
{
	if(settings->value(REQUEST_PARSING_TYPE_KEY) == xstr(REQUEST_PARSING_TYPE_VALUE_POST))
		return QSharedPointer<ParsingProcessor>(new PostParsingProcessor(settings));

	return QSharedPointer<ParsingProcessor>();
}

QSharedPointer<class ModbusSerialMaster> ProcessingManager::_serialMaster;
