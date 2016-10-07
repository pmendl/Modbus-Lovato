#include "CommandsProcessor.h"

#include "DebugMacros.h"
#include <QNetworkReply>
#include <QRegularExpression>
#include <QFileInfo>

#include "Commands/CommandsList.h"


CommandsProcessor::CommandsProcessor()
{}

void CommandsProcessor::processHttpReply(QNetworkReply *reply)
{
	DP_COMMANDS_PROCESSOR("CommandsProcessor processing" << reply);
	DP_COMMANDS_PROCESSOR_DETAILS("\tContent-Disposition=" << reply->rawHeader("Content-Disposition"));

	const QRegularExpression contentDispositionExpression(
				QStringLiteral("filename\\s*=\\s*(\\\")([^\"]*)")
				);
	QRegularExpressionMatch match(contentDispositionExpression.match(reply->rawHeader("Content-Disposition")));
	if(match.hasMatch()) {
		QFileInfo fileInfo(match.captured(2));
		DP_COMMANDS_PROCESSOR_DETAILS("\tfilename=" << fileInfo.fileName());

		if(fileInfo.completeSuffix().toUpper() == QStringLiteral("INI")) {
			DP_COMMANDS_PROCESSOR_DETAILS("\tINI extension detected.");
			if(fileInfo.fileName().toUpper() == QStringLiteral("SYSTEM")) {
				DP_COMMANDS_PROCESSOR_DETAILS("\tSystem INI file detected.");
			}
			if(fileInfo.fileName().toUpper() == QStringLiteral("USER")) {
				DP_COMMANDS_PROCESSOR_DETAILS("\tUser INI file detected.");
			}

		}
		else if(fileInfo.completeSuffix().toUpper() == QStringLiteral("CMD")) {
			DP_COMMANDS_PROCESSOR_DETAILS("\tCMD extension detected.");

			CommandsList commands(reply->url().url(), reply);
			processCommandsList(&commands);
		}
		else {
				DP_COMMANDS_PROCESSOR_DETAILS("\tFilename extension found in Content-Disposition not supported! ERROR");
		}
	}

	DP_COMMANDS_PROCESSOR_DETAILS("CommandsProcessor finished" << reply);

}

void CommandsProcessor::processCommandsList(CommandsList *commandsList) {
	for (CommandDescriptor descr : *commandsList) {
		emit commandReceived(descr);
	}
}
