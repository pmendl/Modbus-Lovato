#ifndef CRCPOLYNOMIAL_H
#define CRCPOLYNOMIAL_H

#include <QtGlobal>

class CrcPolynomial
{
public:
	CrcPolynomial(quint16 polynom = 0xA001);
	void reset(void);
	CrcPolynomial& operator <<(quint8 input);
	CrcPolynomial& operator <<(QByteArray input);
	operator quint16() const;

protected:
	quint16 polynom;
	quint16 _result;
};

#endif // CRCPOLYNOMIAL_H
