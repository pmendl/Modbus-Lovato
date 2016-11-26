#include "System/PrioritiesCountingHash.h"

#include "Debug/DebugMacros.h"

void PrioritiesCountingHash::startPriority(int i)
{
	if(this->isEmpty()) {
		_maxPriority=_minPriority=i;
		this->insert(i,1);
		return;
	}

	int n(this->value(i, 0)+1);
	this->insert(i,n);

	if(i < _minPriority) _minPriority = i;
	else if (i > _maxPriority) _maxPriority = i;
}

void PrioritiesCountingHash::endPriority(int i)
{
	if(this->isEmpty()) {
		DP_SHOULD_NOT_OCCURE_ERROR("PrioritiesCountingHash::endPriority called while priorities list is empty");
		return;
	}

	int n(this->value(i, 0)-1);
	if (n < 0) {
		DP_SHOULD_NOT_OCCURE_ERROR("PrioritiesCountingHash::endPriority called while priorities list does not contain given priority");
	}
	else if (n == 0) {
		this->remove(i);
		rescanExtremes();
	}
	else {
		this->insert(i,n);
	}

}

void PrioritiesCountingHash::rescanExtremes() {
	for(QHash::const_iterator it = this->begin(); it != this->end(); ++it) {
		int priority(it.key());
		if(priority < _minPriority) _minPriority = priority;
		if(priority > _maxPriority) _maxPriority = priority;
	}
}

int PrioritiesCountingHash::maxPriority() const
{
	return _maxPriority;
}

int PrioritiesCountingHash::minPriority() const
{
	return _minPriority;
}

