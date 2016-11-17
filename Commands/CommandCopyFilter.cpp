#include "CommandCopyFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogServer.h"
#include "Log/LogCopier.h"
#include "Commands/CommandDescriptor.h"


CommandCopyFilter::CommandCopyFilter(QSharedPointer<LogServer> logServer, QObject *parent,
						  void (*preprocessValidCommand)(CommandDescriptor &descriptor)) :
	QObject(parent),
	_logServer(logServer),
	_preprocessValidCommand(preprocessValidCommand)
{}

void CommandCopyFilter::onCommandReceived(CommandDescriptor descriptor)
{
	if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_COPY_VALUE)) {
		// --- COPY COMMAND ---
		_preprocessValidCommand(descriptor);
		QSharedPointer<LogMaintenanceLocker> lock(_logServer->fileMaintenanceLocker());

		DP_MEMORY("----------------------> new LogCopier");
		new LogCopier(_logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME))),
					  _logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TARGET_FILE_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_FROM_NAME))),
					  QDateTime::fromString(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TO_NAME))),
					  descriptor.value(QStringLiteral(COMMAND_PARAMETER_GROUP_NAME))
					  );
	}
}
