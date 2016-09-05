#include "CommandsProcessor.h"

#include <QDebug>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QFileInfo>

#include "Commands/CommandsParser.h"

CommandsProcessor::CommandsProcessor()
{}

void CommandsProcessor::processHttpReply(QSharedPointer<class QNetworkReply> reply)
{
	qDebug() << "CommandsProcessor processing" << reply.data();
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

			CommandsParser parser(reply.data());
			(void)parser;
		}

	}
	else {
		qDebug() << "\tNo Content-Disposition filename found! ERROR";
	}

	qDebug() << "CommandsProcessor finished" << reply.data();
}
