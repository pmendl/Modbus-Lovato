#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSharedPointer>

#define MODBUS_MAXRETRIES_DEFAULT	5
#define MODBUS_INITIALREADTIMEOUT_DEFAULT	100
#define MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT	5

typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

#endif // GLOBALS_H
