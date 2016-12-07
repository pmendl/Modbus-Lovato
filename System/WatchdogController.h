#ifndef WATCHDOGCONTROLLER_H
#define WATCHDOGCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

class WatchdogController : public QObject
{
	Q_OBJECT

public:
	explicit WatchdogController(QObject *parent = 0);

signals:

private slots:
	void onTimer();

private:
	QSharedPointer<class GpioPin> _watchdogGpio, _powerdownGpio;
	bool _currentState;
	QTimer _timer;
};

#endif // WATCHDOGCONTROLLER_H
