#include "ProcessingManager.h"

#include<QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QDir>

#include "Globals.h"
#include "Processing/RequestManager.h"
#include "Processing/AllParsingProcessors.h"
#include "Modbus/ModbusSerialMaster.h"

ProcessingManager::ProcessingManager(QObject *parent) :
	QObject(parent),
	_serialMaster("/dev/ttyRPC0"),
	_logServer(new LogServer(REQUEST_PARSING_LOG_PATH_DEFAULT))
{
	QSettings settings;

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
	ADUSharedPtr_t response(_serialMaster.process(req));
	if(response.isNull())
		qDebug() << "\tNULL RESPONSE!";
	// Command 0x03 hardwired for now; can get reimplemented more flexible later
	else if(response->extractAt<char>(0) != 0x03) {
			qDebug() << "\tERROR RESPONSE!";
	};
	rm->onResponse(response);
}

QSharedPointer<ParsingProcessor> ProcessingManager::processor(QSettings *settings, QString group)
{
	QSharedPointer<ParsingProcessor> p;
	qDebug() << "\tConstructing ParsingProcessor of type" << settings->value(REQUEST_PARSING_TYPE_KEY);
	if(settings->value(REQUEST_PARSING_TYPE_KEY) == xstr(REQUEST_PARSING_TYPE_VALUE_POST))
		p.reset(new PostParsingProcessor(settings, group));
	else if(settings->value(REQUEST_PARSING_TYPE_KEY) == xstr(REQUEST_PARSING_TYPE_VALUE_LOG))
		p.reset(new LogParsingProcessor(settings, group, _logServer));

	if(!p.isNull() && p->isValid()) {
		qDebug () << "\t\tProcessingManager::processor returns" << p;
		return p;
	}

	return QSharedPointer<ParsingProcessor>();
}

QString ProcessingManager::objectNameFromGroup(QString prefix, QString group) {
	if(group.isEmpty())
		return prefix;
	else
		return prefix + OBJECT_NAME_PREPOSITION + group;
}
