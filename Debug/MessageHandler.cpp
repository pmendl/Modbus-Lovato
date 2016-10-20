#include "MessageHandler.h"

#include <QByteArray>

#include <stdio.h>

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
	_buffer = msg.toLocal8Bit();
	fprintf(stderr,	"%s\n", _buffer.constData());
}

MessageHandler globalMessageHandler;
