#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QtGlobal>
#include <QString>

namespace Debug {

void myMessageOutput(QtMsgType, const QMessageLogContext &, const QString &msg);

}

class MessageHandler
{
public:
	MessageHandler();

	void handleMessage(const QString &msg);

private:
	QByteArray _buffer;
};

extern MessageHandler globalMessageHandler;

#endif // MESSAGEHANDLER_H
