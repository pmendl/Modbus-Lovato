#include "CommandsProcessor.h"

#include <QDebug>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QFileInfo>

#include "Commands/CommandsList.h"


CommandsProcessor::CommandsProcessor()
{}

void CommandsProcessor::processHttpReply(QNetworkReply *reply)
{
	qDebug() << "CommandsProcessor processing" << reply;
	qDebug() << "\tContent-Disposition=" << reply->rawHeader("Content-Disposition");

	const QRegularExpression contentDispositionExpression(
				QStringLiteral("filename\\s*=\\s*(\\\")([^\"]*)")
				);
	QRegularExpressionMatch match(contentDispositionExpression.match(reply->rawHeader("Content-Disposition")));
	if(match.hasMatch()) {
		QFileInfo fi(match.captured(2));
		qDebug().noquote().nospace() << "\tfilename=" << fi.fileName();

		if(fi.completeSuffix().toUpper() == QStringLiteral("CONF")) {
			qDebug() << "\tCONF extension detected.";

			CommandsList commands(reply->url().url(), reply);
		}
		else {
				qDebug() << "\tNo Content-Disposition filename found! ERROR";
		}
	}

	qDebug() << "CommandsProcessor finished" << reply;

}

void CommandsProcessor::processCommandsList(CommandsList *commandsList) {
	for (CommandDescriptor descr : *commandsList)
		emit commandReceived(descr);
}
