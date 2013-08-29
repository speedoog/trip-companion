#include "GpsFrame.h"

#include <QStringList>
#include <math.h>

static QString sGGAFrame ="$GPGGA";
static QString sRMCFrame ="$GPRMC";

GpsFrame::GpsFrame()
{
	_dLatitudeValue		=0.0;
	_dLongitudeValue	=0.0;
	_nLatitudeSign		=0;
	_nLongitudeSign		=0;

	_dAltitude			=0.0;
}

GpsFrame::GpsFrameType GpsFrame::GetFrameType(const QStringList& slSplitFrame)
{
	const QString&	sHeader =slSplitFrame[0];
	if (sHeader == sGGAFrame)
	{
		return GF_GGA;
	}
	else if (sHeader == sRMCFrame)
	{
		return GF_RMC;
	}
	else
	{
		return GF_OTHER;
	}
}

int GpsFrame::GetPackedTime(const QStringList& slSplitFrame)
{
	const QString& sTime =slSplitFrame[1];
	const int nTime =sTime.mid(0,6).toInt();
	return nTime;
}

void GpsFrame::FeedWithFrameGPGGA(const QStringList& slSplitFrame)
{
	//
	// $GPGGA - Global Positioning System Fix Data. Time, position and fix related data for a GPS receiver. (14 Args)
	//
	// ex: 	$GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	//

	// 1    = UTC 					"hhmmss.ss"
	const QString& sArg1 =slSplitFrame[1];
	_Time._nHour 		=sArg1.mid(0,2).toInt();
	_Time._nMinutes 	=sArg1.mid(2,2).toInt();
	_Time._nSeconds 	=sArg1.mid(4,2).toInt();

	// 2	= Latitude				"llll.ll"
	double dLatitude =slSplitFrame[2].toDouble()/100.0;
	// 3    = LatitudeSign			 "N" or "S"
	_nLatitudeSign	=slSplitFrame[3][0].toLatin1();

	// 4    = Longitude 			"yyyyy.yy"
	double dLongitude =slSplitFrame[4].toDouble()/100.0;
	// 5    = LongitudeSign 		"E" or "W"
	_nLongitudeSign	=slSplitFrame[5][0].toLatin1();

	// 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
	// 7    = Number of satellites in use [not those in view]
	// 8    = Horizontal dilution of position
	// 9    = Antenna altitude above/below mean sea level (geoid)
	// 10   = Meters  (Antenna height unit)
	// 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
	// 12   = Meters  (Units of geoidal separation)
	double dAltitudeGeoid	=slSplitFrame[9].toDouble();
//	double dAltitudeDiff	=slSplitFrame[11].toDouble();
//	_dAltitude =dAltitudeGeoid-dAltitudeDiff;
	_dAltitude =dAltitudeGeoid;

	// 13   = Age in seconds since last update from diff. reference station
	// 14   = Diff. reference station ID# '*' Checksum

	double dLatDeg  = floor(dLatitude);
	double dLatPart = (dLatitude - dLatDeg) * 100.0 / 60.0;

	_dLatitudeValue = dLatDeg+dLatPart;
	if (_nLatitudeSign!='N') _dLatitudeValue =-_dLatitudeValue;

	double dLonDeg  = floor(dLongitude);
	double dLonPart = (dLongitude - dLonDeg) * 100.0 / 60.0;
	_dLongitudeValue= dLonDeg+dLonPart;
	if (_nLongitudeSign!='E') _dLongitudeValue =-_dLongitudeValue;
}

void GpsFrame::FeedWithFrameGPRMC(const QStringList& slSplitFrame)
{
	//
	// $GPRMC - Recommended minimum specific GPS/Transit data (12 Args)
	//
	// ex: 	$GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

	// 1    = UTC 					"hhmmss.ss"
	const QString& sArg1 =slSplitFrame[1];
	_Time._nHour 		=sArg1.mid(0,2).toInt();
	_Time._nMinutes 	=sArg1.mid(2,2).toInt();
	_Time._nSeconds 	=sArg1.mid(4,2).toInt();

	// 2    = Data status (V=navigation receiver warning)
	// todo ...

	// 3	= Latitude				"llll.ll"
	double dLatitude =slSplitFrame[3].toDouble()/100.0;
	// 4    = LatitudeSign			 "N" or "S"
	_nLatitudeSign	=slSplitFrame[4][0].toLatin1();

	// 5    = Longitude 			"yyyyy.yy"
	double dLongitude =slSplitFrame[5].toDouble()/100.0;
	// 6    = LongitudeSign 		"E" or "W"
	_nLongitudeSign	=slSplitFrame[6][0].toLatin1();

	// 7    = Speed over ground in knots
	// 8    = Track made good in degrees True
	// 9    = UT date
	const QString& sArg9 =slSplitFrame[9];
	_Time._nDay 	=sArg9.mid(0,2).toInt();
	_Time._nMonth	=sArg9.mid(2,2).toInt();
	_Time._nYear 	=sArg9.mid(4,2).toInt()+2000;

	// 10   = Magnetic variation degrees (Easterly var. subtracts from true course)
	// 11   = E or W
	// 12   = Checksum

	//_GPSQuality
	//_SatelliteCount
	//_HorizontalDilution
	//_AltitudeAntennaValue
	//_AltitudeAntennaUnit
	//_AltitudeGeoidalValue
	//_AltitudeGeoidalUnit
}
