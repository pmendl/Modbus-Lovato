#ifndef KEYBOARDSCANNER_H
#define KEYBOARDSCANNER_H

#include <QTimer>

class KeyboardScanner : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		silentMode = 0,
		emitChar = 0x01,
		emitLine = 0x02,
		echoMode = 0x04
	} mode_t;

signals:
	void KeyPressed(char ch);
	void LineCollected(QString line);

public slots:
	void setMode(mode_t mode);

public:
   KeyboardScanner(mode_t mode  = emitChar);
   ~KeyboardScanner();
/*
   char waitKey(bool echo);
   QString waitLine(bool echoMode = true);
*/
   mode_t mode() const;
   void startTimer();


protected:
   void timerEvent(QTimerEvent *);

private:
   mode_t _mode, _oldmode;
   volatile int _key;
   QString _line;
   volatile bool _completeLine;
   QBasicTimer _timer;
};

#endif // KEYBOARDSCANNER_H
