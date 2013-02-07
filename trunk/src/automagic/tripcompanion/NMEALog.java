package automagic.tripcompanion;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.app.Activity;
import android.widget.Toast;

public class NMEALog extends Thread
{
	public 		String				 	_sFilename;
	public 		long					_nFileSize, _nFilePos;
	public 		Activity				_Activity;
	protected 	ArrayList<NMEAFrame> 	_frames;

	public void run()
	{
		LoadFile(_sFilename);
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
