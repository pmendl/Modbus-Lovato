#ifndef POSTPARSINGPROCESSOR_H
#define POSTPARSINGPROCESSOR_H

#include "Processing/ParsingProcessor.h"

#include <QBasicTimer>
#include <QUrl>

//#include "Network/DebugHttpMultiPart.h"
#include "Globals.h"
#include HTTP_MULTI_PART_INCLUDE
#include "Network/NetworkSender.h"



class PostParsingProcessor : public ParsingProcessor
{
	Q_OBJECT

public:
	PostParsingProcessor(class QSettings *settings, QString group,
						 quint64 timeout = NETWORK_DEFAULT_TIMEOUT);
	virtual ~PostParsingProcessor() {}
	virtual bool isValid() const;
	virtual void process(class RequestManager *rm);

protected slots:
	void onMultipartSent(QHttpMultiPart *multiPart, QNetworkReply *reply);
	void onReplyFinished();


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
	requestPriority_t _priority;
	quint16 _delayedCount;
	NetworkSender _sender;
	HTTP_MULTI_PART_USED *_multipart;
	QBasicTimer _timer;
	quint16 _timeout;
};

#endif // POSTPARSINGPROCESSOR_H
