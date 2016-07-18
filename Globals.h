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

// --- RequestGroups related ---
#define REQUEST_GROUPS_KEY "RequestGroups"
#define REQUEST_ACTIVITY_KEY "active"
#define REQUEST_DEVICE_KEY "device"
#define REQUEST_ADDRESS_KEY "address"
#define REQUEST_DATA_TYPE_KEY "dataType"
#define REQUEST_BYTES_PER_ITEM "bytesPerItem"
#define REQUEST_REGISTER_COUNT "registerCount"
#define REQUEST_ARRAY_KEY "item"
#define REQUEST_ITEM_NAME_KEY "name"
#define REQUEST_ITEM_PDU_INDEX_KEY "pduIndex"
#define REQUEST_ITEM_PDU_OFFSET_KEY "pduOffset"
#define REQUEST_ITEM_MULTIPLIER_KEY "multiplier"
#define REQUEST_ITEM_DIVIDER_KEY "divider"
#define REQUEST_ITEM_SIGNUM_INDEX_KEY "signumIndex"
#define REQUEST_ITEM_
#define REQUEST_ITEM_
#define REQUEST_ITEM_


// === Object names/prefixes ===
#define REQUEST_MANAGER_NAME_PREFIX "RequestManagerOf"

// === Global datatypes ===
typedef QSharedPointer<class ProtocolDataUnit> PDUSharedPtr_t;
typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

// === Macro stringification helpers ===
#define xstr(s) str(s)
#define str(s) #s

#endif // GLOBALS_H
