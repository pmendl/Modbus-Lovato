#ifndef MEMORYANALYTICS_H
#define MEMORYANALYTICS_H

#include <QString>

namespace Debug {
	int snapMemory();
	void setMemoryRef(bool snap = true);
	int diffLast();
	int diffRef();
	bool printAllwaysCriterion(int mem, int lastMem, int refMem);
	void printMemory(bool snap = true);

	extern int eventIndex;


} // Debug

#endif // MEMORYANALYTICS_H
