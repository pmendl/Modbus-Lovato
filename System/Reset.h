#ifndef RESET_H
#define RESET_H

#include <QObject>
#include <QEvent>


namespace System {

void resetInitiate(void);
void resetEnforce(void);
bool startResetSensitiveProcess(int priority);
void endResetSensitiveProcess(int priority);

extern const int initiateResetEventType;
extern const int executeResetEventType;

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
