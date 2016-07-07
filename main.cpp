#include <QCoreApplication>

#include <iostream>

#include "Network/NetworkResource.h"
#include "Console/KeyboardScanner.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	KeyboardScanner ks;
	QObject::connect(&ks, &KeyboardScanner::KeyPressed, [&ks, &a](char c){
		std::cout << c << "\n";
		if(c == 'q' || c == 'Q') {
			std::cout << "Quitting...\n";
			ks.finish();
		}
	});
	QObject::connect(&ks, &KeyboardScanner::finished, &a, &QCoreApplication::quit);

	ks.start();
	std::cout << "Modbus started...\n";
	int result = a.exec();
	std::cout << "Modbus quited...\n";
	return result;
}
