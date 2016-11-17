#include "CommandReplaceFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogServer.h"
#include "Commands/CommandDescriptor.h"


CommandReplaceFilter::CommandReplaceFilter(QSharedPointer<LogServer> logServer, QObject *parent,
						  void (*preprocessValidCommand)(CommandDescriptor &descriptor)) :
	QObject(parent),
	_logServer(logServer),
	_preprocessValidCommand(preprocessValidCommand)
{}

void CommandReplaceFilter::onCommandReceived(CommandDescriptor descriptor) {
	if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_REPLACE_VALUE)) {
		// --- REPLACE COMMAND ---
		_preprocessValidCommand(descriptor);
		QString source(_logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_SOURCE_FILE_NAME)))),
				target(_logServer->pathname(descriptor.value(QStringLiteral(COMMAND_PARAMETER_TARGET_FILE_NAME)))),
				temp(target.section(QChar('.'),0,-2)+QStringLiteral(".old"));

		if((source.isEmpty() || target.isEmpty())) {
			D_P("\tSource and/or target parameter missing or invalid:" << source << "->" << target << "\n\tAborting...");
			return;
		}

		QSharedPointer<LogMaintenanceLocker> lock(_logServer->fileMaintenanceLocker());

		if(QFile::remove(temp))
			D_P("\tRemoved temporary:" << temp);

		if(QFile::exists(target)) {
			if(QFile::rename(target, temp))
				D_P("\tRenamed" << target << "->" << temp);
			else {
				D_P("\tRenaming" << target << "->" << temp << "FAILED!\n\tAborting...");
				return;
			}
		}

		if(QFile::rename(source,target))
			D_P("\tRenamed" << source << "->" << target);
		else {
			D_P("\tRenaming" << source << "->" << target << "FAILED!\n\tAborting...");
			return;
		}

		if(QFile::remove(temp))
			D_P("\tRemoved temporary:" << temp);
		else
			D_P("\tFAILED temporary removal!!! File may remain on disk:" << temp);
	}
}
