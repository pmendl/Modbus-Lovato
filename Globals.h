#ifndef GLOBALS_H
#define GLOBALS_H

/*
 * The following define blocks any automatic processing, leaving in action
 * only main.cpp defined responses to keypresses. It is intended for debug
 * or testing only.
 * SHOULD BE COMMENTED-OUT ALL TIMES in the production environment !!!
 */
#define NO_AUTOMATIC_PROCESSING

// === Log record types and separators ===
#define LOG_RECORD_VALUES "VALUES "
#define LOG_SEPARATOR_ITEMS "|"
#define LOG_SEPARATOR_RECORD "\n"

// === Timings ===
#define MODBUS_MAXRETRIES_DEFAULT	5
#define MODBUS_INITIALREADTIMEOUT_DEFAULT	100
#define MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT	5

#define NETWORK_DEFAULT_TIMEOUT	10*1000

// === Settings keys ===
// --- Modbus related ---
#define MODBUS_GROUP_NAME "Modbus"
#define MODBUS_MAXRETRIES_KEY "MaxRetries"
#define MODBUS_INITIALREADTIMEOUT_KEY "InitialReadTimeout"
#define MODBUS_CONSEQUENTREADTIMEOUT_KEY "ConsequentReadTimeout"

// --- RequestGroups related ---
#define REQUEST_GROUPS_KEY "RequestGroups"
#define REQUEST_ACTIVITY_KEY "active"
#define REQUEST_DEVICE_KEY "device"
#define REQUEST_ADDRESS_KEY "address"
#define REQUEST_DATA_TYPE_KEY "dataType"
#define REQUEST_DATA_TYPE_VALUE_INT int
#define REQUEST_DATA_TYPE_VALUE_FLOAT float
#define REQUEST_DATA_TYPE_VALUE_DOUBLE double
#define REQUEST_BYTES_PER_ITEM "bytesPerItem"
#define REQUEST_REGISTER_COUNT "registerCount"
#define REQUEST_PERIOD_KEY "period"

#define REQUEST_ARRAY_ITEM_KEY "item"
#define REQUEST_ITEM_NAME_KEY "name"
#define REQUEST_ITEM_DATA_TYPE_KEY REQUEST_DATA_TYPE_KEY
#define REQUEST_ITEM_BYTES_PER_ITEM REQUEST_BYTES_PER_ITEM
#define REQUEST_ITEM_PDU_INDEX_KEY "pduIndex"
#define REQUEST_ITEM_PDU_OFFSET_KEY "pduOffset"
#define REQUEST_ITEM_MULTIPLIER_KEY "multiplier"
#define REQUEST_ITEM_DIVIDER_KEY "divider"
#define REQUEST_ITEM_SIGNUM_INDEX_KEY "signum"

#define REQUEST_ARRAY_PARSING_KEY "parsing"
#define REQUEST_PARSING_TYPE_KEY "type"
//#define REQUEST_PARSING_TYPE_VALUE_INVALID invalid
#define REQUEST_PARSING_TYPE_VALUE_POST post
#define REQUEST_PARSING_TYPE_VALUE_LOG log
#define REQUEST_PARSING_OCCURANCE_KEY "occurance"
#define REQUEST_PARSING_OCCURANCE_DEFAULT 1

#define REQUEST_PARSING_POST_URL_KEY "url"
#define REQUEST_PARSING_POST_TIMEOUT_KEY "timeout"

#define REQUEST_PARSING_LOG_PATH_KEY "logPath"
#define REQUEST_PARSING_LOG_PATH_DEFAULT "Logs"
#define REQUEST_PARSING_LOG_FILENAME_KEY "logName"


// === Object names/prefixes ===
#define OBJECT_NAME_PREPOSITION "Of"
#define REQUEST_MANAGER_NAME_PREFIX "RequestManager"
#define POST_PARSING_PROCESSOR_PREFIX "PostParsingProcessor"
#define LOG_PARSING_PROCESSOR_PREFIX "LogParsingProcessor"

// === Special parsed items's names/values ===
#define PARSED_ITEM_RESPONSE_TYPE_KEY	"responseType"
#define PARSED_ITEM_RESPONSE_TYPE_NORMAL_RESPONSE_VALUE	"normalResponse"
#define PARSED_ITEM_RESPONSE_TYPE_NULL_RESPONSE_VALUE	"nullResponse"
#define PARSED_ITEM_RESPONSE_TYPE_ERROR_RESPONSE_VALUE	"errorResponse"
#define PARSED_ITEM_ERROR_RESPONSE_CODE_KEY	"errorResponseCode"
#define PARSED_ITEM_ERROR_RESPONSE_EXCEPTION_KEY	"errorResponseException"

// === Global datatypes ===
#include <QSharedPointer>
typedef QSharedPointer<class ProtocolDataUnit> PDUSharedPtr_t;
typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

// === Macro stringification helpers ===
#define xstr(s) str(s)
#define str(s) #s

/******************************************************************************
 * Key responseType removed from other then null response postings
 * as it is context-deducible.
 *
 * To make it appear again comment following #define line.
 ******************************************************************************/
#define NO_RESPONSE_TYPE_KEY_USED


#endif // GLOBALS_H
