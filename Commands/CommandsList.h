#ifndef COMMANDSLIST_H
#define COMMANDSLIST_H

#include <QUrl>
#include <QList>

#include <Commands/CommandDescriptor.h>

class CommandsList : public QList<CommandDescriptor>
{
public:

	explicit CommandsList(class QIODevice *device);
	explicit CommandsList(QString originatorUrl, class QIODevice *device);
	explicit CommandsList(QUrl originatorUrl, class QIODevice *device);

};

#endif // COMMANDSDESCRIPTOR_H
