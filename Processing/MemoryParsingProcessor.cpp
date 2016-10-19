#include "MemoryParsingProcessor.h"

#include "DebugMacros.h"
#include <QProcess>

#include "Globals.h"
#include "Processing/RequestManager.h"
//#include "Processing/ProcessingManager.h"

/// @file

/// @class LogParsingProcessor
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

	QProcess p;
	p.start("awk", QStringList() << "/MemFree/ { print $0 }" << "/proc/meminfo");
	p.waitForFinished();
	D_P("***" << p.readAllStandardOutput());
	p.close();
}
