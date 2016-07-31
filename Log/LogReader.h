#ifndef LOGREADER_H
#define LOGREADER_H

#include <QDateTime>
#include <QIODevice>

class LogReader : public QIODevice
{
public:
	LogReader(QString pathname, QDateTime from = QDateTime(), QDateTime to = QDateTime());
	bool open(OpenMode mode);
	virtual bool isSequential() const;
	virtual qint64 readData(char *ptr, qint64 count);
	virtual qint64 writeData(const char *, qint64);

private:
	int _count;
};

#endif // LOGREADER_H
