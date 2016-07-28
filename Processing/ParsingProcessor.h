#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QObject>

#include "Network/NetworkAccessBase.h"

/// @file

/// @warning One must ALLWAYS set occurance, even if no key present (it will assign default value of 1)
class ParsingProcessor : public QObject
{
	Q_OBJECT

public:
	virtual ~ParsingProcessor() {}
	virtual bool isValid() const = 0;
	virtual void process(class RequestManager *rm) = 0;
	void setOccurance(class QSettings *settings);
	bool nextOccurance();
	quint32 occurance() const;
	quint32 actualOccurance() const;

private:
	quint32 _occurance,_actualOccurance;

};

#endif // PARSINGPROCESSOR_H
