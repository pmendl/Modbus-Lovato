#ifndef DATAUNITS_H
#define DATAUNITS_H

#include <QByteArray>

#include <initializer_list>

#include "Globals.h"


class ProtocolDataUnit : public QByteArray
{
public:
	explicit ProtocolDataUnit(std::initializer_list<char> l);
	explicit ProtocolDataUnit(QByteArray byteArray);
	virtual ~ProtocolDataUnit() {}
	virtual bool isValid();
	virtual qint16 bytesToRead();
	virtual qint16 aduPrefixSize();
	virtual qint16 aduPostfixSize();
protected:
	qint16 commandResolutionSize();
	qint16 commandResponseSize();

};

class  ApplicationDataUnitSerial : public ProtocolDataUnit
{
public:
	explicit ApplicationDataUnitSerial(std::initializer_list<char> l);
	explicit ApplicationDataUnitSerial(quint8 address, PDUSharedPtr_t pdu);
	explicit ApplicationDataUnitSerial(QByteArray qba = QByteArray());
	virtual ~ApplicationDataUnitSerial() {}
	virtual bool isValid();
	quint16 crc(qint16 adjustSize);
	bool isCrcValid();
	virtual qint16 aduPrefixSize();
	virtual qint16 aduPostfixSize();
};

#endif // DATAUNITS_H
