package automagic.tripcompanion;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.util.Vector;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.os.AsyncTask;
import android.os.Bundle;
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
	

	public Bitmap getResizedBitmap(Bitmap bm, int newWidth, int newHeight)
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
	
	static int Max(int a, int b)
	{
		return a>b?a:b;
	}

	/*
	private static boolean isPowerOfTwo(int n)
	{
		double logNbase2 =  Math.log(n)/Math.log(2);	
		int logNbase2Integer = (int) (Math.floor(logNbase2));
		
		if(logNbase2-logNbase2Integer==0)
			return true;
		else
			return false;
	}
	*/

	private static int NearestPowerOfTwo(int n)
	{
		double logNbase2 =  Math.log(n)/Math.log(2);
		int nVal =(int)Math.pow(2, logNbase2);
		return nVal;
	}

	private Bitmap decodeFile(File f, int nMaxSize)
	{
	    try {
	        // Decode image size
	        BitmapFactory.Options o = new BitmapFactory.Options();
	        o.inJustDecodeBounds = true;
	        BitmapFactory.decodeStream(new FileInputStream(f),null,o);

	        // Find the correct scale value. It should be the power of 2.
	        int nBigestSize =Max(o.outWidth, o.outHeight);
	        
	        float rScale =((float)nBigestSize)/((float)nMaxSize);
	        
	        int scale=NearestPowerOfTwo((int)rScale);
//	        while(o.outWidth/scale/2>=nMaxSize && o.outHeight/scale/2>=nMaxSize)
//	            scale*=2;
	        
	        float rRatio =((float)o.outWidth)/((float)o.outHeight);

	        // Decode with inSampleSize
	        BitmapFactory.Options o2 = new BitmapFactory.Options();
	        o2.inSampleSize =scale;

	        Bitmap BmpLoaded =BitmapFactory.decodeStream(new FileInputStream(f), null, o2);
	        
	        // loaded, now resize ...
	        int nNewSizeX =nMaxSize;
	        int nNewSizeY =(int)(((float)nMaxSize)/rRatio);
	        
	        //---------------------------- fail here ---------------------
	        Bitmap BmpOut =getResizedBitmap(BmpLoaded, nNewSizeX, nNewSizeY);
	        		
	        return BmpOut;

	    } catch (FileNotFoundException e) {}
	    return null;
	}

	void ResizeJpg2(String sIn, String sOut, int quality, int Width)
	{
		File fileIn =new File(sIn);
		Bitmap myBitmapOut =decodeFile(fileIn, Width);
		SaveJpg(sOut, myBitmapOut, quality);
	}

	void ResizeJpg(String sIn, String sOut, int quality, int Height, int Width)
	{
		Bitmap myBitmapIn = BitmapFactory.decodeFile(sIn);
		Bitmap myBitmapOut=getResizedBitmap(myBitmapIn, Height, Width); 
		SaveJpg(sOut, myBitmapOut, quality);
	}
	
	void SaveJpg(String sFilenameOut, Bitmap bitmap, int quality)
	{
		FileOutputStream fileOutputStream = null;
		try
		{
			fileOutputStream = new FileOutputStream(sFilenameOut);
		} catch (FileNotFoundException e) { e.printStackTrace(); }
		
		BufferedOutputStream bos = new BufferedOutputStream(fileOutputStream);

		bitmap.compress(CompressFormat.JPEG, quality, bos);
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
	
	void CopyExifTag(ExifInterface exifSrc, ExifInterface exifDst, String sTag)
	{
		String sTagValue =exifSrc.getAttribute(sTag);
		if (sTagValue!=null)
		{
			exifDst.setAttribute(sTag, sTagValue);
		}
	}
	
	void CopyExif(String sSrc, String sDst)
	{
		try {
		    // copy paste exif information from original file to new
		    // file
		    ExifInterface exifSrc = new ExifInterface(sSrc);
		    ExifInterface exifDst = new ExifInterface(sDst);

		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_APERTURE				);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_DATETIME				);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_EXPOSURE_TIME			);
//			TAG_FLASH					// int
//			TAG_FOCAL_LENGTH			// float
//			TAG_GPS_ALTITUDE			// float
//			TAG_GPS_ALTITUDE_REF		// int
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_DATESTAMP			);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_LATITUDE			);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_LATITUDE_REF		);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_LONGITUDE			);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_LONGITUDE_REF		);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_PROCESSING_METHOD	);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_GPS_TIMESTAMP			);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_ISO						);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_MAKE					);
		    CopyExifTag(exifSrc, exifDst, ExifInterface.TAG_MODEL					);
//			TAG_ORIENTATION				// int
//			TAG_WHITE_BALANCE 			// int

		    exifDst.saveAttributes();

		} catch (Exception e) {
		    e.printStackTrace();
		}
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

		//TestJpeg();
	}

	public void ButtonGo()
	{
		ToastMe("Go !");
		
//		ResizeJpg("/mnt/sdcard/download/JPG/flip.jpg", "/mnt/sdcard/download/JPG/flip_resize.jpg", 50, 100, 100); 
//		ResizeJpg("/mnt/sdcard/download/JPG/ok.jpg", "/mnt/sdcard/download/JPG/ok_resize.jpg", 50, 100, 100);
		
		// path : /mnt/sdcard/download/VietFull
		
		String sIn 	="/mnt/sdcard/download/JPG/big.jpg";
		String sOut ="/mnt/sdcard/download/JPG/big_resize.jpg";
		ResizeJpg2(sIn, sOut, 80, 1920);
		CopyExif(sIn, sOut);

		ToastMe("End");
		
		/*
		_ButtonGo.setClickable(false);
		_ButtonGo.setEnabled(false);
		
		// String sFilename = "/mnt/sdcard/download/test.log";
		//String sFilename = "/mnt/sdcard/download/GPS/GPS_20121209_122833.log"; // small log montreuil (22ko)
		String sFilename ="/mnt/sdcard/download/GPS/GPS_20121225_133848.log"; // St just - Pin (1.8Mo)
		// String sFilename ="/mnt/sdcard/download/GPS/GPS_20121226_135637.log"; // balade a Pin (1Mo)

		asyncTaskUpdateProgress async =new asyncTaskUpdateProgress();
		async.execute(sFilename);
		*/
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
