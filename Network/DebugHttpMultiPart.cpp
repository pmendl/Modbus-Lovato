#include "DebugHttpMultiPart.h"

#include <QProcess>
#include "DebugMacros.h"


DebugHttpMultiPart::DebugHttpMultiPart(QObject *parent) :
	QHttpMultiPart(parent)
{
	DP_DEBUGHTTPMULTIPART("*** Constructor INSTANCE COUNT =" << ++_instanceCount << this);
}

DebugHttpMultiPart::DebugHttpMultiPart(ContentType contentType, QObject *parent) :
QHttpMultiPart(contentType, parent)
{
	DP_DEBUGHTTPMULTIPART("*** Constructor INSTANCE COUNT =" << ++_instanceCount << this);

}

DebugHttpMultiPart::~DebugHttpMultiPart()
{
	DP_DEBUGHTTPMULTIPART("*** Destructor INSTANCE COUNT =" << --_instanceCount << this);

	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $0 }" << "/proc/meminfo");
	p.waitForFinished();
	DP_DEBUGHTTPMULTIPART("***" << p.readAllStandardOutput());
	p.close();


//	this->QHttpMultiPart::~QHttpMultiPart();
	DP_DEBUGHTTPMULTIPART("\t*** Destructor finished on" << this);
}

int DebugHttpMultiPart::_instanceCount = 0;
