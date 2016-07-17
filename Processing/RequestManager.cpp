#include "RequestManager.h"

#include <QSettings>
#include <QRegularExpression>
#include <QDebug>

#include "Globals.h"

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
	dataItemDefinition_t item;
	setObjectName(REQUEST_MANAGER_NAME_PREFIX + QString(settings.group()).remove(QRegularExpression(".*/")));
	if((_active = settings.value(REQUEST_ACTIVITY, false).toBool())) {

	_device = settings.value("device").toString().toInt();
	_address = settings.value("address").toString().toInt();

	int size = settings.beginReadArray("item");
	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		item._name = settings.value("name").toString();
		item._pduOffset = settings.value("offset").toInt();
		item._multiplier = settings.value("multiplier").toDouble();
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
