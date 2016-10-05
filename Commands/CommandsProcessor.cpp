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
		QFileInfo fileInfo(match.captured(2));
		qDebug().noquote().nospace() << "\tfilename=" << fileInfo.fileName();

		if(fileInfo.completeSuffix().toUpper() == QStringLiteral("INI")) {
			qDebug() << "\tINI extension detected.";
			if(fileInfo.fileName().toUpper() == QStringLiteral("SYSTEM")) {
				qDebug() << "\tSystem INI file detected.";
			}
			if(fileInfo.fileName().toUpper() == QStringLiteral("USER")) {
				qDebug() << "\tUser INI file detected.";
			}

		}
		else if(fileInfo.completeSuffix().toUpper() == QStringLiteral("CMD")) {
			qDebug() << "\tCMD extension detected.";

			CommandsList commands(reply->url().url(), reply);
			processCommandsList(&commands);
		}
		else {
				qDebug() << "\tFilename extension found in Content-Disposition not supported! ERROR";
		}
	}

	qDebug() << "CommandsProcessor finished" << reply;

}

void CommandsProcessor::processCommandsList(CommandsList *commandsList) {
	for (CommandDescriptor descr : *commandsList) {
//		qDebug() << "*** emited" << descr;
		emit commandReceived(descr);
	}
}
