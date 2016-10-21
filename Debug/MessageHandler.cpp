#include "MessageHandler.h"

#include <QByteArray>

#include <stdio.h>

#include "Debug/MemoryAnalytics.h"

void Debug::myMessageOutput(QtMsgType, const QMessageLogContext &, const QString &msg)
{
	globalMessageHandler.handleMessage(msg);
}

MessageHandler::MessageHandler()
{
	_buffer.reserve(100000);
}

void MessageHandler::handleMessage(const QString &msg)
{
	std::putc('.',stderr);
	_buffer.append(QString(QStringLiteral("<%1>")).arg(Debug::snapMemory()));
	_buffer.append(msg);
	_buffer.append('\n');
}

void MessageHandler::dispatchMessage(bool doPrint) {
	if(doPrint) {
		fprintf(stderr,	"%u \n", _buffer.size());
		fprintf(stderr,	"%s", _buffer.constData());
	}
	fprintf(stderr,	"<%u>", Debug::snapMemory());
	//std::putc('x',stderr);
	_buffer.truncate(0);
}

MessageHandler globalMessageHandler;


// 	return eventPrintFlag = (q_func_info == "void RequestManager::onResponse(PDUSharedPtr_t)");
