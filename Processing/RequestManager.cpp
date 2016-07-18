#include "RequestManager.h"

#include <QSettings>
#include <QRegularExpression>
#include <QDebug>

#include "Globals.h"

RequestManager::itemType_t typeFromString(QString s,
										  RequestManager::itemType_t deflt = RequestManager::uintType) {
	if(s.toLower() == "float")
		return RequestManager::floatType;

	if(s.toLower() == "double")
		return RequestManager::doubleType;

	return deflt;
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

	_device = settings.value(REQUEST_DEVICE_KEY).toString().toUInt();
	_address = settings.value(REQUEST_ADDRESS_KEY).toString().toUInt();
	itemType_t type=typeFromString(settings.value(REQUEST_DATA_TYPE_KEY).toString());
	quint8 bytesPerItem= settings.value(REQUEST_BYTES_PER_ITEM, bytesPerType(type)).toUInt();
	int arraySize = settings.beginReadArray(REQUEST_ARRAY_KEY);
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

		qDebug() << "Object" << objectName() << "is active.";
	}
	else {
		qDebug() << "Object" << objectName() << "is INACTIVE.";
	}
/*
	foreach(item, itemDefinition) {
		qDebug()
				<< "name=" << item.name
				<< "position=" << item.position
				<< "size=" << item.size
				<< "multiplier=" << item.multiplier * 100
				   ;
	}
*/
}

bool RequestManager::isActive() const
{
	return _active;
}
