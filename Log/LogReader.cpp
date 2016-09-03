#include "LogReader.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QBuffer>
//#include<QMetaObject>

#include "Network/NetworkSender.h"

#include "Globals.h"

extern void postFile(NetworkSender *sender);
//extern NetworkSender ns;

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
//	_logFile(pathname),
//	_logFragment(new LogFragment(QSharedPointer<QFile>(new QFile(pathname)), id, from, to, group, this)),
	_sender(new NetworkSender(this, url))
{
//	connect(this, &QThread::finished, this, &LogReader::onFinished);
	start();

	processFragment(new LogFragment(QSharedPointer<QFile>(new QFile(pathname)), id, from, to, group, this));
	qDebug() << "LogReader" << pathname << "constructed.";
}

void LogReader::processFragment(LogFragment *fragment) {
	if(!fragment) return;
	fragment->moveToThread(this);
	connect(fragment, &LogFragment::fragmentReady, this, &LogReader::onFragmentReady);
	connect(fragment, &LogFragment::fragmentFailed, [this](LogFragment *fragment){
		qDebug() << "LogReader detected fragmentFailed on" << fragment;
		_sender.wait();
		deleteLater();
	});
	qDebug() << ((
					QMetaObject::invokeMethod(fragment, "fillFragment")
				) ? "\tLogReader::processFragment succeeded" : "\tLogReader::processFragment failed")
				;
}

LogReader::~LogReader() {
	qDebug() << "LogReader destroyed.";
}

/*
bool LogReader::isValid() {
	return _logFragment->logfile()->isOpen();
}
*/
/*
QString LogReader::url() const
{
	return _url;
}
*/

/*
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
*/

void LogReader::onFragmentReady(LogFragment *fragment)
{
	qDebug() << "LogReader::onFragmentReady starts HTTP transmit ...";
	if(fragment == 0) {
		qDebug() << "LogReader::onFragmentReady called with zero pointer! ERROR!";
		return;
	}


	if(!fragment->open(QIODevice::ReadOnly)) {
		qDebug() << "LogReader::onFragmentReady aborts as it can not open fragment for reading...";
		fragment->deleteLater();
		return;
	}

	QHttpMultiPart *multipart(new QHttpMultiPart(QHttpMultiPart::FormDataType));
	fragment->setParent(multipart);

	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant( "text/plain"));
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

	qDebug() << "\tWaiting for last HTTP transmit to end...";
	_sender.wait();

	qDebug() << "\tPosting new HTTP multipart send signal...";
	qDebug() << ((
					QMetaObject::invokeMethod(&_sender, "sendMultipart", Q_ARG(QHttpMultiPart *,multipart))
				) ? "\tSucceeded" : "\tFailed")
				;

	qDebug() << "\tStarting processing of new fragment...";

	processFragment((fragment->nextFragment()));


	qDebug() << "LogReader finished HTTP transmit ...";

}
