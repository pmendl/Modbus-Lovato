#ifndef KEYBOARDSCANNER_H
#define KEYBOARDSCANNER_H

#include <QThread>

class KeyboardScanner : public QThread
{
	Q_OBJECT
signals:
	void KeyPressed(char ch);
public slots:
	void finish();
public:
   KeyboardScanner();
   ~KeyboardScanner();
protected:
   void run();
private:
   bool doRun;
};

#endif // KEYBOARDSCANNER_H
