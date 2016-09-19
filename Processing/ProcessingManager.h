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
	explicit ProcessingManager(QObject *parent = 0, bool suppressPeriodicalRequesting = false);
	QSharedPointer<class ParsingProcessor> processor(class QSettings *settings, QString group = QStringLiteral(""));
	static QString objectNameFromGroup(QString prefix, QString group);
	QSharedPointer<LogServer> logServer() const;

	bool suppressPeriodicalRequesting() const;
	void setSuppressPeriodicalRequesting(bool suppressPeriodicalRequesting);

signals:

public slots:
	void onQueryRequest();

private:
	ModbusSerialMaster _serialMaster;
	QSharedPointer<LogServer> _logServer;
	bool _suppressPeriodicalRequesting;
};

#endif // PROCESSINGMANAGER_H
