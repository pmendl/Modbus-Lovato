#ifndef COMMANDSLIST_H
#define COMMANDSLIST_H

#include <QObject>
#include <QList>
#include <QHash>

#include <Commands/CommandDescriptor.h>

class CommandsList : public QObject, public QList<CommandDescriptor>
{
	Q_OBJECT

public:

	explicit CommandsList(class QIODevice *device, QObject *parent = 0);
	explicit CommandsList(QString originatorUrl, class QIODevice *device, QObject *parent = 0);
	explicit CommandsList(QUrl originatorUrl, class QIODevice *device, QObject *parent = 0);

signals:


public slots:
};

#endif // COMMANDSDESCRIPTOR_H
