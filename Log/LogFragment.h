#ifndef LOGFRAGMENT_H
#define LOGFRAGMENT_H

#include <QBuffer>
#include <QDateTime>

class LogFragment : public QBuffer
{
	Q_OBJECT

public:
	LogFragment(QString id,
				QDateTime from = QDateTime(), QDateTime to = QDateTime(),
				QString group = QString(), QObject *parent = 0);

	LogFragment(
			QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			QString group = QString(), QObject *parent = 0);


	LogFragment(
			QString id,
			QString group, QObject *parent = 0);

	QBuffer *pullBuffer();

/* Getters */
	QDateTime from() const;
	QDateTime to() const;
	QString group() const;
	QString id() const;
/* Public members */
	qint64 startIndex, endIndex;
	bool lastFragment;

private:
	QDateTime _from, _to;
	QString _group, _id;



};

#endif // LOGFRAGMENT_H
