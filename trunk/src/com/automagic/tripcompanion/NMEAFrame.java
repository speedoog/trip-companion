package com.automagic.tripcompanion;

public class NMEAFrame {

	// ex: 	$GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	// 1    = UTC of Position
	// 2    = Latitude
	// 3    = N or S
	// 4    = Longitude
	// 5    = E or W
	// 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
	// 7    = Number of satellites in use [not those in view]
	// 8    = Horizontal dilution of position
	
	// 9    = Antenna altitude above/below mean sea level (geoid)
	// 10   = Meters  (Antenna height unit)
	
	// 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
	// 12   = Meters  (Units of geoidal separation)
	
	// 13   = Age in seconds since last update from diff. reference station
	// 14   = Diff. reference station ID# '*' Checksum

	boolean _MinimalData;			// true if RMC frame / false if GPGGA

	int		_TimeHour;
	int		_TimeMinutes;
	int		_TimeSeconds;
	
	double	_LatitudeValue;			// Latitude  => 4851.8273
	char	_LatitudeSign;			// 'N' or 'S'
	
	double	_LongitudeValue;		// Longitude => 00226.9146
	char	_LongitudeSign;			// 'E' or 'W'
	
	char	_GPSQuality;			// GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
	
	char	_SatelliteCount;		// Number of satellites in use [not those in view] (ex: 03)
	
	double 	_HorizontalDilution;	// Horizontal dilution of position (ex: 4.2)

	double 	_AltitudeAntennaValue;	// Antenna altitude above/below mean sea level (geoid) -47.3
	char	_AltitudeAntennaUnit;	// unit meters = 'M'
	
	double 	_AltitudeGeoidalValue;	// Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
	char	_AltitudeGeoidalUnit;	// Meters  (Units of geoidal separation) = 'M'
	
	NMEAFrame()
	{
	}
	
	void FeedWithFrame(String sFrame)
	{
		int t=0;
		
		
		String sArgs[];
		sArgs =sFrame.split(",");
		
		String sHeader =sArgs[0];
		
		if (sHeader.compareTo("$ADVER") == 0)
		{
			// skip
			++t;
		}
		else if (sHeader.compareToIgnoreCase("$GPRMC") == 0)
		{
			//
			// $GPRMC - Recommended minimum specific GPS/Transit data (12 Args)
			//
			// ex: 	$GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
			
			_MinimalData =true;
			
			// 1    = UTC 					"hhmmss.ss"
			_TimeHour 	 =Integer.parseInt(sArgs[1].substring(0, 2));
			_TimeMinutes =Integer.parseInt(sArgs[1].substring(2, 4));
			_TimeSeconds =(int)Double.parseDouble(sArgs[1].substring(4, 10));

			// 2    = Data status (V=navigation receiver warning)
			// todo ...
			
			// 3    = Latitude 				"llll.ll"
			_LatitudeValue =Double.parseDouble(sArgs[3]);
			// 4    = LatitudeSign 			"N" or "S"
			_LatitudeSign	=sArgs[4].charAt(0);
			

			// 5    = Longitude				"yyyyy.y"
			_LongitudeValue =Double.parseDouble(sArgs[5]);
			// 6    = LongitudeSign 		"E" or "W"
			_LongitudeSign	=sArgs[6].charAt(0);

			// 7    = Speed over ground in knots
			// 8    = Track made good in degrees True
			// 9    = UT date
			// 10   = Magnetic variation degrees (Easterly var. subtracts from true course)
			// 11   = E or W
			// 12   = Checksum

			/*
			_GPSQuality
			_SatelliteCount
			_HorizontalDilution
			_AltitudeAntennaValue
			_AltitudeAntennaUnit
			_AltitudeGeoidalValue
			_AltitudeGeoidalUnit
			*/
		}
		else if (sHeader.compareToIgnoreCase("$GPGGA") == 0)
		{
			//
			// $GPGGA - Global Positioning System Fix Data. Time, position and fix related data for a GPS receiver. (14 Args)
			//
			// ex: 	$GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
			//
			
			_MinimalData =false;

			// 1    = UTC 					"hhmmss.ss"
			_TimeHour 	 =Integer.parseInt(sArgs[1].substring(0, 2));
			_TimeMinutes =Integer.parseInt(sArgs[1].substring(2, 4));
			_TimeSeconds =(int)Double.parseDouble(sArgs[1].substring(4, 10));
			
			// 2    = Latitude 				"llll.ll"
			_LatitudeValue =Double.parseDouble(sArgs[2]);
			// 3    = LatitudeSign			 "N" or "S"
			_LatitudeSign	=sArgs[3].charAt(0);

			// 4    = Longitude 			"yyyyy.yy"
			_LongitudeValue =Double.parseDouble(sArgs[4]);
			// 5    = LongitudeSign 		"E" or "W"
			_LongitudeSign	=sArgs[5].charAt(0);

			// 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
			// 7    = Number of satellites in use [not those in view]
			// 8    = Horizontal dilution of position
			// 9    = Antenna altitude above/below mean sea level (geoid)
			// 10   = Meters  (Antenna height unit)
			// 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
			// 12   = Meters  (Units of geoidal separation)
			// 13   = Age in seconds since last update from diff. reference station
			// 14   = Diff. reference station ID# '*' Checksum

			++t;
		}
		
		if (t==0)
		{
			++t;
		}
	
	}
}