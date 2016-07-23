#ifndef PROCESSINGMANAGER_H
#define PROCESSINGMANAGER_H

#include <QObject>
#include <QQueue>

#include "Globals.h"


class ProcessingManager : public QObject
{
	Q_OBJECT
public:
	explicit ProcessingManager(QObject *parent = 0);
	static QSharedPointer<class ParsingProcessor> processor(class QSettings *settings);

signals:

public slots:
	void onQueryRequest();

private:
	static QSharedPointer<class ModbusSerialMaster> _serialMaster;
};

#endif // PROCESSINGMANAGER_H
