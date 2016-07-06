#ifndef KEYBOARDSCANNER_H
#define KEYBOARDSCANNER_H

#include <QThread>

class KeyboardScanner : public QThread
{
	Q_OBJECT
signals:
	void KeyPressed(char ch);
public:
   KeyboardScanner();
   ~KeyboardScanner();
   void run();
};

#endif // KEYBOARDSCANNER_H
