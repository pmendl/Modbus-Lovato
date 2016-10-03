#ifndef DEBUGHTTPMULTIPART_H
#define DEBUGHTTPMULTIPART_H

#include <QHttpMultiPart>

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
