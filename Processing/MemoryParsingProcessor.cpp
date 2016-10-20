#include "MemoryParsingProcessor.h"

#include "Debug/DebugMacros.h"
#include <QProcess>

#include "Globals.h"
#include "Processing/RequestManager.h"
//#include "Processing/ProcessingManager.h"

/// @file

/// @class MemoryParsingProcessor
/// @brief Pseudoparser used for reporting free system memory
///
/// In fact usefull only for debugging. Set to some reasonable period to check,
/// if system lockups are not due to some memory leakage.

bool MemoryParsingProcessor::isValid() const {
	return true;
}

void MemoryParsingProcessor::process(RequestManager *){
	if(nextOccurance())
		return;

	Debug::printMemory();
}
