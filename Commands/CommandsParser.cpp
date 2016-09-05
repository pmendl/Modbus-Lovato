#include "CommandsParser.h"

#include <QDebug>
#include <QIODevice>
#include <QRegularExpression>

CommandsParser::CommandsParser(QIODevice *device, QObject *parent) : QObject(parent)
{
	qDebug() << "CommandsParser starts...";
	QRegularExpression commandExpr(QStringLiteral("^\\s*(\\w+)\\s*$"));
	QRegularExpression parameterExpr(QStringLiteral("^\\s*(\\w+)\\s*=\\s*(\"[^\"]*\"|[^=]*)\\s*$"));

	do {
		QRegularExpressionMatch match;
		QString line(device->readLine());
		match=commandExpr.match(line);
		if(match.hasMatch())
			qDebug() << "\tCOMMAND:" << match.captured(1);
		match=parameterExpr.match(line);
		if(match.hasMatch())
			qDebug() << "\tPARAMETER:" << match.captured(1);
		qDebug() << line;

	} while(device->bytesAvailable() > 0);
	qDebug() << "CommandsParser ends...";
}
