#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QBasicTimer>
#include <QVariant>

#include "Globals.h"
#include "Modbus/DataUnits.h"

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
		QString name;
		quint8 pduOffset, bytesPerItem;
		itemType_t type;
		double multiplier;
		quint64 divider;
		QString signumKey;
		bool isSignumKey;
	} dataItemDefinition_t;

	typedef struct {
		QSharedPointer<dataItemDefinition_t> def;
		QVariant raw;
		QVariant value;
	} parsedItem_t;

	explicit RequestManager(class QSettings &settings, QObject *parent = 0);
	quint8 device() const;
	PDUSharedPtr_t request();
	QVariant item(QSharedPointer<dataItemDefinition_t> def);

signals:
	void requesting();

public slots:
	void onResponse(PDUSharedPtr_t response);

protected slots:
	void timerEvent(QTimerEvent *event);

private:
	quint8 _device;
	const quint8 _command; // Reserved for further extensions. Const 0x03 so far.
	quint16 _address;
	quint8 _registerCount;
	QList<QSharedPointer<dataItemDefinition_t>> _itemDefinitions;
	QList<QSharedPointer<class ParsingProcessor>> _parsingProcessors;
	QHash<QString, quint8> _signums;
	PDUSharedPtr_t _response;
	QBasicTimer _timer;
	QHash<QString, parsedItem_t> _parsedItems;
};

#endif // REQUESTMANAGER_H
