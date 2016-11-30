#ifndef SIGNALISATIONCONTROLLER_H
#define SIGNALISATIONCONTROLLER_H

#include <QObject>
#include <QSharedPointer>

class SignalisationController : public QObject
{
	Q_OBJECT
public:
	explicit SignalisationController(QObject *parent = 0);
	static void setHttpStatus(bool status);

signals:

public slots:

private:
	static QSharedPointer<class GpioPin> _httpGpio;
};

#endif // SIGNALISATIONCONTROLLER_H
