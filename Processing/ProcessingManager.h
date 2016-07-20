#ifndef PROCESSINGMANAGER_H
#define PROCESSINGMANAGER_H

#include <QObject>

#include "Globals.h"


class ProcessingManager : public QObject
{
	Q_OBJECT
public:
	explicit ProcessingManager(QObject *parent = 0);

signals:

public slots:
	void onRequest(PDUSharedPtr_t request);
};

#endif // PROCESSINGMANAGER_H
