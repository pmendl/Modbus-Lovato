#ifndef DATAUNITS_H
#define DATAUNITS_H

#include <QByteArray>

#include <initializer_list>


class ProtocolDataUnit : public QByteArray
{
public:
	explicit ProtocolDataUnit(std::initializer_list<char> l);
	explicit ProtocolDataUnit(QByteArray byteArray);
	explicit ProtocolDataUnit(int reserve = 300);
	virtual ~ProtocolDataUnit() {}
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
	explicit ApplicationDataUnitSerial(quint8 address, ProtocolDataUnit pdu);
	explicit ApplicationDataUnitSerial(int reserve = 300);
	explicit ApplicationDataUnitSerial(QByteArray qba);
	virtual ~ApplicationDataUnitSerial() {}
	virtual qint16 aduPrefixSize();
	virtual qint16 aduPostfixSize();
};

#endif // DATAUNITS_H
