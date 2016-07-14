#include "RequestManager.h"

#include <QSettings>
#include <QDebug>

RequestManager::RequestManager(QString name, QObject *parent) : QObject(parent)
{
	QSettings settings;
	dataItemDefinition_t item;

	settings.beginGroup(name);

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

	settings.endGroup();
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
