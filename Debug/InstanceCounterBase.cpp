#include "InstanceCounterBase.h"

#include "Debug/DebugMacros.h"



InstanceCounterBase::InstanceCounterBase(QString classId) :
	_classId(classId)
{
	instanceCount_t::iterator it(_instanceCount.find(_classId));
	if(it == _instanceCount.end()) {
		_instanceCount.insert(classId, 1);
	}
	else {
		++(it.value());
	}
}

InstanceCounterBase::~InstanceCounterBase()
{
	--(_instanceCount.find(_classId).value());
}

void InstanceCounterBase::print()
{
	for(instanceCount_t::iterator it = _instanceCount.begin(); it != _instanceCount.end(); ++it) {
		DP_INSTANCE_COUNTER_BASE("INSTANCE COUNT:" << it.key() << "=" <<it.value());
	}
}

 InstanceCounterBase::instanceCount_t InstanceCounterBase::_instanceCount;
