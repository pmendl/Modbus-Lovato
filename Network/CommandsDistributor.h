#ifndef COMMANDSDISTRIBUTOR_H
#define COMMANDSDISTRIBUTOR_H

#include <QSharedPointer>

class CommandsDistributor : public QObject
{
	Q_OBJECT
public:
	explicit CommandsDistributor(QObject *parent = 0);
	void emitCommandReply(QSharedPointer<class QNetworkReply> reply) const;

signals:
	void commandReplyReceived(QSharedPointer<class QNetworkReply> reply) const;

public slots:
};

#endif // COMMANDSDISTRIBUTOR_H
