#include "CommandDeleteFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogServer.h"
#include "Commands/CommandDescriptor.h"
#include "System/Reset.h"

CommandDeleteFilter::CommandDeleteFilter(QSharedPointer<LogServer> logServer, QObject *parent,
						  void (*preprocessValidCommand)(CommandDescriptor &descriptor)) :
	QObject(parent),
	_logServer(logServer),
	_preprocessValidCommand(preprocessValidCommand)
{}

void CommandDeleteFilter::onCommandReceived(CommandDescriptor descriptor)
{

	if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_DELETE_VALUE)) {
			// --- DELETE COMMAND ---
			_preprocessValidCommand(descriptor);

			if(!System::startResetSensitiveProcess(RESET_PRIORITY_BULK)) {
				DP_RESET_ERROR("NOT EXECUTING DELETE COMMAND: Reset protection in progress.");
				return;
			}

			QString source(_logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))));

			if(source.isEmpty()) {
				DP_CMD_LOG_DELETE_ERROR("\tSource parameter missing or invalid:" << source << "\n\tAborting...");
				return;
			}

			QSharedPointer<LogMaintenanceLocker> lock(_logServer->fileMaintenanceLocker());

			if(QFile::remove(source))
				DP_CMD_LOG_DELETE("\tRemoved:" << source);
			else
				DP_CMD_LOG_DELETE_ERROR("\tRemoval FAILED !!! File may remain on disk:" << source);
	}

}
