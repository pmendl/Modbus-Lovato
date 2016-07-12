#ifndef KEYBOARDSCANNER_H
#define KEYBOARDSCANNER_H

#include <QThread>

class KeyboardScanner : public QThread
{
	Q_OBJECT
signals:
	void KeyPressed(char ch);
public slots:
	void setDetection(bool detect);
	void finish();

public:
   KeyboardScanner();
   ~KeyboardScanner();
protected:
   void run();
private:
   bool _doRun, _doDetect;
};

#endif // KEYBOARDSCANNER_H
