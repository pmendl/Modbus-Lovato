#include "LogReader.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QRegularExpression>
#include <QNetworkReply>

#include <cstring>

#include "Globals.h"
#include "Network/NetworkSender.h"

LogReader::LogReader(QString url, QString pathname, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	LogReader( url,  pathname,  QStringLiteral(POST_ELEMENT_LOG_NO_ID_VALUE),  from,  to,  group,  parent)
{}


LogReader::LogReader(QString url, QString pathname, QString id, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	QThread(parent),
	_url(url),
	_logFile(pathname),
	_opened(_logFile.open(QIODevice::ReadOnly)),
	_startIndex(0),
	_endIndex(-1),
	_from(from),
	_to(to),
	_group(group),
	_id(id),
	_logBuffer(&_logOutput)
{
	qDebug() << "LogReader" << pathname << "isValid() = " << _opened;
	connect(this, &QThread::finished, this, &QObject::deleteLater);
}

LogReader::~LogReader() {
	if(_opened)
		_logFile.close();
	qDebug() << "LogReader destroyed.";
}

bool LogReader::isValid() {
	return _opened;
}

void LogReader::run() {
	qDebug() << "LogReader thread runs...";
	if(!_opened)
		return;

	const QRegularExpression recordRegexp(QStringLiteral("(.*?)\\|VALUES (.*?)\\|(.*)"));
	QString record;
	do {
		_endIndex = _logFile.pos();
		record = _logFile.readLine(LOG_MAX_BUFFER_SIZE);
//		qDebug() << _logFile.pos() << record;
		QRegularExpressionMatch match(recordRegexp.match(record));
//		qDebug() << "Matches:" << match.capturedTexts();
		if(match.hasMatch()) {
			qDebug() << match.captured(1) << match.captured(2);
			if(_from.isValid() && (QDateTime::fromString(match.captured(1)) < _from))
				continue;
			if(_to.isValid() && (QDateTime::fromString(match.captured(1)) > _to))
				continue;
			if((!_group.isEmpty()) &&
					(!QRegularExpression(_group).match(match.captured(2)).hasMatch())
					)
				continue;

			if((_logOutput.size() + record.size()) < LOG_MAX_BUFFER_SIZE)
				_logOutput += record;
			else {
				QSharedPointer<QHttpMultiPart> multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType));

				QHttpPart part;
				part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain"));
				part.setHeader(QNetworkRequest::ContentDispositionHeader,
							   QVariant(
								   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
								   .arg(POST_ELEMENT_LOG_FILE_NAME)
								   .arg(_logFile.fileName())
								   )
							   );
				part.setBodyDevice(&_logBuffer);
				multipart->append(part);

				if(!_id.isEmpty()) {
					part.setHeader(QNetworkRequest::ContentDispositionHeader,
									   QString(QStringLiteral("form-data; name=%1"))
									   .arg(POST_ELEMENT_LOG_ID_NAME));
					part.setBody(_id.toUtf8());
					multipart->append(part);
				}

				if(_from.isValid()) {
					part.setHeader(QNetworkRequest::ContentDispositionHeader,
									   QString(QStringLiteral("form-data; name=%1"))
									   .arg(POST_ELEMENT_LOG_FROM_NAME));
					part.setBody(_from.toString().toUtf8());
					multipart->append(part);
				}

				if(_to.isValid()) {
					part.setHeader(QNetworkRequest::ContentDispositionHeader,
									   QString(QStringLiteral("form-data; name=%1"))
									   .arg(POST_ELEMENT_LOG_TO_NAME));
					part.setBody(_to.toString().toUtf8());
					multipart->append(part);
				}

				part.setHeader(QNetworkRequest::ContentDispositionHeader,
								   QString(QStringLiteral("form-data; name=%1"))
								   .arg(POST_ELEMENT_LOG_START_INDEX_NAME));
				part.setBody(QString(QStringLiteral("%1")).arg(_startIndex).toUtf8());
				multipart->append(part);

				part.setHeader(QNetworkRequest::ContentDispositionHeader,
								   QString(QStringLiteral("form-data; name=%1"))
								   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
				part.setBody(QString(QStringLiteral("%1")).arg(_endIndex).toUtf8());
				multipart->append(part);

#warning CONTINUE IMPLEMENTATION HERE
// Call NetworkSender
				_sender->wait();
				_sender.reset(new NetworkSender(_url, multipart));

				_startIndex = _endIndex;
				_logFile.seek(_startIndex);
			}
		}






	} while (!record.isEmpty());
}
