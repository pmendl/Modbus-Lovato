#include "CommandsProcessor.h"

#include "Debug/DebugMacros.h"
#include <QNetworkReply>
#include <QRegularExpression>
#include <QFileInfo>
#include <QSettings>

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
			QSharedPointer<QSettings> settings(new QSettings());
			if(fileInfo.fileName().toUpper() == QStringLiteral("SYSTEM.INI")) {
				settings.reset(new QSettings(QSettings::SystemScope, "PMCS", "LovatoModbus"));
				DP_COMMANDS_PROCESSOR_DETAILS("\tSystem INI file detected.");
			}
			else if(fileInfo.fileName().toUpper() == QStringLiteral("USER.INI")) {
				DP_COMMANDS_PROCESSOR_DETAILS("\tUser INI file detected.");
			}
			else {
				DP_COMMANDS_PROCESSOR_ERROR("\tINI file name is neither User or System one ! ERROR");
				return;
			}

			QByteArray buff(reply->readAll());
			QFile file(settings->fileName());
			DP_COMMANDS_PROCESSOR_DETAILS("\tPath =" << file.fileName());
			if(!file.open(QFile::WriteOnly)) {
				DP_COMMANDS_PROCESSOR_ERROR("\tCould not open INI file for writting! ABORTING");
				return;
			}
			qint64 count(file.write(buff));

			// Ensures no "result variable unused" warning when DP_CMD_LOG_READER_DETAILS
			// is defined as NODEBUG() and as such generates no code.
			// In no case generates any code itself.
			(void)count;

			DP_COMMANDS_PROCESSOR_DETAILS("\tWritten" << count << "bytes.");
			file.close();
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
