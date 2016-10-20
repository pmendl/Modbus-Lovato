#include "PostParsingProcessor.h"

#include <QProcess>
#include "Debug/DebugMacros.h"
#include <QSettings>
#include HTTP_MULTI_PART_INCLUDE
#include <QNetworkReply>
#include <QDateTime>

#include "Network/DebugHttpMultiPart.h"
#include "Globals.h"
#include "Processing/RequestManager.h"
#include "Processing/ProcessingManager.h"

PostParsingProcessor::PostParsingProcessor(QSettings *settings, QString group, quint64 timeout) :
	_url(NetworkSender::parseUrl(settings->value(REQUEST_PARSING_POST_URL_KEY).toString())),
	_inProcess(false),
	_priority(nullRequestPriority),
	_delayedCount(0),
	_timeout(settings->value(REQUEST_PARSING_POST_TIMEOUT_KEY, timeout).toUInt())
{
	setObjectName(ProcessingManager::objectNameFromGroup(POST_PARSING_PROCESSOR_PREFIX, group));
	setOccurance(settings);
	DP_NET_POSTING_INIT("\t\tParsingProcessor will post to" << _url.url());
	connect(&_sender, &NetworkSender::multipartSent, this, &PostParsingProcessor::onMultipartSent);
}

bool PostParsingProcessor::isValid() const
{
	return _url.isValid();
}

void PostParsingProcessor::process(RequestManager *rm)
{
	if(nextOccurance())
		return;

	requestPriority_t priority = normalRequestPriority;
	if(rm->parsedItems().contains(PARSED_ITEM_ERROR_RESPONSE_CODE_KEY))
		priority = errorRequestPriority;
	else
#ifdef NO_RESPONSE_TYPE_KEY_USED
		if(rm->parsedItems().contains(PARSED_ITEM_RESPONSE_TYPE_KEY))
#else
		if(rm->parsedItems().value(PARSED_ITEM_RESPONSE_TYPE_KEY,
								   RequestManager::parsedItem_t()).value
				== PARSED_ITEM_RESPONSE_TYPE_NULL_RESPONSE_VALUE)
#endif
			priority = nullRequestPriority;

	if(priority < _priority)
		return;

	_priority = priority;

	// Adapted from http://doc.qt.io/qt-5/qhttpmultipart.html#details
	HTTP_MULTI_PART_USED *multiPart(new HTTP_MULTI_PART_USED(QHttpMultiPart::FormDataType));

	multiPart->appendFormData("groupName", rm->groupName());
	multiPart->appendFormData("responseTime", QDateTime::currentDateTimeUtc());
	for ( RequestManager::parsedItem_t item : rm->parsedItems()) {
		multiPart->appendFormData(item.def->name, item.value);
	}
	// Adapted code end

	if(_inProcess) {
		++_delayedCount;
		DP_DELAYED_COUNT("RequestManager" << rm << ": _delayedCount=" << _delayedCount);
		return;
	}

	_inProcess = true;

	if(_delayedCount > 0) {
		multiPart->appendFormData("delayedCount", QString(QStringLiteral("%1").arg(_delayedCount)));
		_delayedCount = 0;
	}

	_multipart = multiPart;
	_sender.send(_url, multiPart);
}

 void PostParsingProcessor::onMultipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply) {
	 DP_EVENTS_START()
	 if(multiPart != _multipart) {
		 DP_EVENTS_END("multiPart != _multipart")
		 return;
	 }

	 connect(reply, &QNetworkReply::finished, this, &PostParsingProcessor::onReplyFinished);
	 _multipart=0;
	 DP_EVENTS_END("End")
 }

 void PostParsingProcessor::onReplyFinished() {
	 DP_EVENTS_START()
	 QNetworkReply *reply(static_cast<QNetworkReply *>(sender()));
	 {
		 _inProcess = false;
		 _priority = nullRequestPriority;
	 }
	 DP_EVENTS_END("End")
}
