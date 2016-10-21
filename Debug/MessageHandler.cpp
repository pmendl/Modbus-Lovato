#include "MessageHandler.h"

#include <QByteArray>

#include <stdio.h>

#include "Debug/MemoryAnalytics.h"

void Debug::myMessageOutput(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
	globalMessageHandler.handleMessage(type, msg);
}

MessageHandler::MessageHandler()
{
	_buffer.reserve(100000); // To prevent memory changes due to text appends
}

void MessageHandler::handleMessage(QtMsgType type, const QString &msg)
{
	if(type == QtDebugMsg) {
		std::putc('.',stderr);
	}
	else {
		fprintf(stderr, "%s", msg.toLocal8Bit().constData());
	}
	_buffer.append(QString(QStringLiteral("<%1>")).arg(Debug::snapMemory()));
	_buffer.append(msg);
	_buffer.append('\n');
}

void MessageHandler::dispatchMessage(bool doPrint) {
	if(doPrint) {
		fprintf(stderr,	"%u \n", _buffer.size());
		fprintf(stderr,	"%s", _buffer.constData());
		for (QHash<QString, int>::iterator i = _events.begin(); i != _events.end(); ++i) {
			i.value() = 0;
		}
	}
	fprintf(stderr,	"<%u>", Debug::snapMemory());
	//std::putc('x',stderr);
	clear();
}

void MessageHandler::clear() {
	_buffer.truncate(0); // ... workaround instead of clear() to preserve reservation
}

void MessageHandler::countEvent(QString id) {
	QHash<QString, int>::iterator i(_events.find(id));
	if(i != _events.end()) {
		++i.value();
	}
	else {
		_events.insert(id, 1);
	}
}

QHash<QString,int> MessageHandler::getEvents() {
	return _events;
}

MessageHandler globalMessageHandler;


// 	return eventPrintFlag = (q_func_info == "void RequestManager::onResponse(PDUSharedPtr_t)");
