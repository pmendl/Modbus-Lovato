#ifndef PRIORITIESCOUNTINGHASH_H
#define PRIORITIESCOUNTINGHASH_H

#include <QHash>

class PrioritiesCountingHash : public QHash<int,int>
{
public:
	void startPriority(int i);
	void endPriority(int i);

	int maxPriority() const;
	int minPriority() const;

protected:
	void rescanExtremes();
	void setExtremes(int i);

private:
	int _maxPriority, _minPriority;
};

#endif // PRIORITIESCOUNTINGHASH_H
