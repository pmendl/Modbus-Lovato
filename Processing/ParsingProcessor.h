#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QSharedPointer>
#include <QSettings>
#include <QUrl>

class ParsingProcessor
{
public:
	virtual ~ParsingProcessor() {}
	virtual bool isValid() = 0;
	virtual void process(class RequestManager *rm) = 0;
};

class PostParsingProcessor : public ParsingProcessor
{
public:
	PostParsingProcessor(QSettings *settings);
	virtual ~PostParsingProcessor() {}
	virtual bool isValid();
	virtual void process(class RequestManager *rm);

private:
	QUrl _url;
};

#endif // PARSINGPROCESSOR_H
