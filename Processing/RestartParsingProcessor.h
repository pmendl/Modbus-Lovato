#ifndef RESTARTPARSINGPROCESSOR_H
#define RESTARTPARSINGPROCESSOR_H

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
	QRegularExpression _regexp;
};

#endif // RESTARTPARSINGPROCESSOR_H
