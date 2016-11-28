#include "Reset.h"

#include <QCoreApplication>
#include <QSettings>

#include <cstdlib>

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "System/PrioritiesCountingHash.h"
#include "System/TrueCandidates.h"


namespace System {

PrioritiesCountingHash resetBlockers;
bool _resetInProgress(false);

void resetInitiate(void) {
	InitiateResetEvent event;
	qApp->sendEvent(qApp, &event);
}

void resetExecute(void)	{
	DP_RESET("RESET EXECUTE");

	QSettings settings;
	if(!trueCandidates.contains(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_RESET_KEY)).toString().toLower())) {
		system("sudo reboot");
		exit(1);
	}
}

void resetEnforce(void) {
	DP_RESET_DETAILS("RESET ENFORCE");
	resetExecute();
}

bool startResetSensitiveProcess(int priority) {
	if((!_resetInProgress) || resetBlockers.maxPriority() > priority) {
		resetBlockers.startPriority(priority);
		if(_resetInProgress)
			DP_RESET_SHOW_BLOCKERS(resetBlockers);
		return true;
	}
	return false;
}

void endResetSensitiveProcess(int priority) {
	resetBlockers.endPriority(priority);
	if(_resetInProgress) {
		DP_RESET_SHOW_BLOCKERS(resetBlockers);
		if(resetBlockers.isEmpty())
			resetExecute();
	}
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
		DP_RESET("RESET INITED");
	}
	return false;
}


