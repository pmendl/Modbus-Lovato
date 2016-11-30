#ifndef RESET_H
#define RESET_H

#include <QObject>
#include <QEvent>
#include <QSettings>
#include <QUrl>
#include <QSharedPointer>


namespace System {

void resetInitiate(QString reason);
void resetEnforce(void);
bool startResetSensitiveProcess(int priority);
void endResetSensitiveProcess(int priority);

extern const int initiateResetEventType;
extern const int executeResetEventType;

} // System

class InitiateResetEvent : public QEvent
{
public:
	InitiateResetEvent(QString reason);

	const QString _reason;
};

class InitiateResetEventFilter : public QObject
{
	Q_OBJECT

public:
	InitiateResetEventFilter();

protected:
	virtual bool eventFilter(QObject *, QEvent *event);

private:
	QString _logName;
	class NetworkSender *_sender;
};


#endif // RESET_H
