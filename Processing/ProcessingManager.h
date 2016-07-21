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

signals:

public slots:
	void onRequest();

private:
	QQueue<ADUSharedPtr_t> _queue;
};

#endif // PROCESSINGMANAGER_H
