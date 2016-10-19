#ifndef MEMORYPARSINGPROCESSOR_H
#define MEMORYPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"

class MemoryParsingProcessor : public ParsingProcessor
{
	Q_OBJECT

public:
	virtual ~MemoryParsingProcessor() {}
	virtual bool isValid() const;
	virtual void process(class RequestManager *);
};

#endif // MEMORYPARSINGPROCESSOR_H
