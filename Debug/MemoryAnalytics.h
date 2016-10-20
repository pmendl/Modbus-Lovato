#ifndef MEMORYANALYTICS_H
#define MEMORYANALYTICS_H

#include <QString>

namespace Debug {
	int snapMemory();
	void setMemoryRef();
	int diffLast();
	int diffRef();
	bool printAllwaysCriterion(int mem, int lastMem, int refMem);
	void printMemory(bool snap = true);

	extern int eventIndex;


	// --------------- VERY SPECIAL FUNCTIONS FOR FOCUS ON OBSERVED EVENT ------------------

	extern bool eventPrintFlag;

	bool checkPrint(QString q_func_info);

} // Debug

#endif // MEMORYANALYTICS_H
