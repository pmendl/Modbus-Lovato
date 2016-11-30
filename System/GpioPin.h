#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <QFile>

/**
 * @brief The GpioPin class
 *
 * This class allows to read or set GPIO pin via kernel pseudodevice in various ways.
 * @todo For historical reason expects, that direction is fix set by system at boottime.
 * Should be extended by setting direction, if published to public domain.
 */
class GpioPin
{
public:
	GpioPin(int nr);

	bool isDirection(const char *text);
	bool isOutput(void);
	bool isInput(void);
	bool setValue(int value);
	bool setValue(bool value);
	bool setOn(void);
	bool setOff(void);
	int read(void);
	bool read(bool);
	int nr(void);

protected:
	bool open(QFile *file, const QString pathPattern, const QIODevice::OpenMode mode);

	int _nr;
};

#endif // GPIOPIN_H
