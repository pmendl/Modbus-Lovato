#include <QCoreApplication>

#include <iostream>

#include "Test.h"

tcPlayApp::tcPlayApp ( int & argc, char ** argv)
: QCoreApplication (argc, argv)
{
	setApplicationName    ("Play App");
	setOrganizationName   ("Acme");
	setObjectName ("tcPlayApp");
}

ConsoleInput::ConsoleInput (QObject *parent) :
	QObject(parent)
{
	m_thread = new tcConsoleThrd (this);
	m_thread->run();
}

ConsoleInput::~ConsoleInput()
{
	delete m_thread;
}

tcConsoleThrd::tcConsoleThrd (QObject *parent) :
	QThread (parent),
	ts(stdin)
{
}

void tcConsoleThrd::run ()
{
	std::cout << "Running...\n";
	std::cout.flush();

	while (1)
	{
		std::cout << "Reading...\n> ";
		std::cout.flush();

		QString temp = ts.device()->readLine().trimmed();

		std::cout << "Read: " << qPrintable(temp) << "\n";
		std::cout.flush();

		if (temp == "quit")
		{
			QEvent *ce = new QEvent (QEvent::Quit);
			QCoreApplication::postEvent (QCoreApplication::instance(), ce);
			return;
		}

		std::cout << "> ";
		std::cout.flush();

		printf ("%s\n", qPrintable (temp));
	}
}
