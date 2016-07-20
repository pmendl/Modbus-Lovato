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
	explicit ProtocolDataUnit(quint8 fn, quint16 address, quint8 regCount);
	virtual ~ProtocolDataUnit() {}
	virtual bool isValid() const;
	virtual qint16 bytesToRead() const;
	virtual char pduAt(int i) const;
	virtual qint16 aduPrefixSize() const;
	virtual qint16 aduPostfixSize() const;
protected:
	qint16 commandResolutionSize() const;
	qint16 commandResponseSize() const;

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
	virtual qint16 aduPrefixSize() const;
	virtual qint16 aduPostfixSize() const;
};

#endif // DATAUNITS_H
