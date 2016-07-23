#include "ParsingProcessor.h"

#include <QDebug>

#include "Globals.h"
#include "Processing/RequestManager.h"

PostParsingProcessor::PostParsingProcessor(QSettings *settings) :
	_url(settings->value(REQUEST_PARSING_POST_URL_KEY).toString())
{}

bool PostParsingProcessor::isValid()
{
	return _url.isValid();
}


void PostParsingProcessor::process(RequestManager *rm)
{
	qDebug() << "PARSING PROCESSOR processes" << rm->objectName();

}
