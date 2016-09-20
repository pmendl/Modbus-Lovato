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
	_runningReply(0),
	_postFileContent(postFileContent)
{
	start();

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(pathname)), postFileContent, id, from, to, group, 0, this));
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
	qDebug() << "LogReader has new fragment" << fragment << "ready for sending ...";
	_readyFragment = fragment;
	checkSending();
}

void LogReader::checkSending()
{
	if(_readyFragment != 0) {
		if(_runningReply.isNull() || (!_runningReply->isRunning()))
			sendReadyFragment();
	}
	else {
		if(_lastFragment)
			deleteLater();
	}
}




	qDebug() << "LogReader::onFragmentReady starts HTTP transmit ...";
	if(fragment == 0) {
		qDebug() << "LogReader::onFragmentReady called with zero pointer! ERROR!";
		return;
	}

	if(!fragment->open(QIODevice::ReadOnly)) {
		qDebug() << "LogReader::onFragmentReady aborts as it can not open fragment for reading...";
#warning DEBUG/RESTORE THE BELOW FUNCTIONALITY (LogReader destruction)
//		fragment->deleteLater();
		return;
	}

	QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType, fragment));
// PROBABLY INVALID:	fragment->setParent(multipart);

	QHttpPart part;
/*
	if(_postFileContent) {
		part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain; charset=utf-8"));
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QVariant(
						   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
						   .arg(POST_ELEMENT_LOG_FILE_NAME)
						   .arg(fragment->logfile()->fileName())
						   )
					   );
		part.setBodyDevice(fragment);
		multipart->append(part);
		part = QHttpPart();
	}

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
	part.setBody(QString(QStringLiteral("%1")).arg(fragment->startIndex()).toUtf8());
	multipart->append(part);
	part = QHttpPart();


	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=%1"))
					   .arg(POST_ELEMENT_LOG_END_INDEX_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(fragment->endIndex()).toUtf8());
	multipart->append(part);

	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QString(QStringLiteral("form-data; name=%1"))
				   .arg(POST_ELEMENT_LOG_RECORD_COUNT_NAME));
	part.setBody(QString(QStringLiteral("%1")).arg(fragment->recordCnt()).toUtf8());
	multipart->append(part);
	part = QHttpPart();

	if(fragment->firstFound() != fragment->startIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_FIRST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(fragment->firstFound()).toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}

	if(fragment->lastFound() != fragment->endIndex()) {
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
						   QString(QStringLiteral("form-data; name=%1"))
						   .arg(POST_ELEMENT_LOG_LAST_FOUND_NAME));
		part.setBody(QString(QStringLiteral("%1")).arg(fragment->lastFound()).toUtf8());
		multipart->append(part);
		part = QHttpPart();

	}
*/

#warning DEBUG ONLY	CODE !!!
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name=debugTest")));
	part.setBody(QByteArray("OK"));
	multipart->append(part);
	part = QHttpPart();
// DEBUG ONLY CODE  END

	qDebug() << "\tWaiting for last HTTP transmit to end...";
	_sender.wait();

	qDebug() << "\tPosting new HTTP multipart send signal...";

	_sender.sendMultipart(multipart);
/*
	qDebug() << ((
					QMetaObject::invokeMethod(&_sender, "sendMultipart", Q_ARG(QHttpMultiPart *, multipart))
				) ? "\tSucceeded" : "\tFailed")
				;
*/
/*

	qDebug() << &multix;
	qDebug() << ((
					QMetaObject::invokeMethod(&_sender, "sendMultipart", Q_ARG(QHttpMultiPart *, &multix))
				) ? "\tSucceeded" : "\tFailed")
				;
*/

	processFragment((fragment->nextFragment()));


	qDebug() << "LogReader finished HTTP transmit ...";

}

	void LogReader::processFragment(LogFragment *fragment) {
		if(!fragment) {
			deleteLater();
			return;
		}
		qDebug() << "\tStarting processing of new fragment...";
		connect(fragment, &LogFragment::fragmentReady, this, &LogReader::onFragmentReady);
		connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
			qDebug() << "LogReader detected fragmentFailed on" << fragment;
			_sender.wait();
			deleteLater();
		});
	}
