#include "LogReader.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QBuffer>

#include "Globals.h"
#include "Network/NetworkSender.h"

QHttpMultiPart multix;

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
//	if(false)
	connect(&_sender, &NetworkSender::multipartSent, this, &LogReader::onMultipartSent);
	qDebug() << "*** LogReader: NetworkSender::multipartSent connected";

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(pathname)), postFileContent, id, from, to, group, this, this));
	qDebug() << "LogReader" << pathname << "constructed." << url;
}

bool LogReader::postFileContent() const
{
	return _postFileContent;
}

LogReader::~LogReader() {
	quit();
	wait();
	qDebug() << "LogReader destroyed.";
}

void LogReader::onFragmentReady(LogFragment *fragment)
{
	qDebug() << "*** LogReader has new fragment" << fragment << "ready for sending ...";
	_readyFragment = fragment;
	checkSending();
}

void LogReader::onReplyFinished()
{
	qDebug() << "*** LogReader finished HTTP transmit ...";
	_sendPending = false;
	checkSending();
}

void LogReader::checkSending()
{
	qDebug() << "*** checkSending()" << _readyFragment << _lastFragment << _sendPending;
	if(_readyFragment != 0) {
		if(!_sendPending) {
			qDebug() << "*** sendReadyFragment()";
			sendReadyFragment();
		}
	}
	else {
		if(_lastFragment) {
			qDebug() << "*** _lastFragment=true ->  deleteLater()";
			deleteLater();
		}
	}
}

void LogReader::onMultipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply) {
	qDebug() << "*** onMultipartSent called..." << reply;
	qDebug() << "*** onMultipartSent:" << multiPart << " ?= "<< _multipart;
	if(multiPart != _multipart)
		return;
	qDebug() << "*** reply" << reply << "connected to checkSending()";
	connect(reply, &QNetworkReply::finished, this, &LogReader::checkSending);
}

void LogReader::sendReadyFragment() {
	qDebug() << "LogReader::sendReadyFragment starts HTTP transmit ...";
	if(_readyFragment == 0) {
		qDebug() << "LogReader::sendReadyFragment called with zero pointer! ERROR!";
		return;
	}

	if(!_readyFragment->open(QIODevice::ReadOnly)) {
		qDebug() << "LogReader::sendReadyFragment aborts as it can not open _readyFragment for reading...";
#warning DEBUG/RESTORE THE BELOW FUNCTIONALITY (LogReader destruction)
		_readyFragment->deleteLater();
		_readyFragment=0;
		return;
	}

//	QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType, _readyFragment));
	_multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType, _readyFragment);

	QHttpPart part;
/*
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
		multipart->append(part);
		part = QHttpPart();
	}

	if(!_readyFragment->id().isEmpty()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_ID_NAME));
		part.setBody(_readyFragment->id().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->from().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FROM_NAME));
		part.setBody(_readyFragment->from().toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->to().isValid()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_TO_NAME));
		part.setBody(_readyFragment->to().toString().toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_START_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->startIndex()).toUtf8());
	multipart->append(part);
	part = QHttpPart();


	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->endIndex()).toUtf8());
	multipart->append(part);

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QString(QStringLiteral("form-data; name=%1"))
				   .arg(POST_ELEMENT_LOG_RECORD_COUNT_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->recordCnt()).toUtf8());
	multipart->append(part);
	part = QHttpPart();

	if(_readyFragment->firstFound() != _readyFragment->startIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FIRST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->firstFound()).toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(_readyFragment->lastFound() != _readyFragment->endIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_LAST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(_readyFragment->lastFound()).toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}
*/

#warning DEBUG ONLY	CODE !!!
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=debugTest")));
	part.setBody(QByteArray("here will come LOG command response"));
	_multipart->append(part);
	part = QHttpPart();
// DEBUG ONLY CODE  END

	qDebug() << "\tPosting new HTTP multipart send signal...";

	qDebug() << ((
					QMetaObject::invokeMethod(&_sender, "sendMultipart", Q_ARG(QHttpMultiPart *, _multipart))
				) ? "\tSucceeded" : "\tFailed")
				;

/*

	qDebug() << &multix;
	qDebug() << ((
					QMetaObject::invokeMethod(&_sender, "sendMultipart", Q_ARG(QHttpMultiPart *, &multix))
				) ? "\tSucceeded" : "\tFailed")
				;
*/

	processFragment((_readyFragment->nextFragment()));



}

void LogReader::processFragment(LogFragment *fragment) {
	_readyFragment = 0;
	if(!fragment) {
// IS IT OK TO RELY ON _lastFragment check in checkSending?
/*
		qDebug() << "*** fragment=NULL ->  deleteLater()";
		deleteLater();
*/
		_lastFragment = true;
		return;
	}
	qDebug() << "\tStarting processing of new fragment...";
	connect(fragment, &LogFragment::fragmentReady, this, &LogReader::onFragmentReady);
	connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
		qDebug() << "LogReader detected fragmentFailed on" << fragment;
		deleteLater();
	});
	QMetaObject::invokeMethod(fragment, "fillFragment");
}
