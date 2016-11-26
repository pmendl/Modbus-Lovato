#ifndef RESET_H
#define RESET_H

#include <QObject>
#include <QEvent>


namespace System {

void initiateReset(void);
bool startResetSensitiveProcess(int priority);
void endResetSensitiveProcess(int priority);

} // System

class InitiateResetEvent : public QEvent
{
public:
	InitiateResetEvent();

};


class InitiateResetEventFilter : public QObject
{
	Q_OBJECT

protected:
	virtual bool eventFilter(QObject *, QEvent *event);

};


#endif // RESET_H
