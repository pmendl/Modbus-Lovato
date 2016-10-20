#ifndef MEMORYANALYTICS_H
#define MEMORYANALYTICS_H

namespace Debug {
	int getMemory();
	void setMemoryRef();
	bool checkRef(int limit);
	void printMemory();
} // Debug

#endif // MEMORYANALYTICS_H
