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
//	QSettings settings;
//	QUrl _resetNotifyUrl;
	class NetworkSender *_sender;

	//(NetworkSender::parseUrl(settings.value(QStringLiteral(RESET_GROUP_KEY "/" RESET_NOTIFICATION_URL_KEY)).toString().toLower()));



};


#endif // RESET_H
