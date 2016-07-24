#ifndef PARSINGPROCESSOR_H
#define PARSINGPROCESSOR_H

#include <QObject>
#include <QSharedPointer>
#include <QBasicTimer>
#include <QSettings>
#include <QUrl>

#include "Network/NetworkAccessBase.h"

/// @file

class ParsingProcessor : public QObject
{
	Q_OBJECT

public:
	virtual ~ParsingProcessor() {}
	virtual bool isValid() = 0;
	virtual void process(class RequestManager *rm) = 0;
};


class PostParsingProcessor : public ParsingProcessor, public NetworkAccessBase
{
	Q_OBJECT

public:
	PostParsingProcessor(QSettings *settings);
	virtual ~PostParsingProcessor() {}
	virtual bool isValid();
	virtual void process(class RequestManager *rm);
/*
signals:
	/ **
	 * @brief Send parsed items' values to the server.
	 * @param sender String unique to the given group of reported items. It is
	 * used to prevent flood of newly created connections if previous request
	 * gets delayed in transition (or server side) while allowing paralel transmittion of different
	 * items groups.
	 * @sa NetworkSender
	 * @param url Address of the destination server.
	 * @param http HTTP multipart object to be transmitted.
	 * /
	void sendRequest(QString sender, QUrl url, QSharedPointer<class QHttpMultiPart> http);
*/
protected:
	/**
	 * @brief Content type dependent priority of HTTP POST.
	 * @details Priority increases with more interresting (and thus important)
	 * content, i.e. the one containing more details about situation.<br>
	 * Priority decides, which request (older or the new one) keep if waiting for
	 * congested communication link.
	 */
	typedef enum {
		nullRequestPriority = 0,	///< No response from Modbus device <em>(lowest priority)</em>
		errorRequestPriority = 1,	///< Error response from Modbus device
		normalRequestPriority = 2	///< Response containing data from Modbus device <em>(highest priority)</em>
	}  requestPriority_t;

private:
	QUrl _url;
	QSharedPointer<QHttpMultiPart> _multiPart;
	QNetworkReply *_reply;
	requestPriority_t _priority;
	quint16 _delayedCount;
	QBasicTimer _timer;
	// ?type _timeout;
};

#endif // PARSINGPROCESSOR_H
