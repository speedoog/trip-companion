#ifndef GPSFRAME_H
#define GPSFRAME_H

#include <QString>
#include <QStringList>
#include "GpsDateTime.h"

class GpsFrame
{
public:
	enum GpsFrameType
	{
		GF_OTHER	=0,
		GF_GGA		=1,
		GF_RMC		=2
	};

							GpsFrame();
	static	GpsFrameType	GetFrameType(const QStringList& slSplitFrame);
	static	int				GetPackedTime(const QStringList& slSplitFrame);

			void			FeedWithFrameGPGGA(const QStringList& slSplitFrame);
			void			FeedWithFrameGPRMC(const QStringList& slSplitFrame);

	inline  void ToGpxDate(QString& sOut)	{ _Time.ToGpxDate(sOut); }

public:

	GpsDateTime	_Time;

	double		_dLatitudeValue;		// Latitude  => 4851.8273
	double		_dLongitudeValue;		// Longitude => 00226.9146

	char		_nLatitudeSign;			// 'N' or 'S'
	char		_nLongitudeSign;		// 'E' or 'W'

	double		_dAltitude;             // altitude

};

#endif // GPSFRAME_H
