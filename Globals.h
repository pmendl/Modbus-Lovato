#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSharedPointer>

// === Timings ===
#define MODBUS_MAXRETRIES_DEFAULT	5
#define MODBUS_INITIALREADTIMEOUT_DEFAULT	100
#define MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT	5

// === Settings keys ===
// --- Modbus related ---
#define MODBUS_GROUP_NAME "Modbus"
#define MODBUS_MAXRETRIES_KEY "MaxRetries"
#define MODBUS_INITIALREADTIMEOUT_KEY "InitialReadTimeout"
#define MODBUS_CONSEQUENTREADTIMEOUT_KEY "Modbus/ConsequentReadTimeout"

#define REQUEST_GROUPS "RequestGroups"
#define REQUEST_ACTIVITY "active"

// === Object names/prefixes ===
#define REQUEST_MANAGER_NAME_PREFIX "RequestManagerOf"

// === Global datatypes ===
typedef QSharedPointer<class ProtocolDataUnit> PDUSharedPtr_t;
typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

// === Macro stringification helpers ===
#define xstr(s) str(s)
#define str(s) #s

#endif // GLOBALS_H
