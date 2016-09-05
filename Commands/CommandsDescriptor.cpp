#include "CommandsDescriptor.h"

#include <QDebug>
#include <QIODevice>
#include <QRegularExpression>

CommandsDescriptor::CommandsDescriptor(QIODevice *device, QObject *parent) : QObject(parent)
{
	qDebug() << "CommandsDescriptor starts filling up...";
	QRegularExpression commandExpr(QStringLiteral("^\\s*(\\w+)\\s*$"));
	QRegularExpression parameterExpr(QStringLiteral("^\\s*(\\w+)\\s*=\\s*(\"[^\"\\n]*\"|[^=\\n]*)\\s*$"));

	QHash<QString,QString> currentCommand;
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
	qDebug() << "CommandsDescriptor fill-up finished...\n-->" << *this;
}
