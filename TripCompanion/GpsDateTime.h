#ifndef GPSTIME_H
#define GPSTIME_H

#include <QtGlobal>
#include <QString>

class GpsDateTime
{
public:
			GpsDateTime();

	inline qint64	GetPacked() const
	{
		qint64	val;
		val =_nYear;
		val *=100;	val +=_nMonth;
		val *=100;	val +=_nDay;
		val *=100;	val +=_nHour;
		val *=100;	val +=_nMinutes;
		val *=100;	val +=_nSeconds;

		return val;
	}

	inline void ToGpxDate(QString& sOut)
	{
		sOut.sprintf("%04d-%02d-%02dT%02d:%02d:%02dZ", _nYear, _nMonth, _nDay, _nHour, _nMinutes, _nSeconds);
	}

	inline void FromExifDate(const QString& sExifDate)
	{
		// Format : "2013:08:26 09:31:58"
		_nYear		=sExifDate.mid(0,4).toInt();
		_nMonth		=sExifDate.mid(5,2).toInt();
		_nDay		=sExifDate.mid(8,2).toInt();
		_nHour		=sExifDate.mid(11,2).toInt();
		_nMinutes	=sExifDate.mid(14,2).toInt();
		_nSeconds	=sExifDate.mid(17,2).toInt();
	}

	inline void ToExifDate(QString& sExifDateOut)
	{
		// Format : "2013:08:26 09:31:58"
		sExifDateOut.reserve(20);
		sExifDateOut.sprintf("%04d:%02d:%02d %02d:%02d:%02d", _nYear, _nMonth, _nDay, _nHour, _nMinutes, _nSeconds);
	}

	void	FixOffsetTime(int nHour);

	int		_nHour;
	int		_nMinutes;
	int		_nSeconds;

	int		_nDay;
	int		_nMonth;
	int		_nYear;		// 4 digits format => 2013
};

#endif // GPSTIME_H
