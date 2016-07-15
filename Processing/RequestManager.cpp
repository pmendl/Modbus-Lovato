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
RequestManager::RequestManager(QSettings &settings, QObject *parent) : QObject(parent)
{
	dataItemDefinition_t item;
	setObjectName(REQUEST_MANAGER_NAME_PREFIX + QString(settings.group()).remove(QRegularExpression(".*/")));
	if((_active = settings.value(REQUEST_ACTIVITY, false).toBool())) {

/*
	// Other configuration (like timing, URL...) will go here later

//	device = settings.value("device").toInt();
//	command = settings.value("command").toInt();

	int size = settings.beginReadArray("item");
	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		item.name = settings.value("name").toString();
		item.offset = settings.value("offset").toInt();
		item.size = settings.value("size").toInt();
		item.multiplier = settings.value("multiplier").toDouble();
		itemDefinition.append(item);
	}
	settings.endArray();
*/

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
