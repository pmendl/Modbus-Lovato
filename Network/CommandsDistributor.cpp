#include "CommandsDistributor.h"

CommandsDistributor::CommandsDistributor(QObject *parent) : QObject(parent)
{}

void CommandsDistributor::emitCommandReply(QSharedPointer<class QNetworkReply> reply) const
{
	emit commandReplyReceived(reply);
}
