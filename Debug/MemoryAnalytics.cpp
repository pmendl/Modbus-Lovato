#include "Debug/MemoryAnalytics.h"


#include "Debug/DebugMacros.h"
#include "System/Memory.h"

namespace Debug {

int lastMem;
int refMem;

using namespace Debug;

int setMemoryRef(bool doRead)
{
	if(doRead)
		return refMem=System::getMemory();
	else
		return refMem=lastMem;
}

int printMemory(bool doRead)
{
	int mem(lastMem);
	if(doRead) {
		mem=System::getMemory();
	}
	if(refMem < 0) {
		D_P("--------- Memory change:" << (mem - lastMem) << "(MemFree:" << mem << "Kb) ---------");
	}
	else {
		D_P("--------- Memory change:" << (mem - lastMem) << "Ref change:" << (mem-refMem) << "(MemFree:" << mem << "Kb) ---------");
	}
	refMem=-1;
	return lastMem=mem;
}

} // namespace
