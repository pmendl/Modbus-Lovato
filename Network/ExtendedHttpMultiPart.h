#ifndef EXTENDEDHTTPMULTIPART_H
#define EXTENDEDHTTPMULTIPART_H

#include <QHttpMultiPart>

class QFile;

class ExtendedHttpMultiPart : public QHttpMultiPart
{
public:
	typedef QPair<QString, QVariant> itemPair_t;
	typedef QList<itemPair_t> itemPairsList_t;

	explicit ExtendedHttpMultiPart(QObject *parent = 0);
	explicit ExtendedHttpMultiPart(ContentType contentType, QObject *parent = 0);
	~ExtendedHttpMultiPart();

	void appendFile(QString itemKey, QIODevice *device,
					QString fileName = QString(), QString contentTypeHeader = QStringLiteral("text/plain; charset=utf-8"));


//	void appendFormData(QString itemKey, QDateTime itemValue);
//	void appendFormData(QString itemKey, QVariant itemValue);
	void appendFormData(itemPair_t pair);
	void appendFormData(itemPairsList_t pairList);

	template <typename T>
	void appendFormData(QString itemKey, T itemValue);

	void appendFormData(QString itemKey, QString itemValue);

};
/*
template <>
void ExtendedHttpMultiPart::appendFormData(QString itemKey, QString itemValue) {
	QHttpPart textPart;
	textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString(QStringLiteral("form-data; name="))+itemKey);
	textPart.setBody(itemValue.toUtf8());
	append(textPart);
}
*/

template <typename T>
void ExtendedHttpMultiPart::appendFormData(QString itemKey, T itemValue)
{
	appendFormData(itemKey, QVariant(itemValue).toString());
//	appendFormData(itemKey, itemValue.toString());
}



#endif // EXTENDEDHTTPMULTIPART_H
