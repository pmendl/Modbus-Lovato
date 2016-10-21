#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

#include <QDebug>

#define DEBUG(x) x
#define NODEBUG(x) do { } while(0)

// Following is shortcut for Debug Print
// Do notice the semicolon placed at the end - do not append semicolon even after
// expressions concatenated with operator<<() !!
//#define D_P(x) DEBUG(if(Debug::eventPrintFlag) {qDebug() << x;})
#define D_P(x) DEBUG(qDebug() << x)
//#define PRINT(x) D_P(x)
#define PRINT(x) D_P(x)
#define MARK(x) D_P("*** " << x)
#define NO(x) NODEBUG(x)
#define IMMED(x) qInfo() << x;

//========== Enable or disable various debug prints here ==========
//#define USE_MESSAGE_HANDLER	// Instructs to filter debug prints via Debug/MessageHandler.*

//---------- Logging groups ---------
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
#define DP_LOGGING_ERROR(x) IMMED(x)				// LogServer errors
#define DP_COMMANDS_LIST(x) PRINT(x)				// CommandsList activity
#define DP_COMMANDS_PROCESSOR(x) PRINT(x)			// CommandsProcessor activation
#define DP_COMMANDS_PROCESSOR_DETAILS(x) PRINT(x)	// CommandsProcessor details
#define DP_COMMANDS_PROCESSOR_ERROR(x) PRINT(x)	// CommandsProcessor errors
#define DP_NET_POSTING_INIT(x) PRINT(x)			// PostParsingProcessor initialization
#define DP_NET_SENDER_SEND(x) PRINT(x)			// NetworkSender send action
#define DP_NET_SENDER_ERROR(x) PRINT(x)			// NetworkSender errors
#define DP_NET_SENDER_DETAILS(x) NO(x)		// NetworkSender details
#define DP_NET_HTTP_REPLY(x) PRINT(x)				// HTTP response arrival
#define DP_NET_HTTP_REPLY_DETAILS(x) NO(x)	// HTTP response details
#define DP_NET_HTTP_REPLY_ERRORS(x) PRINT(x)	// HTTP response details
#define DP_CMD_LOG_READER(x) PRINT(x)				// LogReader object major actions
#define DP_CMD_LOG_READER_ERROR(x) PRINT(x)		// LogReader object errors
#define DP_CMD_LOG_READER_DETAILS(x) NO(x)	// LogReader object details
#define DP_CMD_LOG_COPIER(x) PRINT(x)				// LogCopier object instantiation
#define DP_CMD_LOG_COPIER_ERROR(x) PRINT(x)		// LogCopier object errors
#define DP_CMD_LOG_COPIER_DETAILS(x) NO(x)	// LogCopier object details
#define DP_CMD_LOG_FRAGMENT(x) PRINT(x)			// LogFragment object major actions
#define DP_CMD_LOG_FRAGMENT_ERROR(x) PRINT(x)		// LogFragment object errors
#define DP_CMD_LOG_FRAGMENT_DETAILS(x) NO(x)	// LogFragment object details
#define DP_KEYPRES_DEBUG(x) NO(x)			// Emition of keypress event
#define DP_DELAYED_COUNT(x) NO(x)			// Delayed count event
#define DP_NONDELAYED_COUNT(x) NO(x)		// Event ehen passing without delay
#define DP_LOGWRITER_ERROR(x) MARK(x)		// connect failure to deleteLater in LogWritter

//--- Object instancing debug prints ---
#define DP_DEBUGHTTPMULTIPART(x) NO(x)
#define DP_LOGWRITTER_INSTANCES(x) NO(x)

//--- Temporary debug prints ---
#define DP_PANIC(x) PRINT(x)

//--- Specialized debug print related macros
#ifdef USE_MESSAGE_HANDLER
#include "Debug/MemoryAnalytics.h"
#include "Debug/MessageHandler.h"

#define DP_MEMORY_CLEAR globalMessageHandler.clear();
#define DP_MEMORY_CHECK D_P("------ EVENTS:" << globalMessageHandler.getEvents(); Debug::checkMemory());
#else
#define DP_MEMORY_CLEAR
#define DP_MEMORY_CHECK
#endif

//#define DP_EVENTS_START(x) PRINT(Q_FUNC_INFO << "START");
#define DP_EVENTS_START(x) NODEBUG(x);
//#define DP_EVENTS_END(x) PRINT(Q_FUNC_INFO << x);
#define DP_EVENTS_END(x) NODEBUG(x);

//--- Specialized analytics
//#define DC_COUNT(x) globalMessageHandler.countEvent(x);
#define DC_COUNT(x) NODEBUG(x);

#endif // DEBUGMACROS_H
