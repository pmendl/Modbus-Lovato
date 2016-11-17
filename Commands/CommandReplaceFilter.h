#ifndef COMMANDREPLACEFILTER_H
#define COMMANDREPLACEFILTER_H


#include <QObject>
#include <QSharedPointer>

class CommandDescriptor ;

class CommandReplaceFilter : public QObject
{
	Q_OBJECT

public:
	explicit CommandReplaceFilter(QSharedPointer<class LogServer> logServer, QObject *parent = 0,
							  void (*preprocessValidCommand)(CommandDescriptor &descriptor) = 0);

	void onCommandReceived(CommandDescriptor descriptor);

signals:

public slots:

private:
	QSharedPointer<class LogServer> _logServer;
	void (*_preprocessValidCommand)(CommandDescriptor &descriptor);
};

#endif // COMMANDREPLACEFILTER_H
