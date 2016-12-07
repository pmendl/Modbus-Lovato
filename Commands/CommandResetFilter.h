#ifndef COMMANDRESETFILTER_H
#define COMMANDRESETFILTER_H


#include <QObject>
#include <QSharedPointer>

class CommandDescriptor ;

class CommandResetFilter : public QObject
{
	Q_OBJECT

public:
	explicit CommandResetFilter(QObject *parent = 0,
							  void (*preprocessValidCommand)(CommandDescriptor &descriptor) = 0);

	void onCommandReceived(CommandDescriptor descriptor);

signals:

public slots:

private:
	void (*_preprocessValidCommand)(CommandDescriptor &descriptor);
};

#endif // COMMANDRESETFILTER_H
