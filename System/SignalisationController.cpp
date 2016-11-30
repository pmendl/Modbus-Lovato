#include "SignalisationController.h"

#include <QSettings>

#include "Debug/DebugMacros.h"
#include "Globals.h"
#include "System/GpioPin.h"

SignalisationController::SignalisationController(QObject *parent) : QObject(parent)
{
	QSettings settings;

	int pin(settings.value(SIGNALISATION_GROUP_KEY "/" SIGNALISATION_HEARTBEAT_GPIO_KEY, QVariant(-1)).toInt());
	if(pin <= 0 ) return;

	_httpGpio.reset(new GpioPin(pin));
	if(!_httpGpio->isOutput()) {
		_httpGpio.reset();
		return;
	}

	DP_INIT("\nStarting HTTP signalization on pin" << pin << ".");
}

void SignalisationController::setHttpStatus(bool status) {
	if(!_httpGpio.isNull()) {
		_httpGpio->setValue(status);
		MARK("HTTP status:" << status);
	}
}

QSharedPointer<GpioPin> SignalisationController::_httpGpio;
