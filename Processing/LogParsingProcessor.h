#ifndef LOGPARSINGPROCESSOR_H
#define LOGPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"
#include "Log/LogServer.h"

class LogParsingProcessor : public ParsingProcessor
{
	Q_OBJECT

public:
	LogParsingProcessor(class QSettings *settings, QString group, QSharedPointer<class LogServer> logServer);
	virtual ~LogParsingProcessor() {}
	virtual bool isValid() const;
	virtual void process(class RequestManager *rm);

private:
	QSharedPointer<class LogServer>_logServer;
	QString _logName;
};

#endif // LOGPARSINGPROCESSOR_H
