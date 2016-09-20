#ifndef COMMANDSDISTRIBUTOR_H
#define COMMANDSDISTRIBUTOR_H

#include <QObject>

class QNetworkReply;

class CommandsDistributor : public QObject
{
	Q_OBJECT
public:
	explicit CommandsDistributor(QObject *parent = 0);
	void emitCommandReply(QNetworkReply *reply) const;

signals:
	void commandReplyReceived(QNetworkReply *reply) const;

public slots:
};

#endif // COMMANDSDISTRIBUTOR_H
