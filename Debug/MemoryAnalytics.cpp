#include "Debug/MemoryAnalytics.h"

#include <QProcess>

#include "Debug/DebugMacros.h"
namespace Debug {

int lastMem;
int refMem;

using namespace Debug;

int getMemory()
{
	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $2 }" << "/proc/meminfo");
	p.waitForFinished();
	int mem(QString::fromUtf8(p.readLine()).toInt());
	p.close();
	return mem;
}

void setMemoryRef()
{
	refMem=Debug::getMemory();
}

bool checkRef(int limit)
{
	return Debug::getMemory()-refMem < limit;
}

void printMemory()
{
	int mem(getMemory());
	if(refMem < 0) {
		D_P("--------- Memory change:" << (mem - lastMem) << "(MemFree:" << mem << "Kb) ---------");
	}
	else {
		D_P("--------- Memory change:" << (mem - lastMem) << "Ref change:" << (mem-refMem) << "(MemFree:" << mem << "Kb) ---------");
	}
	lastMem=mem;
	refMem=-1;
}

} // namespace
