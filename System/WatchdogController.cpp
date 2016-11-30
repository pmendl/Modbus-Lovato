#include "WatchdogController.h"

#include <QSettings>

#include "Debug/DebugMacros.h"
#include "Globals.h"
#include "System/GpioPin.h"

WatchdogController::WatchdogController(QObject *parent) :
	QObject(parent)
{
	QSettings settings;

	int pin(settings.value(WATCHDOG_GROUP_KEY "/" WATCHDOG_HEARTBEAT_GPIO_KEY, QVariant(-1)).toInt());
	if(pin <= 0 ) return;

	_watchdogGpio.reset(new GpioPin(pin));
	if(!_watchdogGpio->isOutput()) {
		_watchdogGpio.reset();
		return;
	}

	int period(settings.value(WATCHDOG_GROUP_KEY "/" WATCHDOG_HEARTBEAT_PERIOD_KEY,
							  WATCHDOG_HEARTBEAT_PERIOD_DEFAULT).toInt());

	DP_INIT("\nStarting watchdog on pin" << pin << "with period" << period << ".");

	connect(&_timer, &QTimer::timeout, this, &WatchdogController::onTimer);
	_timer.start(period);
}

void WatchdogController::onTimer()
{
//	CHECKPOINT("BOOL" << _currentState << (!_currentState));
	if(_currentState)
		_currentState = false;
	else
		_currentState = true;

	_watchdogGpio->setValue(_currentState);
//	CHECKPOINT("Heartbeat" << _currentState);
}
