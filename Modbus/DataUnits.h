#ifndef DATAUNITS_H
#define DATAUNITS_H

#include <QByteArray>

#include <initializer_list>

#include "Globals.h"

class RequestManager;

class ProtocolDataUnit : public QByteArray
{
public:
	explicit ProtocolDataUnit(std::initializer_list<char> l);
	explicit ProtocolDataUnit(QByteArray byteArray);
	explicit ProtocolDataUnit(quint8 fn, quint16 address, quint8 regCount);
	virtual ~ProtocolDataUnit() {}
	virtual bool isValid() const;
	virtual qint16 bytesToRead() const;
	template <typename T> T extractAt(int i) const;
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
	quint16 computeCrc(qint16 adjustSize = 0);
	bool isCrcValid();
	virtual qint16 aduPrefixSize() const;
	virtual qint16 aduPostfixSize() const;
};

//------------------------------------------------------------------------------

template <typename T>
T ProtocolDataUnit::extractAt(int i) const {
	T x;
	char *tp(static_cast<char *>(static_cast<void*>(&x)));
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	const char *fp(data()+aduPrefixSize()+i+sizeof(T)-1);
	for(int count=sizeof(x); count--; --fp, ++tp) {
		*tp = *fp;
	}
#else
	const char *fp(data()+aduPrefixSize()+i);
	for(int count=sizeof(T); count--; ++fp, ++tp) {
		*tp = *fp;
	}
#endif
	return x;
}


#endif // DATAUNITS_H
