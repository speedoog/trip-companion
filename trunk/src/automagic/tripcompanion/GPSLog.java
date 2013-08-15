package automagic.tripcompanion;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
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
		if (sFrame.length()>=4 && sFrame.charAt(3) =='G')
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
	void WriteGPX(String sFilename) throws IOException
	{
		String sCR		="\n";
		String sHeader1 ="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		String sHeader2 ="<gpx";
		String sHeader3 ="	version=\"1.0\"";
		String sHeader4 ="	creator=\"Trip Companion\"";
		String sHeader5 ="	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
		String sHeader6 ="	xmlns=\"http://www.topografix.com/GPX/1/0\"";
		String sHeader7 ="	xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">";
		String sHeader8 ="	<time>2013-08-11T22:02:30Z</time>";
		String sHeader9 ="	<trk>";
		String sHeader10="		<trkseg>";
		// ..........
		String sFooter0 ="			</trkpt>\n";
		String sFooter1 ="		</trkseg>";
		String sFooter2 ="	</trk>";
		String sFooter3 ="</gpx>";

        try
        {
			FileOutputStream 	fos = new FileOutputStream(sFilename);
			OutputStreamWriter 	out = new OutputStreamWriter(fos, "UTF-8");
			
			out.write(sHeader1);	out.write(sCR);
			out.write(sHeader2);	out.write(sCR);
			out.write(sHeader3);	out.write(sCR);
			out.write(sHeader4);	out.write(sCR);
			out.write(sHeader5);	out.write(sCR);
			out.write(sHeader6);	out.write(sCR);
			out.write(sHeader7);	out.write(sCR);
			out.write(sHeader8);	out.write(sCR);
			out.write(sHeader9);	out.write(sCR);
			out.write(sHeader10);	out.write(sCR);
			
			for (GPSFrame gpsframe : _GpsFrames)
			{
		        String sFrame1 = String.format("			<trkpt lat=\"%f\" lon=\"%f\">\n", gpsframe._LatitudeValue, gpsframe._LongitudeValue);
				out.write(sFrame1);
				out.write(sFooter0);
			}

			out.write(sFooter1);	out.write(sCR);
			out.write(sFooter2);	out.write(sCR);
			out.write(sFooter3);	out.write(sCR);
			
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
