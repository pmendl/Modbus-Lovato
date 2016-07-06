#include <QCoreApplication>

#include <iostream>

#include "Network/NetworkResource.h"
#include "Test.h"

int main(int argc, char *argv[])
{
//	QCoreApplication a(argc, argv);
	tcPlayApp a(argc, argv);

	std::cout << "Initialized...\n";
	std::cout.flush();


	ConsoleInput cn (&a);

	std::cout << "Started...\n";
	std::cout.flush();

	return a.exec();
}
