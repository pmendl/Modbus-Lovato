#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QSharedPointer>
#include <QSettings>

class ParsingProcessor
{
public:
	virtual ~ParsingProcessor() {}
	virtual void process(class RequestManager *rm) = 0;
};

class PostParsingProcessor : public ParsingProcessor
{
public:
	PostParsingProcessor(QSettings *settings);
	virtual ~PostParsingProcessor() {}
	virtual void process(class RequestManager *rm);

private:

};

#endif // PARSINGPROCESSOR_H
