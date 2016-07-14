#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QList>

#include "Processing/RequestDefinition.h"

class RequestManager : public QObject
{
	Q_OBJECT
public:
	explicit RequestManager(QString name, QObject *parent = 0);

signals:

public slots:

private:
	QList<dataItemDefinition_t> itemDefinition;
};

#endif // REQUESTMANAGER_H
