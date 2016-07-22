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

		_device = settings.value(REQUEST_DEVICE_KEY).toString().toUInt();
		_address = settings.value(REQUEST_ADDRESS_KEY).toString().toUInt();
		itemType_t type=dataTypeFromString(settings.value(REQUEST_DATA_TYPE_KEY).toString());
		quint8 bytesPerItem= settings.value(REQUEST_BYTES_PER_ITEM, bytesPerType(type)).toUInt();
		int arraySize = settings.beginReadArray(REQUEST_ARRAY_ITEM_KEY);
		_registerCount = settings.value(REQUEST_REGISTER_COUNT, arraySize*bytesPerItem/2).toUInt();

		for (int i = 0; i < arraySize; ++i) {
			dataItemDefinition_t item;
			settings.setArrayIndex(i);
			item.name = settings.value(REQUEST_ITEM_NAME_KEY).toString();
			item.pduOffset = 1+1+(settings.value(REQUEST_ITEM_PDU_INDEX_KEY,1).toUInt()-1)*bytesPerItem;
			item.pduOffset = settings.value(REQUEST_ITEM_PDU_OFFSET_KEY, item.pduOffset).toUInt();
			item._multiplier = settings.value(REQUEST_ITEM_MULTIPLIER_KEY,1.0).toDouble();
			item.divider = settings.value(REQUEST_ITEM_DIVIDER_KEY,1).toUInt();
			item.signumIndex = settings.value(REQUEST_ITEM_SIGNUM_INDEX_KEY,0).toInt();
			_itemDefinitions.append(item);

		}
		settings.endArray();

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
//	qDebug() << "TIMER EVENT: id=" << event->timerId() << "-->" << objectName();
//	PDUSharedPtr_t pdu(new ProtocolDataUnit(_command, _address, _registerCount));
	emit requesting();
}

quint8 RequestManager::device() const
{
	return _device;
}

PDUSharedPtr_t RequestManager::request() {
	return PDUSharedPtr_t(new ProtocolDataUnit(_command, _address, _registerCount));
}

void RequestManager::onResponse(PDUSharedPtr_t response) {
	qDebug() << "Response received: " << response->toHex();
	qDebug() << "PARSING:";

	foreach(dataItemDefinition_t def, _itemDefinitions) {

#warning Needs to respect type later !!!
		quint32 i;
		response->extractFromPdu(def.pduOffset, i);

		qDebug() << "\t" << def.name << ":" << i << "(offset=" << def.pduOffset << ")";
/*
		char *fp(static_cast<char *>(static_cast<void*>(&f)));
		if(def.pduOffset == 2) {
			for(int i=sizeof(f); i--; fp++) {
//				qDebug() << "\t\t" << i << ":" << static_cast<uint>(*fp);
				qDebug() << "\t\t" << i << ":" << QString("%1").arg(*fp, 2, 16, QChar('0'));
			}
		}
*/
	}
}
