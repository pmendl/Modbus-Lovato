#include "CommandResetFilter.h"

#include "Globals.h"
#include "Debug/DebugMacros.h"
#include "Log/LogServer.h"
#include "Log/LogCopier.h"
#include "Commands/CommandDescriptor.h"
#include "System/Reset.h"


CommandResetFilter::CommandResetFilter(QObject *parent,
						  void (*preprocessValidCommand)(CommandDescriptor &descriptor)) :
	QObject(parent),
	_preprocessValidCommand(preprocessValidCommand)
{}

void CommandResetFilter::onCommandReceived(CommandDescriptor descriptor)
{
		// --- RESET COMMAND ---
	if (descriptor.value(QStringLiteral(COMMAND_NAME)) == QStringLiteral(COMMAND_RESET_VALUE)) {
		_preprocessValidCommand(descriptor);
		System::resetInitiate(System::normalReset, "Reset command received");
	}
}
