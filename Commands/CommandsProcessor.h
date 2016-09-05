#ifndef COMMANDSPROCESSOR_H
#define COMMANDSPROCESSOR_H

#include <QSharedPointer>
#include "Commands/CommandDescriptor.h"

class CommandsProcessor : public QObject
{
	Q_OBJECT

public:
	CommandsProcessor();

public slots:
	void processHttpReply(QSharedPointer<class QNetworkReply> reply);
	// Separated mainly for testing purposes
	void processCommandDevice(class QIODevice *device);

signals:
	void commandReceived(CommandDescriptor descriptor);
};

#endif // COMMANDSLIST_H
