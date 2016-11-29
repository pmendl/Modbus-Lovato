#include "Reset.h"

#include <QCoreApplication>

#include <cstdlib>

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "System/PrioritiesCountingHash.h"
#include "System/TrueCandidates.h"
#include "Network/ExtendedHttpMultiPart.h"
#include "Network/NetworkSender.h"


namespace System {

PrioritiesCountingHash resetBlockers;
bool _resetInProgress(false);


void resetInitiate(QString reason) {
	InitiateResetEvent event(reason);
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


InitiateResetEvent::InitiateResetEvent(QString reason) :
	QEvent(static_cast<QEvent::Type>(System::initiateResetEventType)),
	_reason(reason)
{}

InitiateResetEventFilter::InitiateResetEventFilter() :
	_sender(0)
{
	QSettings settings;
	QUrl url=(NetworkSender::parseUrl(settings.value(RESET_GROUP_KEY "/" RESET_NOTIFICATION_URL_KEY).toString()));
	if(url.isValid())
		_sender = new NetworkSender(url);
}

bool InitiateResetEventFilter::eventFilter(QObject *, QEvent *event)
{
	if(event->type() == System::initiateResetEventType) {
		QString reason(static_cast<InitiateResetEvent *>(event)->_reason);

		System::_resetInProgress=true;
		DP_RESET("RESET INITED: reason=" << reason);

		ExtendedHttpMultiPart multipart;
		multipart.appendFormData(QStringLiteral(POST_ELEMENT_RESET_INIT_KEY), reason);
		MARK(RESET_GROUP_KEY "/" RESET_NOTIFICATION_URL_KEY "="
			 << _sender->defaultSlotUrl().url());
		if(_sender) {
			MARK("RESET NOTIFICATION:" << reason  << _sender->defaultSlotUrl().url());
			_sender->sendMultipart(&multipart);
		}
	}
	return false;
}


