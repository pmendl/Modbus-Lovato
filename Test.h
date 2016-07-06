#if false
#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <QThread>

class ConsoleReader : public QThread
{
	Q_OBJECT
signals:
	void KeyPressed(char ch);
public:
   ConsoleReader();
   ~ConsoleReader();
   void run();
};

#endif // TEST_H
#endif
