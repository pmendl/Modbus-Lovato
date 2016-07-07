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
			a.quit();
		}
	});

	ks.start();
	std::cout << "Modbus started...\n";
	int result = a.exec();
//	ks.quit();
	std::cout << "Modbus quited...\n";
	return result;
}
