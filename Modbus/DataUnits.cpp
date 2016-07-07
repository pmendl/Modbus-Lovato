#include "DataUnits.h"


ProtocolDataUnit::ProtocolDataUnit(std::initializer_list<char> l) :
	QByteArray(l.begin(), l.size())
{}

ApplicationDataUnitSerial::ApplicationDataUnitSerial(std::initializer_list<char> l) :
	QByteArray(l.begin(), l.size())
{}

ApplicationDataUnitSerial::ApplicationDataUnitSerial(quint8 address, ProtocolDataUnit pdu) :
	QByteArray(pdu)
{
	prepend(address);
}
