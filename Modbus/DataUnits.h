#ifndef DATAUNITS_H
#define DATAUNITS_H

#include <QByteArray>

#include <initializer_list>


class ProtocolDataUnit : public QByteArray
{
public:
	explicit ProtocolDataUnit(std::initializer_list<char> l);

};

class  ApplicationDataUnitSerial : public QByteArray
{
public:
	explicit ApplicationDataUnitSerial(std::initializer_list<char> l);
	explicit ApplicationDataUnitSerial(quint8 address, ProtocolDataUnit pdu);
	explicit ApplicationDataUnitSerial(int size = 0, char ch = 0);
	explicit ApplicationDataUnitSerial(QByteArray &qba);
};

#endif // DATAUNITS_H
