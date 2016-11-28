#include "Reset.h"

#include <QCoreApplication>

#include "Debug/DebugMacros.h"
#include "System/PrioritiesCountingHash.h"


namespace System {

PrioritiesCountingHash resetBlockers;
bool _resetInProgress(false);

void resetInitiate(void) {
	InitiateResetEvent event;
	qApp->sendEvent(qApp, &event);
}

void resetExecute(void)	{
	MARK("RESET EXECUTE");
	exit(1);
}

void resetEnforce(void) {
	MARK("RESET ENFORCE");
	resetExecute();
}

bool startResetSensitiveProcess(int priority) {
	if((!_resetInProgress) || resetBlockers.maxPriority() > priority) {
		resetBlockers.startPriority(priority);
		MARK(resetBlockers);
		return true;
	}
	return false;
}

void endResetSensitiveProcess(int priority) {
	resetBlockers.endPriority(priority);
	MARK(resetBlockers);
	if(_resetInProgress && resetBlockers.isEmpty())
		resetExecute();
}

const int initiateResetEventType(QEvent::registerEventType());
const int executeResetEventType(QEvent::registerEventType());


} // System


InitiateResetEvent::InitiateResetEvent() :
	QEvent(static_cast<QEvent::Type>(System::initiateResetEventType))
{}

bool InitiateResetEventFilter::eventFilter(QObject *, QEvent *event) {
	if(event->type() == System::initiateResetEventType) {
		System::_resetInProgress=true;
		MARK("RESET INITED");
	}
	return false;
}


