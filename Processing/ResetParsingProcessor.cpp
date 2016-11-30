#include "ResetParsingProcessor.h"

#include<QDebug>
#include <QSettings>
#include <QDateTime>

#include "Debug/DebugMacros.h"
#include "Globals.h"
#include "System/Memory.h"
#include "System/Reset.h"

/**
 * @class RestartParsingProcessor
 * @brief Allows for activation of soft-reset based on date/time and free memory
 *
 * There are three possible values for the respective parser group in the INI file:
 * -# **dateTimeRegExp**<br>
 * If present, the following two memory requirements are only
 * evaluated, if the pattern given here matches against the current date/time
 * represented in the "dd-MM-yyyy HH:mm:ss" format.
 * -# **memorySize**<br>
 * If present, requires the minimal value of free memory, expressed as absolute value in bytes.
 * -# **memoryPercentage**<br>
 * If present, requires the minimal relative amount of free memory,
 * expressed as percents of the free memory ammount measured when the application
 * starts.<br>
 * <br>
 * Following table shows, under what conditions the soft-reset is inited.
 * Date | memorySize | memoryPercentage | Reset inited
 * ---- | :----------: | :----------------: | :-------------:
 * Matched or not present in INI file | unsatisfied | unimportant | YES
 * Matched or not present in INI file| unimportant | unsatisfied | YES
 * Matched | Not present in INI file | Not present in INI file | YES
 * Not matched | unimportant | unimportant | NO
 * Matched or not present in INI file | satisfied | satisfied or not present in INI file| NO
 * Matched or not present in INI file | satisfied or not present in INI file | satisfied | NO
 * Not present in INI file | Not present in INI file | Not present in INI file | NO
*/

RestartParsingProcessor::RestartParsingProcessor(QSettings *settings) :
	ParsingProcessor(settings),
	_dateTimeRegExp(settings->value(REQUEST_RESET_TIME_REGEXP_KEY/*, QStringLiteral(".")*/).toString()),
	_memorySize(settings->value(REQUEST_RESET_MEMORY_SIZE_KEY, -1).toInt()),
	_memoryPercentage((settings->value(REQUEST_RESET_MEMORY_PERCENTAGE_KEY, -1).toInt()
					  * System::getMemory()) / 100)
{}

bool RestartParsingProcessor::isValid() const
{
#warning DO IMPLEMENT
	return true;
}

void RestartParsingProcessor::process(class RequestManager *)
{
	QString currentTime(QDateTime::currentDateTime().toString(QStringLiteral("dd-MM-yyyy HH:mm:ss")));
	if(_dateTimeRegExp.match(currentTime).hasMatch()) {
		int mem(System::getMemory());
		DP_RESET_PARSING_PROCESSOR_DETAILS(mem << ":" << _memorySize << "/" << _memoryPercentage);
		QString reason;
		if(mem < _memorySize)
			reason.append(QStringLiteral("memory_size_required=%1|").arg(_memorySize));
		if(mem < _memoryPercentage)
			reason.append(QStringLiteral("memory_percentage_required=%1|").arg(_memoryPercentage));
		if(!reason.isEmpty()) {
			reason.append(QStringLiteral("memory=%1|").arg(mem));
		}
		if((!reason.isEmpty() ||
			((_memorySize < 0) && (_memoryPercentage < 0))
			)
				&& !_dateTimeRegExp.pattern().isEmpty()) {
			reason.prepend(QStringLiteral("time=")+currentTime+QStringLiteral("|")
						   +QStringLiteral("match=")+_dateTimeRegExp.pattern()+QStringLiteral("|")
						   );
		}

		if(!reason.isEmpty()) {
			System::resetInitiate(reason);
		}
	}
}
