#ifndef COMMANDSPROCESSOR_H
#define COMMANDSPROCESSOR_H

#include <QSharedPointer>

class CommandsProcessor : public QObject
{
	Q_OBJECT

public:
	CommandsProcessor();

public slots:
	void processHttpReply(QSharedPointer<class QNetworkReply> reply);
};

#endif // COMMANDSPROCESSOR_H
