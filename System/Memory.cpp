#include "System/Memory.h"

#include <QProcess>

namespace System {

int getMemory()
{
	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $2 }" << "/proc/meminfo");
	p.waitForFinished();
	int mem(QString::fromUtf8(p.readLine()).toInt());
	p.close();
	return mem;
}

} // System
