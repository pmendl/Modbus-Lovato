#ifndef TEST_H
#define TEST_H

#include <QThread>
#include <QTextStream>
#include <QCoreApplication>

class tcConsoleThrd : public QThread
{
public:
	tcConsoleThrd  (QObject *parent);
	void     run   ();
	QTextStream ts;
};

class ConsoleInput : public QObject
{
	Q_OBJECT
public:
	ConsoleInput(QObject *parent);
	~ConsoleInput();
	tcConsoleThrd  *m_thread;
};

class tcPlayApp : public QCoreApplication
{
public:
	tcPlayApp ( int & argc, char ** argv);
};



#endif // TEST_H
