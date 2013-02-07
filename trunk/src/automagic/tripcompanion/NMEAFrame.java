package automagic.tripcompanion;

public class NMEAFrame {

	int		_TimeHour;
	int		_TimeMinutes;
	int		_TimeSeconds;
	
	float	_LatitudeValue;			// Latitude  => 4851.8273
	char	_LatitudeSign;			// 'N' or 'S'

	float	_LongitudeValue;		// Longitude => 00226.9146
	char	_LongitudeSign;			// 'E' or 'W'

	float 	_Altitude;				// altitude
	
	NMEAFrame()	{	}
	
	void FeedWithFrameGPGGA(String sFrame)
	{
		String sArgs[];
		sArgs =sFrame.split(",");
		
		//
		// $GPGGA - Global Positioning System Fix Data. Time, position and fix related data for a GPS receiver. (14 Args)
		//
		// ex: 	$GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
		//
		
		// 1    = UTC 					"hhmmss.ss"
		_TimeHour 		=Integer.parseInt(sArgs[1].substring(0, 2));
		_TimeMinutes 	=Integer.parseInt(sArgs[1].substring(2, 4));
		_TimeSeconds 	=(int)Float.parseFloat(sArgs[1].substring(4, 10));
		
		_LatitudeValue 	=Float.parseFloat(sArgs[2]);
		// 3    = LatitudeSign			 "N" or "S"
		_LatitudeSign	=sArgs[3].charAt(0);

		// 4    = Longitude 			"yyyyy.yy"
		_LongitudeValue =Float.parseFloat(sArgs[4]);
		// 5    = LongitudeSign 		"E" or "W"
		_LongitudeSign	=sArgs[5].charAt(0);

		// 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
		// 7    = Number of satellites in use [not those in view]
		// 8    = Horizontal dilution of position
		// 9    = Antenna altitude above/below mean sea level (geoid)
		// 10   = Meters  (Antenna height unit)
		// 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
		// 12   = Meters  (Units of geoidal separation)
		float AltitudeGeoid =Float.parseFloat(sArgs[9]);
		float AltitudeDiff	=Float.parseFloat(sArgs[11]);
		_Altitude =AltitudeGeoid-AltitudeDiff;
		
		// 13   = Age in seconds since last update from diff. reference station
		// 14   = Diff. reference station ID# '*' Checksum
	}
	
	/*
	void FeedWithFrameGPRMC(String sFrame)
	{
		String sArgs[];
		sArgs =sFrame.split(",");

		//
		// $GPRMC - Recommended minimum specific GPS/Transit data (12 Args)
		//
		// ex: 	$GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
		
		// 1    = UTC 					"hhmmss.ss"
		_TimeHour 		=Integer.parseInt(sArgs[1].substring(0, 2));
		_TimeMinutes 	=Integer.parseInt(sArgs[1].substring(2, 4));
		_TimeSeconds 	=(int)Float.parseFloat(sArgs[1].substring(4, 10));

		// 2    = Data status (V=navigation receiver warning)
		// todo ...
		
		// 3    = Latitude 				"llll.ll"
		_LatitudeValue =Float.parseFloat(sArgs[3]);
		// 4    = LatitudeSign 			"N" or "S"
		_LatitudeSign	=sArgs[4].charAt(0);
		

		// 5    = Longitude				"yyyyy.y"
		_LongitudeValue =Float.parseFloat(sArgs[5]);
		// 6    = LongitudeSign 		"E" or "W"
		_LongitudeSign	=sArgs[6].charAt(0);

		// 7    = Speed over ground in knots
		// 8    = Track made good in degrees True
		// 9    = UT date
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
	*/
		
}
