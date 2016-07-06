#ifndef NETWORKACCESSBASE_H
#define NETWORKACCESSBASE_H

#include <QtNetwork/QNetworkAccessManager>

#include <memory>


/// Pure virtual base implementing global QNetworkAccessManager.
/// @note Based on modern C++ publications singleton was avoided
/// by using static QNetworkAccessManager object.
class NetworkAccessBase
{
public:
	/**
	 * @brief This class SHOULD NOT be directly instantiated
	 */
	virtual ~NetworkAccessBase() = 0;

	static QNetworkAccessManager *networkAccessManager(void);

private:
	static QNetworkAccessManager* _networkAccessManager;
};

#endif // NETWORKACCESSBASE_H
