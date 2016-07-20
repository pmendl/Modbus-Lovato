#include "RequestManager.h"

#include <QDebug>
#include <QSettings>
#include <QRegularExpression>
#include <QTimerEvent>

#include "Globals.h"

RequestManager::itemType_t dataTypeFromString(QString s,
										  RequestManager::itemType_t deflt = RequestManager::uintType) {
	if(s.toLower() == xstr(REQUEST_DATA_TYPE_VALUE_FLOAT))
		return RequestManager::floatType;

	if(s.toLower() == xstr(REQUEST_DATA_TYPE_VALUE_DOUBLE))
		return RequestManager::doubleType;

	return deflt;
}

RequestManager::requestType_t requestTypeFromString(QString s) {
	if(s.toLower() == xstr(REQUEST_REQUEST_TYPE_VALUE_POST))
		return RequestManager::REQUEST_REQUEST_TYPE_VALUE_POST;

	if(s.toLower() == xstr(REQUEST_REQUEST_TYPE_VALUE_LOG))
		return RequestManager::REQUEST_REQUEST_TYPE_VALUE_LOG;

	return RequestManager::REQUEST_REQUEST_TYPE_VALUE_INVALID;
}

quint8 bytesPerType(RequestManager::itemType_t t, quint8 deflt = sizeof(quint16)) {
	switch (t) {
	case RequestManager::doubleType:
	case RequestManager::floatType:
		return t;

	default:
		return deflt;
	}

}

/**
 * @brief Constructs RequestManager instance based on currently opened settings
 * group. Instance gets named (QObject::objectName()) after the group name,
 * the name being <GroupName>RequestManager
 *
 * @param settings QSettings instance with current request group just opened
 * via (QSettings::beginGroup()).
 * @param parent
 */
RequestManager::RequestManager(QSettings &settings, QObject *parent) :
	QObject(parent),
	_command(0x03)
{
	setObjectName(REQUEST_MANAGER_NAME_PREFIX + QString(settings.group()).remove(QRegularExpression(".*/")));
	if((_active = settings.value(REQUEST_ACTIVITY_KEY, false).toBool())) {
		qDebug() << "Object" << objectName() << "is active.";

		_device = settings.value(REQUEST_DEVICE_KEY).toString().toUInt();
		_address = settings.value(REQUEST_ADDRESS_KEY).toString().toUInt();
		itemType_t type=dataTypeFromString(settings.value(REQUEST_DATA_TYPE_KEY).toString());
		quint8 bytesPerItem= settings.value(REQUEST_BYTES_PER_ITEM, bytesPerType(type)).toUInt();
		int arraySize = settings.beginReadArray(REQUEST_ARRAY_ITEM_KEY);
		_registerCount = settings.value(REQUEST_REGISTER_COUNT, arraySize*bytesPerItem/2).toUInt();

		for (int i = 0; i < arraySize; ++i) {
			dataItemDefinition_t item;
			settings.setArrayIndex(i);
			item._name = settings.value(REQUEST_ITEM_NAME_KEY).toString();
			item._pduOffset = 1+1+(settings.value(REQUEST_ITEM_PDU_INDEX_KEY,1).toUInt()-1)*bytesPerItem;
			item._pduOffset = settings.value(REQUEST_ITEM_PDU_OFFSET_KEY, item._pduOffset).toUInt();
			item._multiplier = settings.value(REQUEST_ITEM_MULTIPLIER_KEY,1.0).toDouble();
			item._divider = settings.value(REQUEST_ITEM_DIVIDER_KEY,1).toUInt();
			item._signumIndex = settings.value(REQUEST_ITEM_SIGNUM_INDEX_KEY,0).toInt();
			_itemDefinition.append(item);
		}
		settings.endArray();

		arraySize = settings.beginReadArray(REQUEST_ARRAY_REQUEST_KEY);
		for (int i = 0; i < arraySize; ++i) {
			requestDefinition_t request;
			settings.setArrayIndex(i);
			request.period = settings.value(REQUEST_REQUEST_PERIOD_KEY, 0).toInt();
			request.type = requestTypeFromString(settings.value(REQUEST_REQUEST_TYPE_KEY).toString());
			if((request.period > 0) && (request.type != invalid)) {
				QBasicTimer *timer=new QBasicTimer;
				timer->start(request.period, this),
				_requestDefinition.insert(timer->timerId(), request);
				qDebug() << "Timer started: id=" << timer->timerId() << ", period = " << request.period;
				_timers.append(QSharedPointer<QBasicTimer>(timer));
			}
			else {
				qDebug() << "\tInvalid request. type=" << request.type << ", period=" << request.period;
			}
		}
		settings.endArray();

	}
	else {
		qDebug() << "Object" << objectName() << "is INACTIVE.";
	}
}

bool RequestManager::isActive() const
{
	return _active;
}

void RequestManager::timerEvent(QTimerEvent *event) {
	qDebug() << "TIMER EVENT: id=" << event->timerId() << "-->" << objectName();
	requestDefinition_t operation(_requestDefinition.value(event->timerId()));
	qDebug() << "type =" << operation.type;
	PDUSharedPtr_t pdu(new ProtocolDataUnit(_command, _address, _registerCount));
	emit requesting(pdu);
}

void RequestManager::onResponse(PDUSharedPtr_t response) {
	qDebug() << "Response received: " << response->toHex();
}
