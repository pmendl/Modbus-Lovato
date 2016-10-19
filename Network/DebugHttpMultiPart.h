#ifndef DEBUGHTTPMULTIPART_H
#define DEBUGHTTPMULTIPART_H

#include "Globals.h"
#include HTTP_MULTI_PART_INCLUDE

class DebugHttpMultiPart : public QHttpMultiPart
{
public:
	explicit DebugHttpMultiPart(QObject *parent = 0);
	explicit DebugHttpMultiPart(ContentType contentType, QObject *parent = 0);
	~DebugHttpMultiPart();

private:
	static int _instanceCount;
};

#endif // DEBUGHTTPMULTIPART_H
