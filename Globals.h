#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSharedPointer>

// --- Timings ---
#define MODBUS_MAXRETRIES_DEFAULT	5
#define MODBUS_INITIALREADTIMEOUT_DEFAULT	100
#define MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT	5

// --- Settings keys ---
#define REQUEST_GROUPS "RequestGroups"
#define REQUEST_ACTIVITY "active"

// --- Object names/prefixes ---
#define REQUEST_MANAGER_NAME_PREFIX "RequestManagerOf"


typedef QSharedPointer<class ProtocolDataUnit> PDUSharedPtr_t;
typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

#endif // GLOBALS_H
