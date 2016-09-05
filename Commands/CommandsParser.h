#ifndef COMMANDSPARSER_H
#define COMMANDSPARSER_H

#include <QObject>
#include <QHash>

#define CommandDescriptor QHash<QString,QString>

class CommandsParser : public QObject, public CommandDescriptor
{
	Q_OBJECT

public:

	explicit CommandsParser(class QIODevice *device, QObject *parent = 0);

signals:


public slots:
};

#endif // COMMANDSPARSER_H
