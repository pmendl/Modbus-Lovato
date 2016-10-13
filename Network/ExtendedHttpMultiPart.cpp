#include "ExtendedHttpMultiPart.h"

#include <QSharedPointer>
#include <QFile>
#include <QDateTime>

#include "DebugMacros.h"


ExtendedHttpMultiPart::ExtendedHttpMultiPart(QObject *parent) :
	QHttpMultiPart(parent)
{
}

ExtendedHttpMultiPart::ExtendedHttpMultiPart(ContentType contentType, QObject *parent) :
QHttpMultiPart(contentType, parent)
{
}

ExtendedHttpMultiPart::~ExtendedHttpMultiPart()
{
}

// --- ->*.h

void ExtendedHttpMultiPart::appendFile(QString itemKey, QFile *file, QString contentTypeHeader)
{
	appendFile(itemKey, file, contentTypeHeader, file->fileName());
}

void ExtendedHttpMultiPart::appendFile(QString itemKey, QFile *file, QString contentTypeHeader, QString fileName)
{
	if(contentTypeHeader.isEmpty())
		contentTypeHeader = QStringLiteral("text/plain; charset=utf-8");

	if(fileName.isEmpty())
		fileName = file->fileName();

	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(contentTypeHeader));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant(
					   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
					   .arg(itemKey)
					   .arg(fileName)
					   )
				   );
	part.setBodyDevice(file);
	append(part);
}

void ExtendedHttpMultiPart::appendFile(QString itemKey, QSharedPointer<QFile> file,
				QString contentTypeHeader, QString fileName)
{
	appendFile(itemKey, file.data(), contentTypeHeader, fileName);
}

void ExtendedHttpMultiPart::appendFile(QString itemKey, QSharedPointer<QFile> file,
				QString contentTypeHeader)
{
	appendFile(itemKey, file.data(), contentTypeHeader, file->fileName());
}

void ExtendedHttpMultiPart::appendFormData(QString itemKey, QString itemValue)
{
	QHttpPart textPart;
	textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name="))+itemKey);
	textPart.setBody(itemValue.toUtf8());
	append(textPart);
}

void ExtendedHttpMultiPart::appendFormData(QString itemKey, QDateTime itemValue) {
	appendFormData(itemKey, itemValue.toString());
}

void ExtendedHttpMultiPart::appendFormData(QString itemKey, QVariant itemValue) {
	appendFormData(itemKey, itemValue.toString());
}
