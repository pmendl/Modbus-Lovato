#include "MemoryParsingProcessor.h"

#include "Debug/DebugMacros.h"
#include <QProcess>

#include "Globals.h"
#include "Debug/MemoryAnalytics.h"
#include "Processing/RequestManager.h"
#include "Network/ExtendedHttpMultiPart.h"
#include "Debug/InstanceCounterBase.h"

/// @file

/// @class MemoryParsingProcessor
/// @brief Pseudoparser used for reporting free system memory
///
/// In fact usefull only for debugging. Set to some reasonable period to check,
/// if system lockups are not due to some memory leakage.

MemoryParsingProcessor::MemoryParsingProcessor(QSettings *settings) :
	ParsingProcessor(settings)
{}

bool MemoryParsingProcessor::isValid() const {
	return true;
}

void MemoryParsingProcessor::process(RequestManager *){
	if(nextOccurance()) {
		return;
	}

	ExtendedHttpMultiPart::appendToGlobalData("memFree", Debug::printMemory());
	InstanceCounterBase::print();
}
