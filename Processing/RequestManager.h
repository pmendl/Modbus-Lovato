#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QList>

class RequestManager : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		uintType=sizeof(quint16),
		floatType=sizeof(float),
		doubleType=sizeof(double)
	} itemType_t;

	typedef struct {
		QString _name;
		quint8 _pduOffset, _bytesPerItem;
		itemType_t type;

		double _multiplier;
		quint64 _divider;
		quint8 _signumIndex; // 1-based; 0 value means ignore/unset
	} dataItemDefinition_t;

	explicit RequestManager(class QSettings &settings, QObject *parent = 0);

	bool isActive() const;

signals:

public slots:

private:
	bool _active;
	quint8 _device;
	const quint8 _command; // Reserved for further extensions. Const 0x03 so far.
	quint16 _address;
	quint8 _registerCount;
	QList<dataItemDefinition_t> _itemDefinition;
};

#endif // REQUESTMANAGER_H
