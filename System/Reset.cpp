#include "Reset.h"

#include <QCoreApplication>

#include <cstdlib>

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "System/PrioritiesCountingHash.h"
#include "System/TrueCandidates.h"
#include HTTP_MULTI_PART_INCLUDE
#include "Network/NetworkSender.h"
#include "System/SignalisationController.h"
#include "Processing/ProcessingManager.h"


// Instantiated in main.h
extern QSharedPointer<ProcessingManager> processingManager;

namespace System {

PrioritiesCountingHash resetBlockers;
resetState_t _resetInProgressState(noReset);


void resetInitiate(resetState_t state, QString reason) {
	InitiateResetEvent event(state, reason);
	qApp->sendEvent(qApp, &event);
}

void resetExecute(void)	{
	const char * command("sudo reboot");
	QSettings settings;
	switch ( _resetInProgressState) {
	case noReset:
		command = "";
		break;

	case normalReset:
		if(trueCandidates.contains(settings.value(QStringLiteral(DEBUG_GROUP_KEY "/" DEBUG_SUPPRESS_RESET_KEY)).toString().toLower())) {
			command= "";
		};
		break;

	case powerdownReset:
		command = "sudo poweroff";
		break;

	default:
		break;
	}
	DP_RESET("RESET EXECUTE (" << _resetInProgressState << "):" << command);

	system(command);
	exit(1);
}

void resetEnforce(void) {
	DP_RESET_DETAILS("RESET ENFORCE");
	resetExecute();
}

bool startResetSensitiveProcess(int priority) {
	if(priority == RESET_PRIORITY_NETWORK)
		SignalisationController::setHttpStatus(true);

	if(		(!_resetInProgressState != noReset)							// No reset in progress => no problem
			|| priority == RESET_PRIORITY_NETWORK		// NETWORK CAN TRANSMIT ALLWAYS
			|| resetBlockers.maxPriority() > priority)  // While waiting for slower process faster can still start
	{
		resetBlockers.startPriority(priority);
		if(_resetInProgressState != noReset)
			DP_RESET_SHOW_BLOCKERS(resetBlockers);
		return true;
	}
	return false;
}

void endResetSensitiveProcess(int priority) {
	resetBlockers.endPriority(priority);
	if(!resetBlockers.contains(RESET_PRIORITY_NETWORK))
		SignalisationController::setHttpStatus(false);
	if(_resetInProgressState != noReset) {
		DP_RESET_SHOW_BLOCKERS(resetBlockers);
		if(resetBlockers.isEmpty())
			resetExecute();
	}
}

const int initiateResetEventType(QEvent::registerEventType());
const int executeResetEventType(QEvent::registerEventType());


} // System


InitiateResetEvent::InitiateResetEvent(System::resetState_t state, QString reason) :
	QEvent(static_cast<QEvent::Type>(System::initiateResetEventType)),
	_state(state),
	_reason(reason)
{}

InitiateResetEventFilter::InitiateResetEventFilter() :
	_sender(0)
{
	QSettings settings;
	_logName=settings.value(RESET_GROUP_KEY "/" RESET_NOTIFICATION_LOG_KEY).toString(),
									  QStringLiteral(RESET_NOTIFICATION_LOG_DEFAULT);
	QUrl url=(NetworkSender::parseUrl(settings.value(RESET_GROUP_KEY "/" RESET_NOTIFICATION_URL_KEY).toString()));

	if(url.isValid())
		_sender = new NetworkSender(url);
}

bool InitiateResetEventFilter::eventFilter(QObject *, QEvent *event)
{
	if(event->type() == System::initiateResetEventType) {
		InitiateResetEvent *resetEvent(static_cast<InitiateResetEvent *>(event));
		QString reason(resetEvent->_reason);

		System::_resetInProgressState=resetEvent->_state;
		DP_RESET("RESET INITED: reason=" << reason);

		processingManager->logServer()->log(_logName, "RESET INITED: reason="+reason);

		HTTP_MULTI_PART_USED *multipart(new HTTP_MULTI_PART_USED(QHttpMultiPart::FormDataType));
		multipart->appendFormData(QStringLiteral(POST_ELEMENT_RESET_INIT_KEY), reason);
#warning Server does NOT work with (ignores) values containing apostrophe
//		multipart->appendFormData(QStringLiteral("apostropheTest"), "O 'K'");
		if(_sender) {
			DP_RESET_DETAILS("RESET NOTIFICATION:" << reason << "->" << _sender->defaultSlotUrl().url());
			_sender->sendMultipart(multipart);
		}
	}
	return false;
}


