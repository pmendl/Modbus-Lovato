#include "Debug/MemoryAnalytics.h"

#include <QProcess>

#include "Debug/DebugMacros.h"
namespace Debug {

int actualMem, lastMem, refMem;
int eventIndex = 0;

int snapMemory()
{
	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $2 }" << "/proc/meminfo");
	p.waitForFinished();
	actualMem = QString::fromUtf8(p.readLine()).toInt();
	p.close();
	return actualMem;
}

void setMemoryRef()
{
//	D_P("---- Memory ref");
	refMem=snapMemory();
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

// --------------- VERY SPECIAL FUNCTIONS FOR FOCUS ON OBSERVED EVENT ------------------

bool eventPrintFlag;

bool checkPrint(QString q_func_info) {
	if(++eventIndex<100) {
		qDebug() << eventIndex;
		return false;
	}
	return eventPrintFlag = (q_func_info == "void RequestManager::onResponse(PDUSharedPtr_t)");
}


} // namespace
