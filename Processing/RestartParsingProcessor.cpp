#include "RestartParsingProcessor.h"

#include<QDebug>
#include <QSettings>
#include <QDateTime>

#include "Globals.h"

RestartParsingProcessor::RestartParsingProcessor(QSettings *settings) :
	ParsingProcessor(settings),
	_regexp(settings->value(RESTART_TIME_REGEXP_KEY).toString())
{
	qDebug() << "***" << _regexp.pattern();
}

bool RestartParsingProcessor::isValid() const
{
#warning DO IMPLEMENT
	return true;
}

void RestartParsingProcessor::process(class RequestManager *)
{
	QString currentTime(QDateTime::currentDateTime().toString(QStringLiteral("dd-MM-yyyy HH:mm:ss")));
	qDebug() << "***" << currentTime << (_regexp.match(currentTime).hasMatch() ? ">>> MATCH <<<" : "no match");
}
