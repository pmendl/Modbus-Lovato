#include "CommandsList.h"

#include <QDebug>
#include <QIODevice>
#include <QRegularExpression>
#include <QUrl>

class QUrl;

CommandsList::CommandsList(class QIODevice *device, QObject *parent) :
	CommandsList(QString(), device, parent)
{}

CommandsList::CommandsList(QUrl originatorUrl, QIODevice *device, QObject *parent) :
	CommandsList(originatorUrl.url(), device, parent)
{}


CommandsList::CommandsList(QString originatorUrl, QIODevice *device, QObject *parent) : QObject(parent)
{
	qDebug() << "CommandsDescriptor starts filling up...";
	QRegularExpression commandExpr(QStringLiteral("^\\s*(\\w+)\\s*$"));
	QRegularExpression parameterExpr(QStringLiteral("^\\s*(\\w+)\\s*=\\s*(\"[^\"\\n]*\"|[^=\\n]*)\\s*$"));

	CommandDescriptor currentCommand(originatorUrl);
	do {
		QRegularExpressionMatch match;
		QString line(device->readLine());
		match=commandExpr.match(line);
		if(match.hasMatch()) {
			qDebug() << "\tCOMMAND:" << match.captured(1);
			if(currentCommand.size() > 0) {
				append(currentCommand);
				currentCommand.clear();
			}
			currentCommand.insert("COMMAND", match.captured(1).toUpper());
		}
		match=parameterExpr.match(line);
		if(match.hasMatch()) {
			QString value(match.captured(2).replace('"', QStringLiteral("")));
			qDebug() << "\tPARAMETER:" << match.captured(1) << "=" << value;
			// Safety precaution against hackery command insertions
			if(match.captured(1).toUpper() != QStringLiteral("COMMAND")) {
				currentCommand.insert(match.captured(1).toUpper(), value);
			}
		}
//		qDebug() << line;

	} while(device->bytesAvailable() > 0);
	if(currentCommand.size() > 0) {
		append(currentCommand);
	}
	qDebug() << "CommandsList fill-up finished...\n-->" << *this;
}
