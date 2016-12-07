#include "WatchdogController.h"

#include <QSettings>

#include "Debug/DebugMacros.h"
#include "Globals.h"
#include "System/GpioPin.h"
#include "System/Reset.h"

WatchdogController::WatchdogController(QObject *parent) :
	QObject(parent)
{
	QSettings settings;

	int pin(settings.value(WATCHDOG_GROUP_KEY "/" WATCHDOG_HEARTBEAT_GPIO_KEY, QVariant(-1)).toInt());
	if(pin > 0 ) {
		_watchdogGpio.reset(new GpioPin(pin));
		if(!_watchdogGpio->isOutput()) {
			_watchdogGpio.reset();
		}
	}

	pin = settings.value(WATCHDOG_GROUP_KEY "/" WATCHDOG_POWERDOWN_GPIO_KEY, QVariant(-1)).toInt();
	if(pin > 0 ) {
		_powerdownGpio.reset(new GpioPin(pin));
		if(!_powerdownGpio->isInput()) {
			_powerdownGpio.reset();
		}
	}

	int period(settings.value(WATCHDOG_GROUP_KEY "/" WATCHDOG_HEARTBEAT_PERIOD_KEY,
							  WATCHDOG_HEARTBEAT_PERIOD_DEFAULT).toInt());

	if((!_watchdogGpio.isNull()) || (!_powerdownGpio.isNull())) {

		DP_INIT("\nStarting watchdog with period" << period << ":");
		if(!_watchdogGpio.isNull())
			DP_INIT("\tHeartbeat on pin" << _watchdogGpio->nr());

		if(!_watchdogGpio.isNull())
			DP_INIT("\tPoweroff detection on pin" << _powerdownGpio->nr());


		connect(&_timer, &QTimer::timeout, this, &WatchdogController::onTimer);
		_timer.start(period);
	}
}

void WatchdogController::onTimer()
{
//	CHECKPOINT("BOOL" << _currentState << (!_currentState));
	if(_currentState)
		_currentState = false;
	else
		_currentState = true;

	if(!_watchdogGpio.isNull())
		_watchdogGpio->setValue(_currentState);

	if((!_powerdownGpio.isNull()) && (_powerdownGpio->read() == 0)) {
		CHECKPOINT("Powerdown ACTIVE");
		System::resetInitiate(System::powerdownReset,"Watchdog powerdown reset");
	}



//	CHECKPOINT("Heartbeat" << _currentState);
}
