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

	typedef struct {
		QString name;
		quint8 pduOffset, bytesPerItem;
		itemType_t type;
		double _multiplier;
		quint64 divider;
		quint8 signumIndex; // 1-based; 0 value means ignore/unset
	} dataItemDefinition_t;

	explicit RequestManager(class QSettings &settings, QObject *parent = 0);
	PDUSharedPtr_t request();
	quint8 device() const;

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
	QList<dataItemDefinition_t> _itemDefinitions;
	QList<QSharedPointer<class ParsingProcessor>> _parsingProcessors;
	QBasicTimer _timer;
};

#endif // REQUESTMANAGER_H
