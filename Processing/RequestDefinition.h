#ifndef REQUESTDEFINITION_H
#define REQUESTDEFINITION_H

typedef struct {
	QString name;
	quint8 offset;
	quint8 size;
	double multiplier;
	quint64 divider;
} dataItemDefinition_t;

#endif // REQUESTDEFINITION_H
