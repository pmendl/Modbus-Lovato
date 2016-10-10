#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

#include <QDebug>

#define DEBUG(x) x
#define NODEBUG(x) do { } while(0)

// Following is shortcut for Debug Print
// Do notice the semicolon placed at the end - do not append semicolon even after
// expressions concatenated with operator<<() !!
#define D_P(x) DEBUG(qDebug() << x)
#define NOD_P(x) NODEBUG(x)

//========== Enable or disable various debug prints here ==========
#define DP_INIT(x) D_P(x)						// Application initialization
#define DP_PROCESSING_INIT(x) D_P(x)			// Processing manager initialization
#define DP_REQUESTMANAGER_INIT(x) D_P(x)		// Request manager initialization
#define DP_REQUESTMANAGER_PARSING(x) NOD_P(x)	// Request manager parsing details
#define DP_REQUESTMANAGER_ERROR(x) D_P(x)		// Request manager errors
#define DP_PROCESSING_REQUEST(x) D_P(x)			// Processing of Modbus request
#define DP_MODBUS_INIT(x) D_P(x)				// ModbusSerialMaster init
#define DP_MODBUS_ERROR(x) D_P(x)				// Modbus protocol errors
#define DP_LOGGING_INIT(x) D_P(x)				// LogServer init
#define DP_LOGGING_ACTION(x) NOD_P(x)			// LogServer record trace
#define DP_LOGGING_ERROR(x) D_P(x)				// LogServer errors
#define DP_COMMANDS_LIST(x) D_P(x)				// CommandsList activity
#define DP_COMMANDS_PROCESSOR(x) D_P(x)			// CommandsProcessor activation
#define DP_COMMANDS_PROCESSOR_DETAILS(x) D_P(x)	// CommandsProcessor details
#define DP_COMMANDS_PROCESSOR_ERROR(x) D_P(x)	// CommandsProcessor errors
#define DP_NET_POSTING_INIT(x) D_P(x)			// PostParsingProcessor initialization
#define DP_NET_SENDER_SEND(x) D_P(x)			// NetworkSender send action
#define DP_NET_SENDER_ERROR(x) D_P(x)			// NetworkSender errors
#define DP_NET_SENDER_DETAILS(x) D_P(x)		// NetworkSender details
#define DP_NET_HTTP_REPLY(x) D_P(x)				// HTTP response arrival
#define DP_NET_HTTP_REPLY_DETAILS(x) D_P(x)	// HTTP response details
#define DP_CMD_LOG_READER(x) D_P(x)				// LogReader object major actions
#define DP_CMD_LOG_READER_ERROR(x) D_P(x)		// LogReader object errors
#define DP_CMD_LOG_READER_DETAILS(x) NOD_P(x)	// LogReader object details
#define DP_CMD_LOG_COPIER(x) D_P(x)				// LogCopier object instantiation
#define DP_CMD_LOG_COPIER_ERROR(x) D_P(x)		// LogCopier object errors
#define DP_CMD_LOG_COPIER_DETAILS(x) NOD_P(x)	// LogCopier object details
#define DP_CMD_LOG_FRAGMENT(x) D_P(x)			// LogFragment object major actions
#define DP_CMD_LOG_FRAGMENT_ERROR(x) D_P(x)		// LogFragment object errors
#define DP_CMD_LOG_FRAGMENT_DETAILS(x) NOD_P(x)	// LogFragment object details

//--- Object instancing debug prints ---
#define DP_DEBUGHTTPMULTIPART(x) D_P(x)

//--- Temporary debug prints ---
#define DP_DELAYED_COUNT(x) D_P(x)
#define DP_MEMORY(x) D_P(x)
#define DP_PANIC(x) D_P(x)
#define DP_NETSENDER_TIMEREVENT(x) D_P(x)




#endif // DEBUGMACROS_H
