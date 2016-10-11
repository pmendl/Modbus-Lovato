#ifndef COMMANDDESCRIPTOR_H
#define COMMANDDESCRIPTOR_H

#include <QHash>

//class QNetworkReply;

class CommandDescriptor : public QHash<QString, QString>
{
public:
	CommandDescriptor(const QString originatorUrl);

	const QString originatorUrl;
};

#endif // COMMANDDESCRIPTOR_H
