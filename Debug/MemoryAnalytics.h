#ifndef MEMORYANALYTICS_H
#define MEMORYANALYTICS_H

namespace Debug {
	int getMemory();
	int setMemoryRef(bool doRead = true);
	int printMemory(bool doRead = true);
} // Debug

#endif // MEMORYANALYTICS_H
