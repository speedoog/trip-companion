package automagic.tripcompanion;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.Locale;
import java.util.Vector;

public class GPSLog
{
	public 	String				_sFilename;
	public 	Vector<GPSFrame>	_GpsFrames;

	void Init(String sFilename)
	{
		_sFilename =sFilename;
		_GpsFrames =new Vector<GPSFrame>();
	}
	
	void FeedWithNmeaFrame(String sFrame)
	{
		String sGPGGAFrame ="$GPGGA";
		int    sGPGGAFrameLen =sGPGGAFrame.length();
		if ( sFrame.length()>sGPGGAFrameLen && sGPGGAFrame.equals(sFrame.substring(0, sGPGGAFrameLen)) )
		{
			GPSFrame GpsFrame =new GPSFrame();
			GpsFrame.FeedWithFrameGPGGA(sFrame);
			_GpsFrames.add(GpsFrame);
		}
	}
/*
 *  ------------------ GPX FILE ------------------

<?xml version="1.0" encoding="UTF-8"?>
<gpx
	version="1.0"
	creator="GPSBabel - http://www.gpsbabel.org"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns="http://www.topografix.com/GPX/1/0"
	xsi:schemaLocation="http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd">
	<time>2013-08-11T22:02:30Z</time>
	<bounds minlat="48.841241667" minlon="2.416945000" maxlat="48.853498333" maxlon="2.439183333"/>
	<trk>
		<trkseg>
			<trkpt lat="48.845960000" lon="2.424243333">
	  			<ele>57.600000</ele>
	  			<time>2013-08-11T19:53:55Z</time>
	  			<course>77.099998</course>
	  			<speed>3.009500</speed>
	  			<fix>3d</fix>
	  			<sat>5</sat>
	  			<hdop>3.000000</hdop>
			</trkpt>
		</trkseg>
	</trk>
</gpx>

 *  ------------------ GPX FILE ------------------
	
 */
	void WriteGPX(String sFilename)
	{
		String sCR		="\n";
		String sGPXHeader1 ="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		String sGPXHeader2 ="<gpx";
		String sGPXHeader3 ="	version=\"1.0\"";
		String sGPXHeader4 ="	creator=\"Trip Companion\"";
		String sGPXHeader5 ="	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
		String sGPXHeader6 ="	xmlns=\"http://www.topografix.com/GPX/1/0\"";
		String sGPXHeader7 ="	xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">";
		String sGPXHeader8 ="	<time>2013-08-11T22:02:30Z</time>";
		String sGPXHeader9 ="	<trk>";
		String sGPXHeader10="		<trkseg>";
		// ..........
		String sGPXFooter0 ="			</trkpt>\n";
		String sGPXFooter1 ="		</trkseg>";
		String sGPXFooter2 ="	</trk>";
		String sGPXFooter3 ="</gpx>";

        try
        {
			FileOutputStream 	fos = new FileOutputStream(sFilename);
			OutputStreamWriter 	out = new OutputStreamWriter(fos, "UTF-8");
			
			out.write(sGPXHeader1);		out.write(sCR);
			out.write(sGPXHeader2);		out.write(sCR);
			out.write(sGPXHeader3);		out.write(sCR);
			out.write(sGPXHeader4);		out.write(sCR);
			out.write(sGPXHeader5);		out.write(sCR);
			out.write(sGPXHeader6);		out.write(sCR);
			out.write(sGPXHeader9);		out.write(sCR);
			out.write(sGPXHeader7);		out.write(sCR);
			out.write(sGPXHeader8);		out.write(sCR);
			out.write(sGPXHeader10);	out.write(sCR);
			
			for (GPSFrame gpsframe : _GpsFrames)
			{
		        String sFrame1 = String.format(Locale.US, "			<trkpt lat=\"%f\" lon=\"%f\">\n", gpsframe._LatitudeValue, gpsframe._LongitudeValue);
				out.write(sFrame1);
				out.write(sGPXFooter0);
			}

			out.write(sGPXFooter1);		out.write(sCR);
			out.write(sGPXFooter2);		out.write(sCR);
			out.write(sGPXFooter3);		out.write(sCR);
			
			out.close();
		}
        catch (Exception e)
        {
		}
	}

	/*
	 *  ------------------ KML FILE ------------------

<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://earth.google.com/kml/2.2">
<Document xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	<name>test__picnic.gpx</name>
	<Style id="lineStyle">
		<LineStyle>
			<color>99ffac59</color>
			<width>6</width>
		</LineStyle>
	</Style>
	<Placemark>
		<name>picnic</name>
		<styleUrl>#lineStyle</styleUrl>
		<LineString>
			<coordinates>
				2.439183333,48.853498333,  
				2.439003333,48.853016667,48
				2.424243333,48.845960000,58
			</coordinates>
		</LineString>
	</Placemark>
</Document>
</kml>
 
	 *  ------------------ KML FILE ------------------
	 */

	void WriteKML(String sFilename)
	{
		String sCR		="\n";
		
		String sKMLHeader01 ="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		String sKMLHeader02 ="<kml xmlns=\"http://earth.google.com/kml/2.2\">";
		String sKMLHeader03 ="<Document xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";
		String sKMLHeader04 ="	<name>test__picnic.gpx</name>";
		String sKMLHeader05 ="	<Style id=\"lineStyle\">";
		String sKMLHeader06 ="		<LineStyle>";
		String sKMLHeader07 ="			<color>99ffac59</color>";
		String sKMLHeader08 ="			<width>6</width>";
		String sKMLHeader09 ="		</LineStyle>";
		String sKMLHeader10 ="	</Style>";
		String sKMLHeader11 ="	<Placemark>";
		String sKMLHeader12 ="		<name>picnic</name>";
		String sKMLHeader13 ="		<description>Test de description</description>";
		String sKMLHeader14 ="		<styleUrl>#lineStyle</styleUrl>";
		String sKMLHeader15 ="		<LineString>";
		String sKMLHeader16 ="			<coordinates>";
		// ..........
		String sKMLFooter01 ="			</coordinates>";
		String sKMLFooter02 ="		</LineString>";
		String sKMLFooter03 ="	</Placemark>";
		String sKMLFooter04 ="</Document>";
		String sKMLFooter05 ="</kml>";
		
        try
        {
			FileOutputStream 	fos = new FileOutputStream(sFilename);
			OutputStreamWriter 	out = new OutputStreamWriter(fos, "UTF-8");
			
			out.write(sKMLHeader01);	out.write(sCR);
			out.write(sKMLHeader02);	out.write(sCR);
			out.write(sKMLHeader03);	out.write(sCR);
			out.write(sKMLHeader04);	out.write(sCR);
			out.write(sKMLHeader05);	out.write(sCR);
			out.write(sKMLHeader06);	out.write(sCR);
			out.write(sKMLHeader07);	out.write(sCR);
			out.write(sKMLHeader08);	out.write(sCR);
			out.write(sKMLHeader09);	out.write(sCR);
			out.write(sKMLHeader10);	out.write(sCR);
			out.write(sKMLHeader11);	out.write(sCR);
			out.write(sKMLHeader12);	out.write(sCR);
			out.write(sKMLHeader13);	out.write(sCR);
			out.write(sKMLHeader14);	out.write(sCR);
			out.write(sKMLHeader15);	out.write(sCR);
			out.write(sKMLHeader16);	out.write(sCR);
			
			for (GPSFrame gpsframe : _GpsFrames)
			{
				String sFrame;
				if (gpsframe._Altitude==-1)
				{
					sFrame = String.format(Locale.US, "%f,%f, \n", gpsframe._LongitudeValue, gpsframe._LatitudeValue);					
				}
				else
				{
					sFrame = String.format(Locale.US, "%f,%f,%f\n", gpsframe._LongitudeValue, gpsframe._LatitudeValue, gpsframe._Altitude);					
				}

				out.write(sFrame);
			}

			out.write(sKMLFooter01);		out.write(sCR);
			out.write(sKMLFooter02);		out.write(sCR);
			out.write(sKMLFooter03);		out.write(sCR);
			out.write(sKMLFooter04);		out.write(sCR);
			out.write(sKMLFooter05);		out.write(sCR);
			
			out.close();
		}
        catch (Exception e)
        {
		}
	}

	
	
	
	void LoadFile(String sFilename)
	{
		/*
//    	Toast.makeText(_Activity, "Start Loading "+sFilename, Toast.LENGTH_SHORT).show(); 

		_frames =new ArrayList<NMEAFrame>();
		
        try
        {
			File myFile = new File(sFilename);
			_nFileSize = myFile.length();
			_nFilePos  = 0;
			
			FileInputStream fIn = new FileInputStream(myFile);
			BufferedReader myReader = new BufferedReader( new InputStreamReader(fIn) );

			String aDataRow = "";
			while ((aDataRow = myReader.readLine()) != null)
			{
				_nFilePos +=aDataRow.length() + 2;		// +2 = CR+LF
				NMEAFrame GpsFrame =new NMEAFrame();
				GpsFrame.FeedWithFrameGPGGA(aDataRow);
				_frames.add(GpsFrame);
			}
			myReader.close();
			
			((MainActivity) _Activity).ToastMe("Succes !");
	    	//Toast.makeText(_Activity, "Succes !", Toast.LENGTH_SHORT).show(); 

			return true;
		}
        catch (Exception e)
        {
			return false;
		}
		*/
	}

}
