#ifndef COMMANDSDESCRIPTOR_H
#define COMMANDSDESCRIPTOR_H

#include <QObject>
#include <QList>
#include <QHash>

class CommandsDescriptor : public QObject, public QList<QHash<QString,QString>>
{
	Q_OBJECT

public:

	explicit CommandsDescriptor(class QIODevice *device, QObject *parent = 0);

signals:


public slots:
};

#endif // COMMANDSDESCRIPTOR_H
