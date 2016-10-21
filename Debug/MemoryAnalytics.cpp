#include "Debug/MemoryAnalytics.h"

#include <QProcess>

#include "Debug/DebugMacros.h"
namespace Debug {

int actualMem, lastMem, refMem;

int snapMemory()
{
	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $2 }" << "/proc/meminfo");
	p.waitForFinished();
	actualMem = QString::fromUtf8(p.readLine()).toInt();
	p.close();
	return actualMem;
}

void setMemoryRef(bool snap)
{
	if(snap) {
		refMem = snapMemory();
	}
	else {
		refMem =  actualMem;
	}
}

int diffLast()
{
	return actualMem - lastMem;
}

int diffRef()
{
	return actualMem - refMem;
}

void printMemory(bool snap)
{
	if (snap) {
		snapMemory();
	}
	if(refMem < 0) {
		D_P("--------- Memory change:" << diffLast() << "\t(MemFree:" << actualMem << "Kb) ---------");
	}
	else {
		D_P("--------- Memory change:" << diffLast() << "\tRef change:" << diffRef() << "\t(MemFree:" << actualMem << "Kb) ---------");
	}
	lastMem=actualMem;
	refMem=-1;
}

} // namespace
