#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QBasicTimer>

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

	typedef enum {
		REQUEST_REQUEST_TYPE_VALUE_POST = REQUEST_REQUEST_TYPE_CONSTANT_POST,
		REQUEST_REQUEST_TYPE_VALUE_LOG = REQUEST_REQUEST_TYPE_CONSTANT_LOG,
		invalid=0
	} requestType_t;

	typedef struct {
		QString _name;
		quint8 _pduOffset, _bytesPerItem;
		itemType_t type;

		double _multiplier;
		quint64 _divider;
		quint8 _signumIndex; // 1-based; 0 value means ignore/unset
	} dataItemDefinition_t;

	typedef struct {
		requestType_t type;
		int period;
	} requestDefinition_t;

	explicit RequestManager(class QSettings &settings, QObject *parent = 0);

	bool isActive() const;

signals:
	void requesting(PDUSharedPtr_t request);

public slots:
	void onResponse(PDUSharedPtr_t response);
	void timerEvent(QTimerEvent *event);

private:
	bool _active;
	quint8 _device;
	const quint8 _command; // Reserved for further extensions. Const 0x03 so far.
	quint16 _address;
	quint8 _registerCount;
	QList<dataItemDefinition_t> _itemDefinition;
	QHash<int, requestDefinition_t> _requestDefinition;
	QList<QSharedPointer<QBasicTimer>> _timers;
};

#endif // REQUESTMANAGER_H
