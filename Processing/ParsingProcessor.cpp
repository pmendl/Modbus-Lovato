#include "ParsingProcessor.h"

#include <QDebug>
#include <QHttpMultiPart>

#include "Globals.h"
#include "Processing/RequestManager.h"

PostParsingProcessor::PostParsingProcessor(QSettings *settings) :
	_url(settings->value(REQUEST_PARSING_POST_URL_KEY).toString()),
	_reply(0),
	_delayedCount(0)
{
	setObjectName(POST_PARSING_PROCESSOR_NAME);
	if(!_url.isValid())
		_url = QUrl::fromUserInput(settings->value(REQUEST_PARSING_POST_URL_KEY).toString());
}

bool PostParsingProcessor::isValid()
{
	return _url.isValid();
}

void PostParsingProcessor::process(RequestManager *rm)
{

	if(_reply) {
		qDebug() << "@@@ STILL WAITING FOR RESPONSE try #" << ++_delayedCount;

		requestPriority_t priority = normalRequestPriority;
		if(rm->parsedItems().contains(PARSED_ITEM_ERROR_RESPONSE_CODE_KEY))
			priority = errorRequestPriority;
		else if(rm->parsedItems().value(PARSED_ITEM_RESPONSE_TYPE_KEY,
										RequestManager::parsedItem_t()).value
				== PARSED_ITEM_RESPONSE_TYPE_NULL_RESPONSE_VALUE)
			priority = errorRequestPriority;


	}


	// Adapted from http://doc.qt.io/qt-5/qhttpmultipart.html#details
	QHttpMultiPart *multiPart(new QHttpMultiPart(QHttpMultiPart::FormDataType));

	QHttpPart textPart;
	for ( RequestManager::parsedItem_t item : rm->parsedItems()) {
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QVariant(QString(QStringLiteral("form-data; name="))+item.def->name));
		textPart.setBody(item.value.toString().toUtf8());
		_multiPart->append(textPart);
	}
	// Adapted code end


	_multiPart.reset(multiPart);
}

/* INTO onFinished
			qDebug() << "@@@ SENT WITH RESULT" << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
#warning Consider more sophisticated processing here
			_reply->deleteLater();
			_reply = 0;
*/

/* INTO	onTimer
qDebug() << "@@@ NETWORK SENDING TIMEOUT - ABORTING !!!";
_reply->abort();
_reply->deleteLater();
_reply = 0;
return;
*/
