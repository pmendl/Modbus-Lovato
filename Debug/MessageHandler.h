#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QtGlobal>
#include <QString>
#include <QHash>

namespace Debug {

void myMessageOutput(QtMsgType, const QMessageLogContext &, const QString &msg);

}

class MessageHandler
{
public:
	MessageHandler();

	void handleMessage(QtMsgType type, const QString &msg);
	void dispatchMessage(bool doPrint = true);
	void clear();
	void countEvent(QString id);
	QHash<QString,int> getEvents();

private:
	QByteArray _buffer;
	QHash<QString,int> _events;
};

#ifdef USE_MESSAGE_HANDLER
extern MessageHandler globalMessageHandler;
#endif

#endif // MESSAGEHANDLER_H
