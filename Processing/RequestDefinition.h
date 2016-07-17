#ifndef REQUESTDEFINITION_H
#define REQUESTDEFINITION_H

typedef struct {
	QString _name;
	quint8 _pduOffset, _bytesPerItem;
	quint8 _size;
	double _multiplier;
	quint64 _divider;
} dataItemDefinition_t;

#endif // REQUESTDEFINITION_H
