#ifndef LOGPARSINGPROCESSOR_H
#define LOGPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"
#include "Log/LogServer.h"

class LogParsingProcessor : public ParsingProcessor
{
	Q_OBJECT

public:
	LogParsingProcessor(class QSettings *settings, QSharedPointer<class LogServer> logServer);
	virtual ~LogParsingProcessor() {}
	virtual bool isValid();
	virtual void process(class RequestManager *rm);

private:
	QSharedPointer<class LogServer>_logServer;
};

#endif // LOGPARSINGPROCESSOR_H
