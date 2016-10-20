#ifndef MEMORYANALYTICS_H
#define MEMORYANALYTICS_H

namespace Debug {
	int snapMemory();
	void setMemoryRef();
	int diffLast();
	int diffRef();
	bool printAllwaysCriterion(int mem, int lastMem, int refMem);
	void printMemory(bool snap = true);
} // Debug

#endif // MEMORYANALYTICS_H
