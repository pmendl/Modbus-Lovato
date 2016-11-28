#ifndef NETWORKACCESSBASE_H
#define NETWORKACCESSBASE_H

#include <QtNetwork/QNetworkAccessManager>
#include <QBasicTimer>

/// Pure virtual base implementing global QNetworkAccessManager.
/// @note Based on modern C++ publications singleton was avoided
/// by using static QNetworkAccessManager object.
class NetworkAccessBase : public QObject
{
public:
	NetworkAccessBase(QObject *parent = 0) : QObject(parent) {}

	/**
	 * @brief Initiate panic connections subsystem
	 *
	 * Has to be called once before start of using of any NetworkAccessBase descendant
	 */
	static void readPanicConnections();

	/**
	 * @brief This class SHOULD NOT be directly instantiated
	 */
	virtual ~NetworkAccessBase() = 0;

	static QNetworkAccessManager *networkAccessManager(void);
	static QUrl parseUrl(QString url);

protected:
	virtual void timerEvent(QTimerEvent *event);

private:
	static QNetworkAccessManager* _networkAccessManager;
	static QBasicTimer _panicTimer;
	static QList<QUrl> _panicUrlList;
};

#endif // NETWORKACCESSBASE_H
