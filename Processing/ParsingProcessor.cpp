#include "ParsingProcessor.h"

#include <QSettings>

#include "Globals.h"

void ParsingProcessor::setOccurance(QSettings *settings)
{
	_occurance = settings->value(REQUEST_PARSING_OCCURANCE_KEY, REQUEST_PARSING_OCCURANCE_DEFAULT).toUInt();
	_actualOccurance = 0;
}

bool ParsingProcessor::nextOccurance() {
	if(++_actualOccurance < _occurance)
		return true;
	_actualOccurance = 0;
	return false;
}

quint32 ParsingProcessor::occurance() const
{
	return _occurance;
}

quint32 ParsingProcessor::actualOccurance() const
{
return _actualOccurance;
}
