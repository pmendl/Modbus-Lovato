#include "DebugHttpMultiPart.h"

#include <QProcess>
#include <QDebug>


DebugHttpMultiPart::DebugHttpMultiPart(QObject *parent) :
	QHttpMultiPart(parent)
{
	qDebug() << "*** Constructor INSTANCE COUNT =" << ++_instanceCount << this;
}

DebugHttpMultiPart::DebugHttpMultiPart(ContentType contentType, QObject *parent) :
QHttpMultiPart(contentType, parent)
{
	qDebug() << "*** Constructor INSTANCE COUNT =" << ++_instanceCount << this;

}

DebugHttpMultiPart::~DebugHttpMultiPart()
{
	qDebug() << "*** Destructor INSTANCE COUNT =" << --_instanceCount << this;

	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $0 }" << "/proc/meminfo");
	p.waitForFinished();
	qDebug() << "***" << p.readAllStandardOutput();
	p.close();


//	this->QHttpMultiPart::~QHttpMultiPart();
	qDebug() << "\t*** Destructor finished on" << this;
}

int DebugHttpMultiPart::_instanceCount = 0;
