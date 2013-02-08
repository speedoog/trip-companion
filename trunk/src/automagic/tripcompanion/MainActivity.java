package automagic.tripcompanion;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Vector;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.ProgressBar;
import android.widget.Toast;

public class MainActivity extends Activity {
	ExpandableListView _Listview;
	ExpandableListAdapter _ListviewAdapter;
	Button _ButtonGo;
	ProgressBar _ProgressBar;
	

	public Bitmap getResizedBitmap(Bitmap bm, int newHeight, int newWidth)
	{
		int width = bm.getWidth();
		int height = bm.getHeight();
		float scaleWidth = ((float) newWidth) / width;
		float scaleHeight = ((float) newHeight) / height;
		
		// create a matrix for the manipulation
		Matrix matrix = new Matrix();
		
		// resize the bit map
		matrix.postScale(scaleWidth, scaleHeight);
		
		// recreate the new Bitmap
		Bitmap resizedBitmap = Bitmap.createBitmap(bm, 0, 0, width, height, matrix, false);

		return resizedBitmap;
	}

	void ResizeJpg(String sIn, String sOut)
	{
		Bitmap myBitmapIn = BitmapFactory.decodeFile(sIn);
		
		Bitmap myBitmapOut=getResizedBitmap(myBitmapIn, 100, 100); 

		FileOutputStream fileOutputStream = null;
		try
		{
			fileOutputStream = new FileOutputStream(sOut);
		} catch (FileNotFoundException e) { e.printStackTrace(); }
		
		int quality = 50; 
		BufferedOutputStream bos = new BufferedOutputStream(fileOutputStream);

		myBitmapOut.compress(CompressFormat.JPEG, quality, bos);
	}
	
	void TestFileList()
	{
		// test file list
		File fileRoot =new File("/mnt/sdcard/download/GPS");
		File[] fileList =fileRoot.listFiles();
		for(int i=0; i < fileList.length; i++)
	    {
	       File file = fileList[i];
	       ToastMe(file.getName());
	    }
	
		int t=0;
		++t;
	}
	
	void TestJpeg()
	{
		ResizeJpg("/mnt/sdcard/download/JPG/flip.jpg", "/mnt/sdcard/download/JPG/flip_resize.jpg"); 
		ResizeJpg("/mnt/sdcard/download/JPG/ok.jpg", "/mnt/sdcard/download/JPG/ok_resize.jpg"); 
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		_ButtonGo = (Button) findViewById(R.id.GoButton);
		_ProgressBar = (ProgressBar) findViewById(R.id.progressBar1);

		// go button
		_ButtonGo.setOnClickListener( new View.OnClickListener() { @Override public void onClick(View view) { ButtonGo(); }} );

		TestJpeg();
	}

	public void ButtonGo()
	{
		ToastMe("Go !");

		_ButtonGo.setClickable(false);
		_ButtonGo.setEnabled(false);
		
		// String sFilename = "/mnt/sdcard/download/test.log";
		//String sFilename = "/mnt/sdcard/download/GPS/GPS_20121209_122833.log"; // small log montreuil (22ko)
		String sFilename ="/mnt/sdcard/download/GPS/GPS_20121225_133848.log"; // St just - Pin (1.8Mo)
		// String sFilename ="/mnt/sdcard/download/GPS/GPS_20121226_135637.log"; // balade a Pin (1Mo)

		asyncTaskUpdateProgress async =new asyncTaskUpdateProgress();
		async.execute(sFilename);
	}

	public void ToastMe(String sText) 						{ Toast.makeText(this, sText, Toast.LENGTH_SHORT).show();	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	public class asyncTaskUpdateProgress extends AsyncTask<String, Integer, Void>
	{
		Vector<NMEAFrame>	_GpsFrames;

		@Override protected void onPostExecute(Void result) 			{ _ButtonGo.setClickable(true); _ButtonGo.setEnabled(true); ToastMe("Done !"); 	}
		@Override protected void onPreExecute() 						{ /* progress = 0;*/ 								}
		@Override protected void onProgressUpdate(Integer... values) 	{ _ProgressBar.setProgress(values[0]);  			}

		@Override
		protected Void doInBackground(String... arg0)
		{
//			ToastMe("Go !");

			String sFilename =arg0[0];

			_GpsFrames =new Vector<NMEAFrame>();

	        try
	        {
				File myFile = new File(sFilename);
				long	_nFileSize = myFile.length();
				long	_nFilePos  = 0;
				
				FileInputStream fIn = new FileInputStream(myFile);
				BufferedReader myReader = new BufferedReader( new InputStreamReader(fIn) );

				String aDataRow = "";
				while ((aDataRow = myReader.readLine()) != null)
				{
					if (aDataRow.length()>=4 && aDataRow.charAt(3) =='G')
					{
						NMEAFrame GpsFrame =new NMEAFrame();
						GpsFrame.FeedWithFrameGPGGA(aDataRow);
						_GpsFrames.add(GpsFrame);
					}
					
					_nFilePos +=aDataRow.length() + 2;		// +2 = CR+LF
					double ratio =((double)_nFilePos) / ((double)_nFileSize);
					publishProgress((int)(100.0f*ratio));
				}
				
				myReader.close();
				
			}
	        catch (Exception e)
	        {
			}

			return null;
		}
	}

}

//********************************************************
/*
ExifInterface exif;

double latitude = 48.863696;
double longitude = 2.44695166;

try {
    exif = new ExifInterface("/sdcard/DCIM/Camera/IMG_20121212_163224.jpg");
    
    String sDate =exif.getAttribute(ExifInterface.TAG_DATETIME);
    
    int num1Lat = (int)Math.floor(latitude);
    int num2Lat = (int)Math.floor((latitude - num1Lat) * 60);
    double num3Lat = (latitude - ((double)num1Lat+((double)num2Lat/60))) * 3600000;

    int num1Lon = (int)Math.floor(longitude);
    int num2Lon = (int)Math.floor((longitude - num1Lon) * 60);
    double num3Lon = (longitude - ((double)num1Lon+((double)num2Lon/60))) * 3600000;

    exif.setAttribute(ExifInterface.TAG_GPS_LATITUDE, num1Lat+"/1,"+num2Lat+"/1,"+num3Lat+"/1000");
    exif.setAttribute(ExifInterface.TAG_GPS_LONGITUDE, num1Lon+"/1,"+num2Lon+"/1,"+num3Lon+"/1000");


    if (latitude > 0) {
        exif.setAttribute(ExifInterface.TAG_GPS_LATITUDE_REF, "N"); 
    } else {
        exif.setAttribute(ExifInterface.TAG_GPS_LATITUDE_REF, "S");
    }

    if (longitude > 0) {
        exif.setAttribute(ExifInterface.TAG_GPS_LONGITUDE_REF, "E");    
    } else {
    exif.setAttribute(ExifInterface.TAG_GPS_LONGITUDE_REF, "W");
    }

    exif.saveAttributes();

} catch (IOException e) {
    Log.e("PictureActivity", e.getLocalizedMessage());
}
*/
//****************************** 
