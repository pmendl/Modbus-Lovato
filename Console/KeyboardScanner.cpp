#include "KeyboardScanner.h"

#include <QDebug>
#include <QCoreApplication>

#include <iostream>
//#include <stdio.h>
//#include <unistd.h>
#include <termios.h>

static struct termios oldSettings;
static struct termios newSettings;

/* Set new echo mode */
void setTermios(bool echo) {
	newSettings = oldSettings; /* make new settings same as old settings */
	newSettings.c_lflag &= ~ICANON; /* disable buffered i/o */
	newSettings.c_cc[VMIN] = newSettings.c_cc[VTIME] = 0; /* set no waiting for more keypresses */
	newSettings.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &newSettings); /* use these new terminal i/o settings now */
}

/* Initialize new terminal i/o settings */
void initTermios(bool echo) {
  tcgetattr(0, &oldSettings); /* grab old terminal i/o settings */
  oldSettings.c_lflag |= ECHO;
  setTermios(echo);
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &oldSettings);
}

KeyboardScanner::KeyboardScanner(mode_t mode) :
	_mode(mode)
{
  initTermios(mode & echoMode);
}

void KeyboardScanner::startTimer() {
	_timer.start(50, this);
}

KeyboardScanner::~KeyboardScanner()
{
  resetTermios();
}

void KeyboardScanner::setMode(mode_t mode) {
	_mode = mode;
	setTermios(_mode & echoMode);

}

/*
char KeyboardScanner::waitKey(bool echo) {
	_oldmode = _mode;
	_mode = echo ? echoMode : silentMode;
	_key =0;
	while(_key <=0) {
		QCoreApplication::processEvents();
		qDebug() << "***>" << _key;
	}
	setMode(_oldmode);
	return _key;
}

QString KeyboardScanner::waitLine(bool echo) {
	_oldmode = _mode;
	_mode = echo ?  echoMode : silentMode;
	_completeLine = false;
	_line.clear();
	while(!_completeLine) {
		QCoreApplication::processEvents();
	}
	return _line;
}
*/

void KeyboardScanner::timerEvent(QTimerEvent *) {

	char c(getchar());
	if(c != static_cast<char>(EOF)) { // 255 = EOF casted to char
//		qDebug() << "*** KEY:" << c;
		_key = c;
		if(_mode & emitChar)
			emit KeyPressed(c);
/*
		if((c != '\n') && c != '\r')
			_line.append(QChar(c));
		else {
			_completeLine = true;
			if (_mode & emitLine)
				emit LineCollected(_line);
		}
*/
	}
}

KeyboardScanner::mode_t KeyboardScanner::mode() const
{
	return _mode;
}

