#ifndef PROCESSINGMANAGER_H
#define PROCESSINGMANAGER_H

#include <QObject>

#include "Globals.h"
#include "Modbus/ModbusSerialMaster.h"
#include "Log/LogServer.h"

class ProcessingManager : public QObject
{
	Q_OBJECT
public:
	explicit ProcessingManager(QObject *parent = 0);
	QSharedPointer<class ParsingProcessor> processor(class QSettings *settings, QString group = QStringLiteral(""));
	static QString objectNameFromGroup(QString prefix, QString group);
	QSharedPointer<LogServer> logServer() const;

signals:

public slots:
	void onQueryRequest();

private:
//	static QSharedPointer<class ModbusSerialMaster> _serialMaster;
	ModbusSerialMaster _serialMaster;
	QSharedPointer<LogServer> _logServer;
};

#endif // PROCESSINGMANAGER_H
