#ifndef POSTPARSINGPROCESSOR_H
#define POSTPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"

#include <QSharedPointer>
#include <QBasicTimer>
#include <QSettings>
#include <QUrl>


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
	void timerEvent(QTimerEvent *);

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
	quint16 _timeout;
};

#endif // POSTPARSINGPROCESSOR_H
