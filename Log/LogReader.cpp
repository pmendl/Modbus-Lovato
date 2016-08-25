#include "LogReader.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QRegularExpression>
#include <QNetworkReply>

#include "Globals.h"

extern void postFile(NetworkSender *sender);
extern NetworkSender ns;

LogReader::LogReader(QString url, QString pathname, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	LogReader( url,  pathname,  POST_ELEMENT_LOG_NO_ID_VALUE,  from,  to,  group,  parent)
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
	_logBuffer(new QBuffer(this))
{
	qDebug() << "LogReader" << pathname << "isValid() = " << _opened;
	connect(this, &QThread::finished, this, &LogReader::onFinished);
	start();
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
	if(!_opened) {
		delete _logBuffer;
		return;
	}

	const QRegularExpression recordRegexp(QStringLiteral("(.*?)\\|VALUES (.*?)\\|(.*)"));
	QString record;
	_logFile.seek(_startIndex);
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

			if((_logBuffer->size() + record.size()) < LOG_MAX_BUFFER_SIZE) {
				_logBuffer->buffer().append(record);
				continue;
			}
			else {
				_startIndex = _endIndex;
				return;
			}
		}

	} while (!record.isEmpty());
	_startIndex = -1;
	return;
}

void LogReader::onFinished() {
	if(!_logBuffer->bytesToWrite() > 0) {
		//httpTransmit();
		postFile(&ns);
	}

	if(_startIndex >= 0) {
		start();
		qDebug() << "LogReader started next iteration.";
		return;
	}

	qDebug() << "LogReader completed.";
	deleteLater();
}


void LogReader::httpTransmit()
{
	qDebug() << "LogReader starts HTTP transmit ...";
/*
	QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType));

	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain"));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant(
					   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
					   .arg(POST_ELEMENT_LOG_FILE_NAME)
					   .arg(_logFile.fileName())
					   )
				   );
	part.setBodyDevice(_logBuffer);
	multipart->append(part);
	part = QHttpPart();

	if(!_id.isEmpty()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_ID_NAME));
		part.setBody(_id.toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}
	if(_from.isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FROM_NAME));
		part.setBody(_from.toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(_to.isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_TO_NAME));
		part.setBody(_to.toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_START_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_startIndex).toUtf8());
	multipart->append(part);
	part = QHttpPart();


	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_endIndex).toUtf8());
	multipart->append(part);

	qDebug() << "LogReader waiting for last HTTP transmit to end...";
	_sender.wait();

*/

	QBuffer *demoFile = new QBuffer(this);
	demoFile->setData(QStringLiteral("Testovací log:\nŘádek 1\nŘádek 2\n").toUtf8());
	demoFile->open(QIODevice::ReadOnly);

	QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain"));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant("form-data; name=\"logFile\"; filename=\"Test.log\""));
//	part.setRawHeader("Expires", QDateTime::currentDateTimeUtc().toString().toUtf8());
	part.setBodyDevice(demoFile);
	multipart->append(part);


	qDebug() << "\t_sender.send(_url, multipart)";
	_sender.send(_url, multipart);
	multipart->setParent(_sender.reply().data());
	_logBuffer->setParent(_sender.reply().data());
	_logBuffer = new QBuffer(this);

	qDebug() << "LogReader finished HTTP transmit ...";

}
