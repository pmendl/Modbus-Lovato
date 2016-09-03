#ifndef LOGFRAGMENT_H
#define LOGFRAGMENT_H

#include <QBuffer>
#include <QDateTime>
#include <QSharedPointer>

class QFile;

class LogFragment : public QBuffer
{
	Q_OBJECT

public slots:
	void fillFragment(void);

signals:
	void fragmentReady(LogFragment *);
	void fragmentFailed(LogFragment *);

public:
	LogFragment(QSharedPointer<QFile> logfile, QString id,
				QDateTime from = QDateTime(), QDateTime to = QDateTime(),
				QString group = QString(), QObject *parent = 0);

	LogFragment(QSharedPointer<QFile> logfile,
			QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			QString group = QString(), QObject *parent = 0);


	LogFragment(QSharedPointer<QFile> logfile,
			QString id,
			QString group, QObject *parent = 0);

	LogFragment *nextFragment();
//	QBuffer *pullBuffer();

/* Getters */
	QDateTime from() const;
	QDateTime to() const;
	QString group() const;
	QString id() const;
	QSharedPointer<QFile> logfile() const;
	qint64 startIndex() const;
	qint64 endIndex() const;
	qint64 firstFound() const;
	qint64 lastFound() const;
	bool lastFragment() const;

private:
	QSharedPointer<QFile> _logFile;
	QDateTime _from, _to;
	QString _group, _id;
	qint64 _startIndex, _endIndex, _firstFound, _lastFound, _recordCnt;
	bool _lastFragment;



};

#endif // LOGFRAGMENT_H
