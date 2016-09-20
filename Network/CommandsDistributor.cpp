#include "CommandsDistributor.h"

CommandsDistributor::CommandsDistributor(QObject *parent) : QObject(parent)
{}

void CommandsDistributor::emitCommandReply(QNetworkReply *reply) const
{
	emit commandReplyReceived(reply);
}
