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

public slots:
	void onFinished();
	void onTimer();

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
