#include "LogReader.h"

#include "DebugMacros.h"
#include <QFile>
#include HTTP_MULTI_PART_INCLUDE
#include <QRegularExpression>
#include <QNetworkReply>
#include <QBuffer>

#include "Network/DebugHttpMultiPart.h"
#include "Globals.h"
#include "Network/NetworkSender.h"

LogReader::LogReader(QString url, QString pathname,  bool postFileContent,
					 QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	LogReader( url,  pathname,  postFileContent, QString(),  from,  to,  group,  parent)
{}

LogReader::LogReader(QString url, QString pathname, bool postFileContent,
		  QString id,
		  QString group, QObject *parent) :
	LogReader( url,  pathname,  postFileContent,  id,  QDateTime(),  QDateTime(),  group,  parent)
{}


LogReader::LogReader(QString url, QString pathname, bool postFileContent, QString id, QDateTime from, QDateTime to,
					 QString group, QObject *parent) :
	QThread(parent),
	_sender(this, url),
	_readyFragment(0),
	_lastFragment(false),
	_multipart(0),
	_sendPending(false),
	_postFileContent(postFileContent)
{
	start();
	connect(&_sender, &NetworkSender::multipartSent, this, &LogReader::onMultipartSent);

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(pathname)), postFileContent, id, from, to, group, 0, this));
	DP_CMD_LOG_READER("LogReader" << pathname << "constructed." << url);
}

bool LogReader::postFileContent() const
{
	return _postFileContent;
}

LogReader::~LogReader() {
	quit();
	wait();
	DP_CMD_LOG_READER("LogReader destroyed.");
}

void LogReader::onFragmentReady(LogFragment *fragment)
{
	DP_CMD_LOG_READER_DETAILS("LogReader has new fragment" << fragment << "ready for sending ...");
	_readyFragment = fragment;
	checkSending();
}

void LogReader::onReplyFinished()
{
	DP_CMD_LOG_READER_DETAILS("LogReader finished HTTP transmit" << static_cast<QNetworkReply*>(sender())->url().url());
	_sendPending = false;
	checkSending();
}

void LogReader::checkSending()
{
	if(_sendPending)
		return;

	if(_readyFragment != 0) {
		sendReadyFragment();
	}
	else {
		if(_lastFragment) {
			deleteLater();
		}
	}
}

void LogReader::onMultipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply) {
	if(multiPart != _multipart)
		return;
	connect(reply, &QNetworkReply::finished, this, &LogReader::onReplyFinished);
}

void LogReader::sendReadyFragment() {
	DP_CMD_LOG_READER_DETAILS("LogReader::sendReadyFragment starts HTTP transmit ...");
	if(_readyFragment == 0) {
		DP_CMD_LOG_READER_ERROR("LogReader::sendReadyFragment called with zero pointer! ERROR!");
		return;
	}

	if(!_readyFragment->open(QIODevice::ReadOnly)) {
		DP_CMD_LOG_READER_ERROR("LogReader::sendReadyFragment aborts as it can not open _readyFragment for reading...");
		_readyFragment->deleteLater();
		_readyFragment=0;
		return;
	}

	_multipart = new HTTP_MULTI_PART_USED(QHttpMultiPart::FormDataType, _readyFragment);
	QHttpPart part;

	if(_postFileContent) {
		part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain; charset=utf-8"));
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QVariant(
						   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
						   .arg(POST_ELEMENT_LOG_FILE_NAME)
						   .arg(_readyFragment->logfile()->fileName())
						   )
					   );
		part.setBodyDevice(_readyFragment);
		_multipart->append(part);
		part = QHttpPart();
	}

	if(!_readyFragment->id().isEmpty()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_ID_NAME));
		part.setBody(_readyFragment->id().toUtf8());
		_multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->from().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FROM_NAME));
		part.setBody(_readyFragment->from().toString().toUtf8());
		_multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->to().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_TO_NAME));
		part.setBody(_readyFragment->to().toString().toUtf8());
		_multipart->append(part);
		part = QHttpPart();

	}

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_START_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->startIndex()).toUtf8());
	_multipart->append(part);
	part = QHttpPart();


	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->endIndex()).toUtf8());
	_multipart->append(part);

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QString(QStringLiteral("form-data; name=%1"))
				   .arg(POST_ELEMENT_LOG_RECORD_COUNT_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->recordCnt()).toUtf8());
	_multipart->append(part);
	part = QHttpPart();

	if(_readyFragment->firstFound() != _readyFragment->startIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FIRST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->firstFound()).toUtf8());
		_multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->lastFound() != _readyFragment->endIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_LAST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->lastFound()).toUtf8());
		_multipart->append(part);
		part = QHttpPart();

	}
	_sendPending = true;


	DP_CMD_LOG_READER_DETAILS("\tPosting new HTTP multipart send signal...");
	bool result(QMetaObject::invokeMethod(&_sender, "sendMultipart", \
											 Q_ARG(QHttpMultiPart *, static_cast<QHttpMultiPart*>(_multipart))));
	DP_CMD_LOG_READER_DETAILS("\tQMetaObject::invokeMethod(sendMultipart...)" << (result ? "Succeeded" : "Failed"));

	processFragment((_readyFragment->nextFragment()));
}

void LogReader::processFragment(LogFragment *fragment) {
	_readyFragment = 0;
	if(!fragment) {
		_lastFragment = true;
		checkSending();
		return;
	}
	DP_CMD_LOG_READER_DETAILS("\tStarting processing of new fragment...");
	connect(fragment, &LogFragment::fragmentReady, this, &LogReader::onFragmentReady);
	connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
		DP_CMD_LOG_READER_ERROR("LogReader detected fragmentFailed on" << fragment);
		fragment->deleteLater();
		deleteLater();
	});
	QMetaObject::invokeMethod(fragment, "fillFragment");
}
