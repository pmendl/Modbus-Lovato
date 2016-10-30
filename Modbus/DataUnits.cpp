#include "DataUnits.h"

#include "DebugMacros.h"
#include <QtGlobal>
#include <QtEndian>

#include "CrcPolynomial.h"

// ----------------------- Modbus PDU --------------------------------------

ProtocolDataUnit::ProtocolDataUnit(std::initializer_list<char> l) :
	QByteArray(l.begin(), l.size())
{}

ProtocolDataUnit::ProtocolDataUnit(QByteArray byteArray) :
	QByteArray(byteArray)
{}

ProtocolDataUnit::ProtocolDataUnit(quint8 fn, quint16 address, quint8 regCount) {
	switch(fn) {
	case 0x03:
		append(fn);
		append(address >> 8);
		append(address & 0xFF);
		append(static_cast<char>(0));
		append(regCount);
		break;
	}
}

ProtocolDataUnit::~ProtocolDataUnit() {}


qint16 ProtocolDataUnit::commandResolutionSize() const
{
	switch(extractAt<char>(0)) {
	case 0x03:
		return 1+1;

	case 0x83:
		return 0;

	default:
		return -1;
	}

}

qint16 ProtocolDataUnit::commandResponseSize()  const
{
		switch(extractAt<char>(0)) {
	case 0x03:
		return 1+1+extractAt<char>(1);

	case 0x83:
		return 1;

	default:
		return -1;
	}
}

bool ProtocolDataUnit::isValid() const
{
	return bytesToRead() == 0;
}

qint16 ProtocolDataUnit::bytesToRead(void) const
{
	if(size() <= aduPrefixSize())
		return aduPrefixSize()+1-size();

	if(size() <= commandResolutionSize())
		return aduPrefixSize()+commandResolutionSize()-size();

	qint16 s(commandResponseSize());
	if(s < 0)
		return s;
	else
		return aduPrefixSize()+s+aduPostfixSize()-size();

}

qint16 ProtocolDataUnit::aduPrefixSize() const
{
	return 0;
}

qint16 ProtocolDataUnit::aduPostfixSize() const
{
	return 0;
}

// ----------------------- Modbus ADU - serial -----------------------------

ApplicationDataUnitSerial::ApplicationDataUnitSerial(std::initializer_list<char> l) :
	ProtocolDataUnit(l)
{}

ApplicationDataUnitSerial::ApplicationDataUnitSerial(quint8 address, PDUSharedPtr_t pdu) :
	ProtocolDataUnit(*pdu)
{
	prepend(address);
	quint16 crc(computeCrc());
	append(crc & 0xFF);
	append(crc >> 8);
}

ApplicationDataUnitSerial::ApplicationDataUnitSerial(QByteArray qba) :
	ProtocolDataUnit(qba)
{}

ApplicationDataUnitSerial::~ApplicationDataUnitSerial() {}

bool ApplicationDataUnitSerial::isValid() {
	return (bytesToRead() == 0) && isCrcValid() ;
}

quint16 ApplicationDataUnitSerial::computeCrc(qint16 adjustSize) {
	CrcPolynomial crc;
	crc << left(size()+adjustSize);
	return crc;
}


bool ApplicationDataUnitSerial::isCrcValid() {
	return ((computeCrc(-2) >> 8) == at(size()-1)) && ((computeCrc(-2) & 0xFF) == at(size()-2));
}


qint16 ApplicationDataUnitSerial::aduPrefixSize() const
{
	return 1;
}

qint16 ApplicationDataUnitSerial::aduPostfixSize() const
{
	return 2;
}
