#ifndef EXTENDEDHTTPMULTIPART_H
#define EXTENDEDHTTPMULTIPART_H

#include <QHttpMultiPart>

class QFile;

class ExtendedHttpMultiPart : public QHttpMultiPart
{
public:
	explicit ExtendedHttpMultiPart(QObject *parent = 0);
	explicit ExtendedHttpMultiPart(ContentType contentType, QObject *parent = 0);
	~ExtendedHttpMultiPart();

	void appendFile(QString itemKey, QIODevice *device,
					QString fileName = QString(), QString contentTypeHeader = QStringLiteral("text/plain; charset=utf-8"));

	void appendFormData(QString itemKey, QString itemValue);
	void appendFormData(QString itemKey, QDateTime itemValue);
	void appendFormData(QString itemKey, QVariant itemValue);

};

#endif // EXTENDEDHTTPMULTIPART_H
