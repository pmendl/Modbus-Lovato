#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QSharedPointer>
#include <QSettings>



class PostParsingProcessor;

class ParsingProcessor
{
public:
	virtual ~ParsingProcessor() {}
	virtual void process(class RequestManager *rm) = 0;

	static QSharedPointer<ParsingProcessor> processor(QString type, QSettings *settings);
};

class PostParsingProcessor : public ParsingProcessor
{
public:
	PostParsingProcessor(QSettings *settings);
	void process(class RequestManager *rm);
};

#endif // PARSINGPROCESSOR_H
