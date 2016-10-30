#include "LogReader.h"

#include <QFile>
#include HTTP_MULTI_PART_INCLUDE
#include <QRegularExpression>
#include <QNetworkReply>
#include <QBuffer>

#include "DebugMacros.h"
#include HTTP_MULTI_PART_INCLUDE

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
	_multiPart(0),
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
	DP_EVENTS_START(onFragmentReady)
	DP_CMD_LOG_READER_DETAILS("LogReader has new fragment" << fragment << "ready for sending ...");
	_readyFragment = fragment;
	checkSending();
	DP_EVENTS_END
}

void LogReader::onReplyFinished()
{
	DP_EVENTS_START(onReplyFinished)
	DP_CMD_LOG_READER_DETAILS("LogReader finished HTTP transmit" << static_cast<QNetworkReply*>(sender())->url().url());
	_sendPending = false;
	checkSending();
	DP_EVENTS_END
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
	DP_EVENTS_START(onMultipartSent)
	if(multiPart != _multiPart)
		return;
	connect(reply, &QNetworkReply::finished, this, &LogReader::onReplyFinished);
}

void LogReader::sendReadyFragment() {
	DP_CMD_LOG_READER_DETAILS("LogReader::sendReadyFragment starts HTTP transmit ...");
	if(_readyFragment == 0) {
		DP_CMD_LOG_READER_ERROR("LogReader::sendReadyFragment called with zero pointer! ERROR!");
		DP_EVENTS_COND("LogReader::sendReadyFragment called with zero pointer! ERROR!");
		return;
	}

	if(!_readyFragment->open(QIODevice::ReadOnly)) {
		DP_CMD_LOG_READER_ERROR("LogReader::sendReadyFragment aborts as it can not open _readyFragment for reading...");
		_readyFragment->deleteLater();
		_readyFragment=0;
		DP_EVENTS_COND("LogReader::sendReadyFragment aborts as it can not open _readyFragment for reading...");
		return;
	}

	_multiPart = new HTTP_MULTI_PART_USED(QHttpMultiPart::FormDataType, _readyFragment);

	if(_postFileContent) {
		_multiPart->appendFile(POST_ELEMENT_LOG_FILE_NAME,_readyFragment, _readyFragment->logfile()->fileName());
	}

	if(!_readyFragment->id().isEmpty()) {
		_multiPart->appendFormData(POST_ELEMENT_LOG_ID_NAME, _readyFragment->id());
	}

	if(_readyFragment->from().isValid()) {
		_multiPart->appendFormData(POST_ELEMENT_LOG_FROM_NAME, _readyFragment->from());
	}

	if(_readyFragment->to().isValid()) {
		_multiPart->appendFormData(POST_ELEMENT_LOG_TO_NAME, _readyFragment->to());
	}

	_multiPart->appendFormData(POST_ELEMENT_LOG_START_INDEX_NAME, _readyFragment->startIndex());
	_multiPart->appendFormData(POST_ELEMENT_LOG_END_INDEX_NAME, _readyFragment->endIndex());
	_multiPart->appendFormData(POST_ELEMENT_LOG_RECORD_COUNT_NAME, _readyFragment->recordCnt());

	if(_readyFragment->firstFound() != _readyFragment->startIndex()) {
		_multiPart->appendFormData(POST_ELEMENT_LOG_FIRST_FOUND_NAME, _readyFragment->firstFound());
	}

	if(_readyFragment->lastFound() != _readyFragment->endIndex()) {
		_multiPart->appendFormData(POST_ELEMENT_LOG_LAST_FOUND_NAME, _readyFragment->lastFound());
	}

	_sendPending = true;
	DP_CMD_LOG_READER_DETAILS("\tPosting new HTTP multipart send signal...");
	bool result(QMetaObject::invokeMethod(&_sender, "sendMultipart", \
											 Q_ARG(QHttpMultiPart *, static_cast<QHttpMultiPart*>(_multiPart))));

	// Ensures no "result variable unused" warning when DP_CMD_LOG_READER_DETAILS
	// is defined as NODEBUG() and as such generates no code.
	// In no case generates any code itself.
	(void)result;

	DP_CMD_LOG_READER_DETAILS("\tQMetaObject::invokeMethod(sendMultipart...)" << (result ? "Succeeded" : "Failed"));

	processFragment((_readyFragment->nextFragment()));
	DP_EVENTS_END
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
