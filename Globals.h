#ifndef GLOBALS_H
#define GLOBALS_H

/*
 * The following define blocks any automatic processing, leaving in action
 * only main.cpp defined responses to keypresses. It is intended for debug
 * or testing only.
 * SHOULD BE COMMENTED-OUT ALL TIMES in the production environment !!!
 */
//#define NO_AUTOMATIC_PROCESSING

// === Log related settings ===
#define LOG_RECORD_VALUES "VALUES "
#define LOG_SEPARATOR_ITEMS "|"
#define LOG_SEPARATOR_RECORD "\n"
#define LOG_MAX_BUFFER_SIZE (1024*1024)

// === Timings ===
#define MODBUS_MAXRETRIES_DEFAULT	5
#define MODBUS_INITIALREADTIMEOUT_DEFAULT	100 // [ms]
#define MODBUS_MAXCONSEQUENTREADS_DEFAULT	3
#define MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT	10 // [ms]
#define MODBUS_INCYCLEWAIT_DEFAULT	(MODBUS_CONSEQUENTREADTIMEOUT_DEFAULT * 1000 * 1000) // [ns]

#define NETWORK_DEFAULT_TIMEOUT	10*1000
//#warning DEBUG ONLY SHORTENED TIME
//#define NETWORK_DEFAULT_TIMEOUT	3*1000

// === Network debugging/release object version choice ===
//#define HTTP_MULTI_PART_USED QHttpMultiPart

//#define HTTP_MULTI_PART_INCLUDE "Network/DebugHttpMultiPart.h"
//#define HTTP_MULTI_PART_USED DebugHttpMultiPart

#define HTTP_MULTI_PART_INCLUDE "Network/ExtendedHttpMultiPart.h"
#define HTTP_MULTI_PART_USED ExtendedHttpMultiPart

/******************************************************************************
 * Key responseType removed from other then null response postings
 * as it is context-deducible.
 *
 * To make it appear again comment following #define line.
 ******************************************************************************/
#define NO_RESPONSE_TYPE_KEY_USED


// === Settings keys ===
// --- Debugging and testing ---
#define DEBUG_SERVER_HTTP_DEFAULT "http://mirtes.wz.cz/import.php"
#define DEBUG_GROUP_KEY "Debug"
#define DEBUG_SUPPRESS_PERIODICAL_REQUESTING_KEY "SuppressPeriodicalRequesting"
#define DEBUG_SERVER_HTTP_KEY "HttpServer"
#define DEBUG_SUPPRESS_RESET_KEY "SuppressResetExecution"

// --- Reset processing related ---
#define RESET_GROUP_KEY "Reset"
#define RESET_FORCED_TIMEOUT_KEY "timeout" // in seconds
#define RESET_NOTIFICATION_LOG_KEY "log"
#define RESET_NOTIFICATION_LOG_DEFAULT "Common.log"
#define RESET_NOTIFICATION_URL_KEY "notificationUrl"

// --- Watchdog related ---
#define WATCHDOG_GROUP_KEY "Watchdog"
#define WATCHDOG_HEARTBEAT_GPIO_KEY "heartbeatPin"
#define WATCHDOG_HEARTBEAT_PERIOD_KEY "period"
#define WATCHDOG_HEARTBEAT_PERIOD_DEFAULT 0 // [ms] Zero means run on each event loop idle

// --- Signalization (LED to user) related ---
#define SIGNALISATION_GROUP_KEY "Signalisation"
#define SIGNALISATION_HEARTBEAT_GPIO_KEY "httpPin"

// --- Server related ---
#define SERVER_PANIC_CONNECTIONS_GROUP_KEY "PanicConnections"
#define SERVER_PANIC_CONNECTIONS_URL_KEY "url"
#define TEST_FILE_ITEM_KEY "testFile"

// --- Modbus related ---
#define MODBUS_GROUP_NAME "Modbus"
#define MODBUS_MAXRETRIES_KEY "MaxRetries"
#define MODBUS_INITIALREADTIMEOUT_KEY "InitialReadTimeout"
#define MODBUS_MAXCONSEQUENTS_KEY "MaxConsequents"
#define MODBUS_CONSEQUENTREADTIMEOUT_KEY "ConsequentReadTimeout"
#define MODBUS_INCYCLEWAIT_KEY "InCycleWait"

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
#define REQUEST_PARSING_TYPE_VALUE_MEMORY memory
#define REQUEST_PARSING_TYPE_VALUE_RESET reset
#define REQUEST_PARSING_OCCURANCE_KEY "occurance"
#define REQUEST_PARSING_OCCURANCE_DEFAULT 1

#define REQUEST_PARSING_POST_URL_KEY "url"
#define REQUEST_PARSING_POST_TIMEOUT_KEY "timeout"

#define REQUEST_PARSING_LOG_PATH_KEY "logPath"
#define REQUEST_PARSING_LOG_PATH_DEFAULT "Logs"
#define REQUEST_PARSING_LOG_FILENAME_KEY "logName"

#define REQUEST_RESET_TIME_REGEXP_KEY "dateTimeRegExp" // Against "dd-MM-yyyy HH:mm:ss"
#define REQUEST_RESET_MEMORY_SIZE_KEY "memorySize" // minimal free memory size [bytes]
#define REQUEST_RESET_MEMORY_PERCENTAGE_KEY "memoryPercentage" // minimal percents of inital free memory [%]


// === Object names/prefixes ===
#define OBJECT_NAME_PREPOSITION "Of"
#define REQUEST_MANAGER_NAME_PREFIX "RequestManager"
#define POST_PARSING_PROCESSOR_PREFIX "PostParsingProcessor"
#define LOG_PARSING_PROCESSOR_PREFIX "LogParsingProcessor"

// === POST elements names and values ===
// --- Special parsed items's names/values ---
#define PARSED_ITEM_RESPONSE_TYPE_KEY	"responseType"
#define PARSED_ITEM_RESPONSE_TYPE_NORMAL_RESPONSE_VALUE	"normalResponse"
#define PARSED_ITEM_RESPONSE_TYPE_NULL_RESPONSE_VALUE "nullResponse"
#define PARSED_ITEM_RESPONSE_TYPE_ERROR_RESPONSE_VALUE	"errorResponse"
#define PARSED_ITEM_ERROR_RESPONSE_CODE_KEY	"errorResponseCode"
#define PARSED_ITEM_ERROR_RESPONSE_EXCEPTION_KEY "errorResponseException"

// --- LOG related elements ---
#define POST_ELEMENT_LOG_FILE_NAME "logFile"
//#define POST_ELEMENT_LOG_NO_ID_VALUE "unidentified"
#define POST_ELEMENT_LOG_ID_NAME "id"
#define POST_ELEMENT_LOG_FROM_NAME "from"
#define POST_ELEMENT_LOG_TO_NAME "to"
#define POST_ELEMENT_LOG_START_INDEX_NAME "startIndex"
#define POST_ELEMENT_LOG_END_INDEX_NAME "endIndex"
#define POST_ELEMENT_LOG_FIRST_FOUND_NAME "firstFound"
#define POST_ELEMENT_LOG_LAST_FOUND_NAME "lastFound"
#define POST_ELEMENT_LOG_RECORD_COUNT_NAME "recordCount"

// --- RESET related elements ---
#define POST_ELEMENT_RESET_INIT_KEY "resetInitiateReason"

// === Commands and parameter names ===
// --- Common definitions ---
#define COMMAND_NAME "COMMAND"

#define COMMAND_PARAMETER_FROM_NAME "FROM"
#define COMMAND_PARAMETER_TO_NAME "TO"
#define COMMAND_PARAMETER_GROUP_NAME "GROUP"

#define COMMAND_PARAMETER_SOURCE_FILE_NAME "SOURCE"
#define COMMAND_PARAMETER_TARGET_FILE_NAME "TARGET"

// --- Log command related ---
#define COMMAND_LOG_VALUE "LOG"

// Uses TARGET,  optional FROM, optional TO, optional GROUP and the following
#define COMMAND_LOG_PARAMETER_POSTCONTENT_NAME "POSTCONTENT"
#define COMMAND_LOG_PARAMETER_ID_NAME "ID"

// --- File orinted commands ---
// Both uses SOURCE, TARGET, optional FROM, optional TO and optional GROUP
#define COMMAND_COPY_VALUE "COPY"
#define COMMAND_REPLACE_VALUE "REPLACE"
// Uses SOURCE
#define COMMAND_DELETE_VALUE "DELETE"

// === Reset protection priority levels ===
#define RESET_PRIORITY_IMMEDIATE 1
#define RESET_PRIORITY_NETWORK 2
#define RESET_PRIORITY_BULK 3


// === Global datatypes ===
#include <QSharedPointer>
typedef QSharedPointer<class ProtocolDataUnit> PDUSharedPtr_t;
typedef QSharedPointer<class ApplicationDataUnitSerial> ADUSharedPtr_t;

// === Global functions ===
extern void printMemory();

// === Macro stringification helpers ===
#define xstr(s) str(s)
#define str(s) #s

#endif // GLOBALS_H
