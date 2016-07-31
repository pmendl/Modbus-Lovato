#include "LogReader.h"

#include <QDebug>

#include <cstring>

LogReader::LogReader(QString pathname, QDateTime from, QDateTime to) :
	_count(3)
{

}

bool LogReader::open(OpenMode mode) {
	qDebug() << "Attempt to open mode" << QString(QStringLiteral("0x%1")).arg(mode, 4, 16, QChar('0'));
	return QIODevice::open(mode);
}

bool LogReader::isSequential() const {
	qDebug() << "Called isSequential(); returning true";
	return true;
}

qint64 LogReader::readData(char *ptr, qint64 count) {
	qDebug() << "Attempt to read data; count=" << count;
	std::strcpy(ptr, "ABC");
	if(count--)
		return 3;
	else
		return -1;
}

qint64 LogReader::writeData(const char *, qint64 count) {
	qDebug() << "Attempt to write data; count=" << count;
	return 0;

}
