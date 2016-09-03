#include "LogReader.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QBuffer>

#include "Network/NetworkSender.h"

#include "Globals.h"

extern void postFile(NetworkSender *sender);
extern NetworkSender ns;

LogReader::LogReader(QString url, QString pathname, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	LogReader( url,  pathname,  QString(),  from,  to,  group,  parent)
{}

LogReader::LogReader(QString url, QString pathname,
		  QString id,
		  QString group, QObject *parent) :
	LogReader( url,  pathname,  id,  QDateTime(),  QDateTime(),  group,  parent)
{}


LogReader::LogReader(QString url, QString pathname, QString id, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	QThread(parent),
	_url(url),
	_logFile(pathname),
	_logFragment(id, from, to, group, parent),
	_sender(new NetworkSender(this))
{
	_logFile.open(QIODevice::ReadOnly);
	qDebug() << "LogReader" << pathname << "isOpen() = " << _logFile.isOpen();

	connect(this, &LogReader::testsig, &_sender, &NetworkSender::sendUrl);
	connect(this, &QThread::finished, this, &LogReader::onFinished);
	start();
}

LogReader::~LogReader() {
	_logFile.close();
	qDebug() << "LogReader destroyed.";
}

bool LogReader::isValid() {
	return _logFile.isOpen();
}

void LogReader::run() {
	qDebug() << "LogReader thread runs...";
	if(!isValid()) {
		qDebug() << "LogReader aborted as log file could not get opened...";
		return;
	}

	const QRegularExpression recordRegexp(QStringLiteral("(.*?)\\|VALUES (.*?)\\|(.*)"));
	QString record;
	_logFile.seek(_logFragment.startIndex);
	do {
		_logFragment.endIndex = _logFile.pos();
		qDebug() << "@@@> " << _logFragment.endIndex;

		record = _logFile.readLine(LOG_MAX_BUFFER_SIZE);
//		qDebug() << _logFile.pos() << record;
		QRegularExpressionMatch match(recordRegexp.match(record));
//		qDebug() << "Matches:" << match.capturedTexts();
		if(match.hasMatch()) {
//***/			qDebug() << match.captured(1) << match.captured(2);
			if(_logFragment.from().isValid() && (QDateTime::fromString(match.captured(1)) < _logFragment.from()))
				continue;
			if(_logFragment.to().isValid() && (QDateTime::fromString(match.captured(1)) > _logFragment.to()))
				continue;
			if((!_logFragment.group().isEmpty()) &&
					(!QRegularExpression(_logFragment.group()).match(match.captured(2)).hasMatch())
					)
				continue;

			if((_logFragment.size() + record.size()) < LOG_MAX_BUFFER_SIZE) {
				qDebug() << record;
				_logFragment.buffer().append(record);
				qDebug() << "\t_logFragment.size()=" << _logFragment.size();
				continue;
			}
			else {
//				_startIndex = _endIndex;
				return;
			}
		}

	} while (!record.isEmpty());
//	_startIndex = -1;
	_logFragment.lastFragment = true;
	return;
}

void LogReader::onFinished() {
	if(_logFragment.size() > 0) {
		httpTransmit(&_logFragment);
//		postFile(_sender);
	}

	if(!_logFragment.lastFragment) {
		start();
		qDebug() << "LogReader started next iteration.";
		return;
	}

	qDebug() << "LogReader completed.";
}


void LogReader::httpTransmit(LogFragment *fragment)
{
	qDebug() << "LogReader starts HTTP transmit ...";

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
	_logFragment.open(QIODevice::ReadOnly);
	part.setBodyDevice(_logFragment.pullBuffer());
	multipart->append(part);
	part = QHttpPart();

	if(!fragment->id().isEmpty()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_ID_NAME));
		part.setBody(fragment->id().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}
	if(fragment->from().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FROM_NAME));
		part.setBody(fragment->from().toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(fragment->to().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_TO_NAME));
		part.setBody(fragment->to().toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_START_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(fragment->startIndex).toUtf8());
	multipart->append(part);
	part = QHttpPart();


	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(fragment->endIndex).toUtf8());
	multipart->append(part);

	qDebug() << "\tWaiting for last HTTP transmit to end...";
	_sender.wait();

	qDebug() << "\tSending new HTTP multipart...";
	_sender.send(_url, multipart);
	multipart->setParent(_sender.reply().data());
	_logFragment.setParent(_sender.reply().data());

	qDebug() << "LogReader finished HTTP transmit ...";

}
