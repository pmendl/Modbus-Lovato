#include "CommandDeleteFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogServer.h"
#include "Commands/CommandDescriptor.h"

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
			QString source(_logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))));

			if(source.isEmpty()) {
				D_P("\tSource parameter missing or invalid:" << source << "\n\tAborting...");
				return;
			}

			QSharedPointer<LogMaintenanceLocker> lock(_logServer->fileMaintenanceLocker());

			if(QFile::remove(source))
				D_P("\tRemoved:" << source);
			else
				D_P("\tRemoval FAILED !!! File may remain on disk:" << source);
	}

}
