#include "RequestManager.h"

#include <QDebug>
#include <QSettings>
#include <QRegularExpression>
#include <QTimerEvent>

#include "Globals.h"
#include "Processing/ProcessingManager.h"
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
RequestManager::RequestManager(QSettings &settings, ProcessingManager *processingManager) :
	QObject(processingManager),
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
			QSharedPointer<dataItemDefinition_t> item(new dataItemDefinition_t);
			settings.setArrayIndex(i);
			item->name = settings.value(REQUEST_ITEM_NAME_KEY).toString();
			item->type = dataTypeFromString(settings.value(REQUEST_ITEM_DATA_TYPE_KEY).toString(), type);
			if(settings.contains(REQUEST_ITEM_BYTES_PER_ITEM))
				item->bytesPerItem = settings.value(REQUEST_ITEM_BYTES_PER_ITEM).toUInt();
			else
				item->bytesPerItem = bytesPerItem;

			item->pduOffset = settings.value(REQUEST_ITEM_PDU_OFFSET_KEY,
											1+1+(settings.value(REQUEST_ITEM_PDU_INDEX_KEY,1).toUInt()-1)*bytesPerItem
											).toUInt();
			item->multiplier = settings.value(REQUEST_ITEM_MULTIPLIER_KEY,1.0).toDouble();
			item->divider = settings.value(REQUEST_ITEM_DIVIDER_KEY,1).toUInt();
			item->signumKey = settings.value(REQUEST_ITEM_SIGNUM_INDEX_KEY).toString();
			_itemDefinitions.append(item);
		}
		settings.endArray();


		arraySize = settings.beginReadArray(REQUEST_ARRAY_PARSING_KEY);
		for (int i = 0; i < arraySize; ++i) {
			settings.setArrayIndex(i);
			QSharedPointer<class ParsingProcessor> processor = processingManager->processor(&settings);
			if(!processor.isNull()) {
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

const QHash<QString, RequestManager::parsedItem_t> RequestManager::parsedItems() const
{
    return _parsedItems;
}

quint8 RequestManager::device() const
{
    return _device;
}

PDUSharedPtr_t RequestManager::request() {
	return PDUSharedPtr_t(new ProtocolDataUnit(_command, _address, _registerCount));
}

QVariant RequestManager::responseItemRaw(QSharedPointer<const dataItemDefinition_t> def) const {
	switch(def->type) {
	case uintType:
		switch(def->bytesPerItem) {
		case 1:
			return _response->extractAt<quint8>(def->pduOffset);
		case 2:
			return _response->extractAt<quint16>(def->pduOffset);
		case 4:
			return _response->extractAt<qint32>(def->pduOffset);
		case 8:
			return _response->extractAt<quint64>(def->pduOffset);
		}
		break;
	case floatType:
		return _response->extractAt<float>(def->pduOffset);
	case doubleType:
		return _response->extractAt<double>(def->pduOffset);
	case invalidType:
		break;
	}
	return QVariant();
}

QVariant RequestManager::responseItemParsed(QString name) const {
	return _parsedItems.value(name).value;
}

void RequestManager::onResponse(PDUSharedPtr_t response) {
/******************************************************************************
 * This stuff is intended for simulation of error responses only.
 * Uncomment only if you fully understand what you are doing !
 */

// --- NULL RESPONSE TEST ---
//	response.reset();

// --- ERROR RESPONSE TEST ---
//	response->operator [](1) = 0x83;
//	response->operator [](2) = 2;
/******************************************************************************/
	if(!response.isNull())
		qDebug() << "\tRESPONSE: " << response->toHex();

	_response = response;
	qDebug() << "PARSING:";
	_parsedItems.clear();
	parsedItem_t item;

/******************************************************************************
 * Key responseType removed from other then null response postings
 * as it is context-deducible.
 *
 * To make it appear again comment following #define line.
 ******************************************************************************/
#define NO_RESPONSE_TYPE_KEY_USED

#ifndef NO_RESPONSE_TYPE_KEY_USED
	const QSharedPointer<const dataItemDefinition_t> responseType(
				new const dataItemDefinition_t({PARSED_ITEM_RESPONSE_TYPE_KEY,
				  0,0,
				  RequestManager::invalidType,
				  0., 1, QString()
				 })
				);
	item.def = responseType;
	item.raw = 0;
#endif
	// NULL RESPONSE
	if(response.isNull()) {
#ifdef NO_RESPONSE_TYPE_KEY_USED
		const QSharedPointer<const dataItemDefinition_t> responseType(
					new const dataItemDefinition_t({PARSED_ITEM_RESPONSE_TYPE_KEY,
					  0,0,
					  RequestManager::invalidType,
					  0., 1, QString()
					 })
					);
		item.def = responseType;
		item.raw = 0;
#endif
		item.value = QStringLiteral(PARSED_ITEM_RESPONSE_TYPE_NULL_RESPONSE_VALUE);
		_parsedItems.insert(PARSED_ITEM_RESPONSE_TYPE_KEY, item);

	}
	// Command 0x03 hardwired for now; can get reimplemented more flexible later
	// ERROR RESPONSE
	else if(response->extractAt<char>(0) != 0x03) {
#ifndef NO_RESPONSE_TYPE_KEY_USED
		item.value = QStringLiteral(PARSED_ITEM_RESPONSE_TYPE_ERROR_RESPONSE_VALUE);
		_parsedItems.insert(PARSED_ITEM_RESPONSE_TYPE_KEY, item);
#endif

		const QSharedPointer<const dataItemDefinition_t> errResponseCode(
				new const dataItemDefinition_t({PARSED_ITEM_ERROR_RESPONSE_CODE_KEY,
				  0,1,
				  RequestManager::uintType,
				  1., 1, QString()
				 })
					);

		const QSharedPointer<const dataItemDefinition_t> errResponseException(
					new const dataItemDefinition_t({PARSED_ITEM_ERROR_RESPONSE_EXCEPTION_KEY,
					  1,1,
					  RequestManager::uintType,
					  1., 1, QString()
					 })
					);

		parsedItem_t item;
		item.def = errResponseCode;
		item.raw = responseItemRaw(errResponseCode);
		QString convert(QStringLiteral("0x%1"));
		item.value = convert.arg(item.raw.toUInt(),2,16,QChar('0'));
		_parsedItems.insert(PARSED_ITEM_ERROR_RESPONSE_CODE_KEY, item);

		item.def = errResponseException;
		item.raw = responseItemRaw(errResponseException);
		convert = QStringLiteral("0x%1");
		item.value = convert.arg(item.raw.toUInt(),2,16,QChar('0'));
		_parsedItems.insert(PARSED_ITEM_ERROR_RESPONSE_EXCEPTION_KEY, item);
	}
	// NORMAL RESPONSE
	else {
#ifndef NO_RESPONSE_TYPE_KEY_USED
		item.value = QStringLiteral(PARSED_ITEM_RESPONSE_TYPE_NORMAL_RESPONSE_VALUE);
		_parsedItems.insert(PARSED_ITEM_RESPONSE_TYPE_KEY, item);
#endif
		foreach(QSharedPointer<dataItemDefinition_t> def, _itemDefinitions) {
			item.def = def;
			item.raw = responseItemRaw(def);
			item.value = responseItemRaw(def).toDouble()*def->multiplier/def->divider *
						 (((!(def->signumKey.isEmpty())) && (responseItemParsed(def->signumKey) < 0))?-1:1);
			_parsedItems.insert(def->name, item);
		}

	}

	foreach (parsedItem_t item, _parsedItems.values()) {
		QString s("%1 : %2 (offset=%3)");

		if (item.def->name == PARSED_ITEM_RESPONSE_TYPE_KEY)
			s=s;

		qDebug() << "\t" << s.arg(item.def->name)
					.arg(item.value.toString())
					.arg(item.def->pduOffset);
	}


	foreach (parsedItem_t item, _parsedItems.values()) {
		QString s("%1 : %2 (offset=%3)");

		if (item.def->name == PARSED_ITEM_RESPONSE_TYPE_KEY)
			s=s;

		qDebug() << "\t" << s.arg(item.def->name)
					.arg(item.value.toString())
					.arg(item.def->pduOffset);
	}

	foreach (QSharedPointer<ParsingProcessor> processor, _parsingProcessors) {
		processor->process(this);
	}
}
