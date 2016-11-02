#ifndef INSTANCECOUNTERBASE_H
#define INSTANCECOUNTERBASE_H

#include <QString>
#include <QHash>

class InstanceCounterBase
{
public:
	InstanceCounterBase(QString classId);
	virtual ~InstanceCounterBase();

	static void print();

private:
	const QString _classId;

	typedef QHash<QString, int> instanceCount_t;
	static instanceCount_t _instanceCount;

};

#endif // INSTANCECOUNTERBASE_H
