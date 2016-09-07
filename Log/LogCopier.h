#ifndef LOGCOPIER_H
#define LOGCOPIER_H

#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QSharedPointer>

#include "Network/NetworkSender.h"
#include "Log/LogFragment.h"


class LogCopier : public QThread
{
	Q_OBJECT

public:
	LogCopier(QString sourceFile, QString targetFile,
			  QDateTime from = QDateTime(), QDateTime to = QDateTime(),
			  QString group = QString(), QObject *parent = 0);
	LogCopier(QString sourceFile, QString targetFile,
			  QString group, QObject *parent = 0);
	virtual ~LogCopier();

protected slots:
	void onFragmentReady(LogFragment *fragment);

protected:
	void processFragment(LogFragment *fragment);

private:
	QFile _targetFile;
};

#endif // LOGCOPIER_H
