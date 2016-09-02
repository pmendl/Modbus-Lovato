#include "LogFragment.h"

#include <QDebug>

LogFragment::LogFragment(
		QDateTime from, QDateTime to,
						 QString group, QObject *parent) :
	LogFragment(QString(), from, to, group, parent)
{}

LogFragment::LogFragment(
		QString id,
		QString group, QObject *parent) :
	LogFragment(id, QDateTime(), QDateTime(), group, parent)
{}

LogFragment::LogFragment(QString id,
						 QDateTime from, QDateTime to,
						 QString group, QObject *parent) :
	QBuffer(parent),
	startIndex(0),
	endIndex(-1),
	lastFragment(false),
	_from(from),
	_to(to),
	_group(group),
	_id(id)

{

}

QBuffer *LogFragment::pullBuffer() {
	if(lastFragment)
		return 0;

	QBuffer *result = new QBuffer(&buffer());
	startIndex=endIndex;
	buffer().clear();
	return result;
}

QDateTime LogFragment::from() const
{
	return _from;
}

QDateTime LogFragment::to() const
{
	return _to;
}

QString LogFragment::group() const
{
	return _group;
}

QString LogFragment::id() const
{
	return _id;
}

