#ifndef POSTPARSINGPROCESSOR_H
#define POSTPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"

#include <QSharedPointer>
#include <QBasicTimer>
#include <QUrl>

#include "Globals.h"


class PostParsingProcessor : public ParsingProcessor, public NetworkAccessBase
{
	Q_OBJECT

public:
	PostParsingProcessor(class QSettings *settings, QString group,
						 quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	virtual ~PostParsingProcessor() {}
	virtual bool isValid() const;
	virtual void process(class RequestManager *rm);

public slots:
	void onFinished(QSharedPointer<QNetworkReply> reply);

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
	bool _inProcess;
	QHttpMultiPart* _multiPart;
	requestPriority_t _priority;
	quint16 _delayedCount;
	class NetworkSender * _sender;
	QBasicTimer _timer;
	quint16 _timeout;
};

#endif // POSTPARSINGPROCESSOR_H
