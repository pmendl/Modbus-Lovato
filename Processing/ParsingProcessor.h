#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QObject>

#include "Network/NetworkAccessBase.h"

/// @file

class ParsingProcessor : public QObject
{
	Q_OBJECT

public:
	virtual ~ParsingProcessor() {}
	virtual bool isValid() = 0;
	virtual void process(class RequestManager *rm) = 0;
};

#endif // PARSINGPROCESSOR_H
