#include "Debug/MemoryAnalytics.h"


#include "Debug/DebugMacros.h"
#include "System/Memory.h"

namespace Debug {

int lastMem;
int refMem;

using namespace Debug;

int setMemoryRef(bool doRead)
{
	return refMem=System::getMemory();
}

int printMemory(bool doRead)
{
	int mem;
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
