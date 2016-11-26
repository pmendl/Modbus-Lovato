#include "Reset.h"

#include "System/PrioritiesCountingHash.h"

namespace System {

PrioritiesCountingHash resetBlockers;
bool _resetInProgress(false);

void initiateReset(void) {
	_resetInProgress=true;
}

bool startResetSensitiveProcess(int priority) {
	if((!_resetInProgress) || resetBlockers.maxPriority() > priority) {
		resetBlockers.startPriority(priority);
		return true;
	}
	return false;
}

void endResetSensitiveProcess(int priority) {
	resetBlockers.endPriority(priority);
}

} // System

