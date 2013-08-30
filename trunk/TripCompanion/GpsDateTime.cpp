#include "GpsDateTime.h"

#include <QDateTime>
#include <QDate>
#include <QTime>

GpsDateTime::GpsDateTime()
: _nHour	(-1)
, _nMinutes	(-1)
, _nSeconds	(-1)
, _nDay		(-1)
, _nMonth	(-1)
, _nYear	(-1)
{
}

void GpsDateTime::FixOffsetTime(int nHour)
{
	QDateTime qdate(QDate(_nYear, _nMonth, _nDay), QTime(_nHour, _nMinutes, _nSeconds));

	QDateTime qdateOffset =qdate.addSecs(60*60*nHour);

	_nYear		=qdateOffset.date().year();
	_nMonth		=qdateOffset.date().month();
	_nDay		=qdateOffset.date().day();

	_nHour		=qdateOffset.time().hour();
	_nMinutes	=qdateOffset.time().minute();
	_nSeconds	=qdateOffset.time().second();
}
