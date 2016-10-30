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

	template <typename T>
	void appendFormData(QString itemKey, T itemValue);

	void appendFormData(itemPair_t pair);
	void appendFormData(itemPairsList_t pairList);

	static void appendToGlobalData(QString itemKey, QVariant itemValue);
	void appendFromGlobalData();


private:
	static itemPairsList_t _globalHttpPairs;

};

template <typename T>
void ExtendedHttpMultiPart::appendFormData(QString itemKey, T itemValue)
{
	appendFormData<QString>(itemKey, QVariant(itemValue).toString());
}

template<>
void ExtendedHttpMultiPart::appendFormData<QString>(QString itemKey, QString itemValue);

#endif // EXTENDEDHTTPMULTIPART_H
