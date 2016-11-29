#ifndef RESETPARSINGPROCESSOR_H
#define RESETPARSINGPROCESSOR_H

#include <QRegularExpression>

#include "Processing/ParsingProcessor.h"

class RestartParsingProcessor : public ParsingProcessor
{
	Q_OBJECT

public:
	RestartParsingProcessor(QSettings *settings);
	virtual ~RestartParsingProcessor() {}
	virtual bool isValid() const;
	virtual void process(class RequestManager *);

private:
	QRegularExpression _dateTimeRegExp;
	int _memorySize,
		_memoryPercentage; // stored as absolute (recomputed!) memory limit [bytes]

};

#endif // RESETPARSINGPROCESSOR_H
