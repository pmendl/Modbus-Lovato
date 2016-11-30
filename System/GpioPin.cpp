#include "GpioPin.h"

#include <QDebug>

GpioPin::GpioPin(int nr) :
	_nr(nr)
{}

bool GpioPin::isDirection(const char *text) {
	QFile file;
	if(!open(&file, "/sys/class/gpio/gpio%1/direction", QIODevice::ReadOnly))
		return false;
	QByteArray direction(file.readAll());
	file.close();
	return direction.startsWith(text);
}

bool GpioPin::isOutput(void) {
	return isDirection("out");
}

bool GpioPin::isInput(void) {
	return isDirection("in");
}

bool GpioPin::setValue(int value) {
	return setValue(value != 0);
}

bool GpioPin::setValue(bool value) {
	QFile file;
	if(!open(&file, "/sys/class/gpio/gpio%1/value", QIODevice::WriteOnly))
		return false;
	if(file.write( value ? "1" : "0") != 1) {
		file.close();
		return false;
	}
	file.close();
	return true;
}

bool GpioPin::setOn(void) {
	return setValue(true);
}

bool GpioPin::setOff(void) {
	return setValue(false);
}

int GpioPin::read(void) {
	return read(true) ? 1 : 0;
}

bool GpioPin::read(bool) {
	QFile file;
	if(!open(&file, "/sys/class/gpio/gpio%1/value", QIODevice::ReadOnly))
		return false;
	QByteArray value(file.readAll());
	file.close();
	return value.startsWith("1");
}

int GpioPin::nr(void) {
	return _nr;
}

bool GpioPin::open(QFile *file, const QString pathPattern, const QIODevice::OpenMode mode) {
	file->setFileName(pathPattern.arg(_nr));
	if (!file->open(mode)) {
		qDebug() << (QString(QStringLiteral("GPIO: pin %1 open failed")).arg(_nr));
		return false;
	}
	return true;
}
