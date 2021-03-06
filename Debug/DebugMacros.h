#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

#include <QDebug>

#define DEBUG(x) x
#define NODEBUG(x) do { } while(0)

// Following is shortcut for Debug Print
// Do notice the semicolon placed at the end - do not append semicolon even after
// expressions concatenated with operator<<() !!
#define D_P(x) DEBUG(qDebug() << x)
#define PRINT(x) D_P(x)
#define MARK(x) D_P("*** " << x)
#define NO(x) NODEBUG(x)

#define CHECKPOINT(x) D_P("*** CHECKPOINT " x);

//========== Enable or disable various debug prints here ==========
#define DP_SHOULD_NOT_OCCURE_ERROR(x) MARK(x)	// Condidions that should never be met
#define DP_INIT(x) PRINT(x)						// Application initialization
#define DP_PROCESSING_INIT(x) PRINT(x)			// Processing manager initialization
#define DP_REQUESTMANAGER_INIT(x) PRINT(x)		// Request manager initialization
#define DP_REQUESTMANAGER_PARSING(x) NO(x)	// Request manager parsing details
#define DP_REQUESTMANAGER_ERROR(x) PRINT(x)		// Request manager errors
#define DP_PROCESSING_REQUEST(x) PRINT(x)			// Processing of Modbus request
#define DP_MODBUS_INIT(x) PRINT(x)				// ModbusSerialMaster init
#define DP_MODBUS_ERROR(x) PRINT(x)				// Modbus protocol errors
#define DP_LOGGING_INIT(x) PRINT(x)				// LogServer init
#define DP_LOGGING_ACTION(x) NO(x)			// LogServer record trace
#define DP_LOGGING_ERROR(x) PRINT(x)				// LogServer errors
#define DP_COMMANDS_LIST(x) PRINT(x)				// CommandsList activity
#define DP_COMMANDS_PROCESSOR(x) PRINT(x)			// CommandsProcessor activation
#define DP_COMMANDS_PROCESSOR_DETAILS(x) NO(x)	// CommandsProcessor details
#define DP_COMMANDS_PROCESSOR_ERROR(x) PRINT(x)	// CommandsProcessor errors
#define DP_COMMAND(x) PRINT(x)						// Command implementing object activation
#define DP_NET_POSTING_INIT(x) PRINT(x)			// PostParsingProcessor initialization
#define DP_NET_SENDER_SEND(x) PRINT(x)			// NetworkSender send action
#define DP_NET_SENDER_ERROR(x) PRINT(x)			// NetworkSender errors
#define DP_NET_SENDER_DETAILS(x) NO(x)		// NetworkSender details
#define DP_NET_HTTP_REPLY(x) PRINT(x)				// HTTP response arrival
#define DP_NET_HTTP_REPLY_DETAILS(x) NO(x)	// HTTP response details
#define DP_CMD_LOG_READER(x) PRINT(x)				// LogReader object major actions
#define DP_CMD_LOG_READER_ERROR(x) PRINT(x)		// LogReader object errors
#define DP_CMD_LOG_READER_DETAILS(x) NO(x)	// LogReader object details
#define DP_CMD_LOG_COPIER(x) PRINT(x)				// LogCopier object instantiation
#define DP_CMD_LOG_COPIER_ERROR(x) PRINT(x)		// LogCopier object errors
#define DP_CMD_LOG_COPIER_DETAILS(x) NO(x)	// LogCopier object details
#define DP_CMD_LOG_DELETE(x) PRINT(x)				// LogCopier object instantiation
#define DP_CMD_LOG_DELETE_ERROR(x) PRINT(x)		// LogCopier object errors
//#define DP_CMD_LOG_DELETE_DETAILS(x) NO(x)	// LogCopier object details
#define DP_CMD_LOG_REPLACE(x) PRINT(x)				// LogCopier object instantiation
#define DP_CMD_LOG_REPLACE_ERROR(x) PRINT(x)		// LogCopier object errors
//#define DP_CMD_LOG_REPLACE_DETAILS(x) NO(x)	// LogCopier object details
#define DP_CMD_LOG_FRAGMENT(x) PRINT(x)			// LogFragment object major actions
#define DP_CMD_LOG_FRAGMENT_ERROR(x) PRINT(x)		// LogFragment object errors
#define DP_CMD_LOG_FRAGMENT_DETAILS(x) NO(x)	// LogFragment object details
#define DP_KEYPRES_DEBUG(x) NO(x)			// Emition of keypress event
#define DP_RESET(x) MARK(x)						// Reset main events
#define DP_RESET_ERROR(x) MARK(x)				// Reset related errors
#define DP_RESET_DETAILS(x) NO(x)				// Reset related details
#define DP_RESET_SHOW_BLOCKERS(x) NO(x)			// Dump of reset blockers hash
#define DP_RESET_PARSING_PROCESSOR_DETAILS(x) NO(x) // Memory values compared

//--- Object instancing debug prints ---
#define DP_DEBUGHTTPMULTIPART(x) NO(x)
#define DP_INSTANCE_COUNTER_BASE(x) PRINT(x)

//--- Temporary debug prints ---
#define DP_MEMORY(x) NO(x)
#define DP_DELAYED_COUNT(x) NO(x)
#define DP_PANIC(x) PRINT(x)

//--- Specialized debug print related macros
//#include "Debug/MemoryAnalytics.h"
//#define DP_EVENTS_START(x) PRINT(Q_FUNC_INFO << "START");
//#define DP_EVENTS_END(x) PRINT(Q_FUNC_INFO << x);

#define DP_EVENTS_START(x) NO(x);
#define DP_EVENTS_END(x) NO(x);




#endif // DEBUGMACROS_H
