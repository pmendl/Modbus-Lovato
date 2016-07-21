#include "ParsingProcessor.h"

#include <cstring>

#include "Globals.h"

QSharedPointer<ParsingProcessor> ParsingProcessor::processor(QString type, QSettings *settings)
{

	ParsingProcessor *ptr = 0;

	if(type == xstr(REQUEST_PARSING_TYPE_VALUE_POST)) {

		ptr = new PostParsingProcessor(settings);
	}

	return QSharedPointer<ParsingProcessor>(ptr);
}

void PostParsingProcessor::process(class RequestManager *rm)
{

}

PostParsingProcessor::PostParsingProcessor(QSettings *settings)
{

}
