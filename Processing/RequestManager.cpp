#include "RequestManager.h"

#include <QDebug>
#include <QSettings>
#include <QRegularExpression>
#include <QTimerEvent>

#include "Globals.h"
#include "Processing/ParsingProcessor.h"

RequestManager::itemType_t dataTypeFromString(QString s,
											  RequestManager::itemType_t deflt = RequestManager::uintType) {
	if(s.toLower() == xstr(REQUEST_DATA_TYPE_VALUE_INT))
		return RequestManager::uintType;

	if(s.toLower() == xstr(REQUEST_DATA_TYPE_VALUE_FLOAT))
		return RequestManager::floatType;

	if(s.toLower() == xstr(REQUEST_DATA_TYPE_VALUE_DOUBLE))
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
	if((settings.value(REQUEST_ACTIVITY_KEY, false).toBool())) {
		qDebug() << "Object" << objectName() << "is active.";

		_device = settings.value(REQUEST_DEVICE_KEY).toString().toUInt(0,0);
		_address = settings.value(REQUEST_ADDRESS_KEY).toString().toUInt(0,0);
		itemType_t type=dataTypeFromString(settings.value(REQUEST_DATA_TYPE_KEY).toString());
		quint8 bytesPerItem= settings.value(REQUEST_BYTES_PER_ITEM, bytesPerType(type)).toUInt();
		int arraySize = settings.beginReadArray(REQUEST_ARRAY_ITEM_KEY);
		_registerCount = settings.value(REQUEST_REGISTER_COUNT, arraySize*bytesPerItem/2).toUInt();

		for (int i = 0; i < arraySize; ++i) {
			dataItemDefinition_t item;
			settings.setArrayIndex(i);
			item.name = settings.value(REQUEST_ITEM_NAME_KEY).toString();
			item.type = dataTypeFromString(settings.value(REQUEST_ITEM_DATA_TYPE_KEY).toString(), type);
			if(settings.contains(REQUEST_ITEM_BYTES_PER_ITEM))
				item.bytesPerItem = settings.value(REQUEST_ITEM_BYTES_PER_ITEM).toUInt();
			else
				item.bytesPerItem = bytesPerItem;

			item.pduOffset = settings.value(REQUEST_ITEM_PDU_OFFSET_KEY,
											1+1+(settings.value(REQUEST_ITEM_PDU_INDEX_KEY,1).toUInt()-1)*bytesPerItem
											).toUInt();
			item.multiplier = settings.value(REQUEST_ITEM_MULTIPLIER_KEY,1.0).toDouble();
			item.divider = settings.value(REQUEST_ITEM_DIVIDER_KEY,1).toUInt();
			if(!(item.signumKey = settings.value(REQUEST_ITEM_SIGNUM_INDEX_KEY,QString()).toString()).isEmpty())
			_signums.insert(item.signumKey, 1);
			_itemDefinitions.append(item);
		}
		settings.endArray();

		foreach(dataItemDefinition_t item, _itemDefinitions) {
			item.isSignumKey=_signums.contains(item.name);
		}


		arraySize = settings.beginReadArray(REQUEST_ARRAY_PARSING_KEY);
		for (int i = 0; i < arraySize; ++i) {
			settings.setArrayIndex(i);
			QSharedPointer<ParsingProcessor> processor = ParsingProcessor::processor(settings.value(REQUEST_PARSING_TYPE_KEY).toString(), &settings);
			if(processor != 0) {
				_parsingProcessors.append(processor);
			}
			else {
				qDebug() << "\tInvalid ParsingProcessor type required.";
			}
		}
		settings.endArray();

		int period = settings.value(REQUEST_PERIOD_KEY, 0).toInt();
		if(period > 0) {
			_timer.start(period, this),
			qDebug() << "Timer started:" << objectName() << "->" << period;
		}
		else {
			qDebug() << "Timer start failed! " << objectName() << "->" << period;
		}
	}
	else {
		qDebug() << "Object" << objectName() << "is INACTIVE.";
	}
}

void RequestManager::timerEvent(QTimerEvent *event) {
	(void) event;
	emit requesting();
}

quint8 RequestManager::device() const
{
	return _device;
}

PDUSharedPtr_t RequestManager::request() {
	return PDUSharedPtr_t(new ProtocolDataUnit(_command, _address, _registerCount));
}

QVariant RequestManager::item(dataItemDefinition_t def) {
	switch(def.type) {
	case uintType:
		switch(def.bytesPerItem) {
		case 1:
			return _response->extractAt<quint8>(def.pduOffset);
		case 2:
			return _response->extractAt<quint16>(def.pduOffset);
		case 4:
#warning SIGNUM TESTS - LAST MULTIPLIER MUST BE REMOVED AFTERWARDS
			return _response->extractAt<qint32>(def.pduOffset) * (def.name=="P3jal"?-1:1);
		case 8:
			return _response->extractAt<quint64>(def.pduOffset);
		}
		break;
	case floatType:
		return _response->extractAt<float>(def.pduOffset);
	case doubleType:
		return _response->extractAt<double>(def.pduOffset);
	}
	return QVariant();
}

void RequestManager::onResponse(PDUSharedPtr_t response) {
	qDebug() << "Response received: " << response->toHex();
	_response = response;
	qDebug() << "PARSING:";
#warning Should move to ParsingProcessor and needs "sign" key to be implemented
	foreach(dataItemDefinition_t def, _itemDefinitions) {
		QString s("%1 : %2 (offset=%3)");
		qDebug() << "\t" << s.arg(def.name)
					.arg(item(def).toDouble()*def.multiplier/def.divider*(def.signumKey.isEmpty()?1:_signums.value(def.signumKey, 1)))
					.arg(def.pduOffset);
	}
}
