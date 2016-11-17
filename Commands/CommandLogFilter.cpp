#include "CommandLogFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogReader.h"
#include "Log/LogServer.h"
#include "Commands/CommandDescriptor.h"

CommandLogFilter::CommandLogFilter(QSharedPointer<LogServer> logServer, QObject *parent,
						  void (*preprocessValidCommand)(CommandDescriptor &descriptor)) :
//CommandLogFilter::CommandLogFilter(QSharedPointer<LogServer> logServer, QObject *parent) :
	QObject(parent),
	_logServer(logServer),
	_preprocessValidCommand(preprocessValidCommand)
{}

void CommandLogFilter::onCommandReceived(CommandDescriptor descriptor)
{
	if(descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_LOG_VALUE)) {
		_preprocessValidCommand(descriptor);
		bool postFileContent(false);
		if(trueCandidates.contains(descriptor.value(QStringLiteral(COMMAND_LOG_PARAMETER_POSTCONTENT_NAME)).toLower())) {
				postFileContent=true;
		}
		DP_MEMORY("----------------------> new LogReader");
		new LogReader(descriptor.originatorUrl,
					  _logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))),
					  postFileContent,
					  descriptor.value(QStringLiteral(COMMAND_LOG_PARAMETER_ID_NAME)),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_FROM_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TO_NAME))),
					  descriptor.value(QStringLiteral(COMMAND_PARAMETER_GROUP_NAME))
					  );
	}
}
