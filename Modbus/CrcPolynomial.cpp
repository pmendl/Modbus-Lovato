#include "CrcPolynomial.h"

#include <QByteArray>

/**
 * 16 bit CRC-polynomial calculation.
 */
/**
 * @class CrcPolynomial
 * This object calculates the checksum in accordance with the
 * CRC polynom given on construction. Data are supplied via the << operator,
 * result can be retrieved directly, via the uint8 operator.
 * @note Before each new set of values reset() method must be called
 * to properly initialize member variables.
 * <br><em>Constructor does this call for you itself before the very first sequence.<em>
 */


/**
 * @brief	Constructs object CRC-polynomial calculation with given polynom
 * @param	polynom Polynom representing the given CRC type.<br>
 *			<em> You get the value this way:<br>
 *			1) Create 17 (i.e. 16+1) bit value by placing binary 1 on position
 *			corresponding to each present exponent and 0 on each other position.<br>
 *			2) Shift the result right by one (loosing eventual +1 at the
 *			end of the polynom).<br>
 *			Example:  x^16 + x^12 + x^5 + 1 leads to 0x8408, which is standard
 *				polynom defined by CCITT.
 *				Another example would be Modbus protocol defining constant 0xA001.
 *
 * Constructs the object calculating checksum for any data,
 * supplied to it.
 */
CrcPolynomial::CrcPolynomial(quint16 polynom) :
	polynom(polynom)
{
	reset();
}

/**
 * @brief Reset object to calculate CRC for new data sequence.
 */
void CrcPolynomial::reset(void)
{
	_result = 0xFFFF;
}

/**
 * @brief Supply another data in the sequence CRC is calculated for.
 * @param input Data in the sequence
 * @return Reference to the object to allow for input data chaining
 */

CrcPolynomial& CrcPolynomial::operator << (quint8 input) {
	for (int i=0; i<8; ++i) {
		bool flag=(_result & 0x0001) ^ (input & 0x01);
		_result >>= 1;
		if(flag) _result ^= polynom;
		input >>= 1;
	}
	return *this;
}

/**
 * @brief Supply another data in the sequence CRC is calculated for.
 * @param input Array containing data in the sequence
 * @return Reference to the object to allow for input data chaining
 */
CrcPolynomial& CrcPolynomial::operator <<(QByteArray input) {
	for (int i=0; i<input.size(); ++i)
		*this << static_cast<quint8>(input.at(i));
	return *this;
}

/**
 * @brief Assigning object into quint16 actually assigns the resulting CRC.
 */
CrcPolynomial::operator quint16() const {
	return _result;
}
