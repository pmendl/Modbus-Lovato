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

void ExtendedHttpMultiPart::appendFile(QString itemKey, QIODevice *device, QString fileName, QString contentTypeHeader)
{
	if(contentTypeHeader.isEmpty())
		contentTypeHeader = QStringLiteral("text/plain; charset=utf-8");

	if(fileName.isEmpty()) {
		QFile *file(dynamic_cast<QFile *>(device));
		if(file != 0) {
			fileName = file->fileName();
		}
		else
			fileName="unknown_file";
	}

	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(contentTypeHeader));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant(
					   QString(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\""))
					   .arg(itemKey)
					   .arg(fileName)
					   )
				   );
	part.setBodyDevice(device);
	append(part);
}

void ExtendedHttpMultiPart::appendFormData(QString itemKey, QString itemValue)
{
	QHttpPart textPart;
	textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name="))+itemKey);
	textPart.setBody(itemValue.toUtf8());
	append(textPart);
}

/*
void ExtendedHttpMultiPart::appendFormData(QString itemKey, QDateTime itemValue) {
	appendFormData(itemKey, itemValue.toString());
}

void ExtendedHttpMultiPart::appendFormData(QString itemKey, QVariant itemValue) {
	appendFormData(itemKey, itemValue.toString());
}
*/

void ExtendedHttpMultiPart::appendFormData(itemPair_t pair) {}
void ExtendedHttpMultiPart::appendFormData(itemPairsList_t pairList) {}
