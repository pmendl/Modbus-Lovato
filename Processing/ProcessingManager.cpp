#include "ProcessingManager.h"

#include<QCoreApplication>
#include "DebugMacros.h"
#include <QSettings>
#include <QDir>

#include "Globals.h"
#include "Processing/RequestManager.h"
#include "Processing/AllParsingProcessors.h"
#include "Modbus/ModbusSerialMaster.h"

ProcessingManager::ProcessingManager(QObject *parent, bool suppressPeriodicalRequesting) :
	QObject(parent),
	_serialMaster("/dev/ttyRPC0"),
	_logServer(new LogServer(REQUEST_PARSING_LOG_PATH_DEFAULT))
{
	setSuppressPeriodicalRequesting(suppressPeriodicalRequesting);

	QSettings settings;

	settings.beginGroup(REQUEST_GROUPS_KEY);
	foreach(QString group, settings.childGroups()) {
		DP_PROCESSING_INIT("Processing group" << group);
		settings.beginGroup(group);
		RequestManager *rm(new RequestManager(settings, this));
		connect(rm, &RequestManager::requesting, this, &ProcessingManager::onQueryRequest);
		settings.endGroup();
	}
	settings.endGroup();
}

void ProcessingManager::onQueryRequest() {
	// THIS ONE EVENT HANDLER IS SPECIAL in that ti does NOT provide start/end debug prints
	// (as it is called too often and produces many other prints)

	// TESTING ONLY - managed by setting Test\SuppressPeriodicalRequesting=true in .INI file
	if (_suppressPeriodicalRequesting)
		return;

	RequestManager *rm(static_cast<RequestManager*>(sender()));
	DP_PROCESSING_REQUEST("PROCESSING" << rm->objectName() << ":" << rm);
	ADUSharedPtr_t req(new ApplicationDataUnitSerial(rm->device(), rm->request()));
	DP_PROCESSING_REQUEST("\tREQUEST: " << req->toHex());
	ADUSharedPtr_t response(_serialMaster.process(req));
	if(response.isNull())
		DP_PROCESSING_REQUEST("\tNULL RESPONSE!");
	// Command 0x03 hardwired for now; can get reimplemented more flexible later
	else if(response->extractAt<char>(0) != 0x03) {
			DP_PROCESSING_REQUEST("\tERROR RESPONSE!");
	};
	rm->onResponse(response);
}

bool ProcessingManager::suppressPeriodicalRequesting() const
{
	return _suppressPeriodicalRequesting;
}

void ProcessingManager::setSuppressPeriodicalRequesting(bool suppressPeriodicalRequesting)
{
	_suppressPeriodicalRequesting = suppressPeriodicalRequesting;
}

QSharedPointer<LogServer> ProcessingManager::logServer() const
{
	return _logServer;
}

QSharedPointer<ParsingProcessor> ProcessingManager::processor(QSettings *settings, QString group)
{
	QSharedPointer<ParsingProcessor> p;
	DP_PROCESSING_INIT("\tConstructing ParsingProcessor of type" << settings->value(REQUEST_PARSING_TYPE_KEY));
	if(settings->value(REQUEST_PARSING_TYPE_KEY) == xstr(REQUEST_PARSING_TYPE_VALUE_POST))
		p.reset(new PostParsingProcessor(settings, group));
	else if(settings->value(REQUEST_PARSING_TYPE_KEY) == xstr(REQUEST_PARSING_TYPE_VALUE_LOG))
		p.reset(new LogParsingProcessor(settings, group, _logServer));

	if(!p.isNull() && p->isValid()) {
		DP_PROCESSING_INIT("\t\tProcessingManager::processor returns" << p);
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
