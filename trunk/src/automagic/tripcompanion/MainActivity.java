package automagic.tripcompanion;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.List;
import java.util.Vector;

import org.apache.sanselan.ImageReadException;
import org.apache.sanselan.ImageWriteException;
import org.apache.sanselan.Sanselan;
import org.apache.sanselan.common.IImageMetadata;
import org.apache.sanselan.formats.jpeg.JpegImageMetadata;
import org.apache.sanselan.formats.jpeg.exifRewrite.ExifRewriter;
import org.apache.sanselan.formats.tiff.TiffImageMetadata;
import org.apache.sanselan.formats.tiff.constants.TagInfo;
import org.apache.sanselan.formats.tiff.write.TiffOutputDirectory;
import org.apache.sanselan.formats.tiff.write.TiffOutputField;
import org.apache.sanselan.formats.tiff.write.TiffOutputSet;

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
	
	public static GPSLog _GPSLog;

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
	/*
	private static void copyExifData(String sIn, String sOut , List<TagInfo> excludedFields)
	{
		File sourceFile	=new File(sIn);
		File destFile	=new File(sOut);
		
	    String tempFileName = destFile.getAbsolutePath() + ".tmp";
	    File tempFile = null;
	    OutputStream tempStream = null;
	 
	    try
	    {
	        tempFile = new File (tempFileName);
	 
	        TiffOutputSet sourceSet = getSanselanOutputSet(sourceFile);
	        TiffOutputSet destSet = getSanselanOutputSet(destFile);
	 
	        destSet.getOrCreateExifDirectory();
	 
	        // Go through the source directories
	        List<?> sourceDirectories = sourceSet.getDirectories();
	        for (int i=0; i<sourceDirectories.size(); i++)
	        {
	            TiffOutputDirectory sourceDirectory = (TiffOutputDirectory)sourceDirectories.get(i);
	            TiffOutputDirectory destinationDirectory = getOrCreateExifDirectory(destSet, sourceDirectory);
	 
	            if (destinationDirectory == null) continue; // failed to create
	 
	            // Loop the fields
	            List<?> sourceFields = sourceDirectory.getFields();
	            for (int j=0; j<sourceFields.size(); j++)
	            {
	                // Get the source field
	                TiffOutputField sourceField = (TiffOutputField) sourceFields.get(j);
	 
	                // Check exclusion list
//	                if (excludedFields.contains(sourceField.tagInfo))
//	                {
//	                    destinationDirectory.removeField(sourceField.tagInfo);
//	                    continue;
//	                }
	 
	                // Remove any existing field
	                destinationDirectory.removeField(sourceField.tagInfo);
	 
	                // Add field
	                destinationDirectory.add(sourceField);
	            }
	        }
	 
	        // Save data to destination
	        tempStream = new BufferedOutputStream(new FileOutputStream(tempFile));
	        new ExifRewriter().updateExifMetadataLossless(destFile, tempStream, destSet);
	        tempStream.close();
	 
	        // Replace file
	        if (destFile.delete())
	        {
	            tempFile.renameTo(destFile);
	        }
	    }
	    catch (ImageReadException exception)
	    {
	        exception.printStackTrace();
	    }
	    catch (ImageWriteException exception)
	    {
	        exception.printStackTrace();
	    }
	    catch (IOException exception)
	    {
	        exception.printStackTrace();
	    }
	    finally
	    {
	        if (tempStream != null)
	        {
	            try
	            {
	                tempStream.close();
	            }
	            catch (IOException e)
	            {
	            }
	        }
	 
	        if (tempFile != null)
	        {
	            if (tempFile.exists()) tempFile.delete();
	        }
	    }
	}
	 
	private static TiffOutputSet getSanselanOutputSet(File jpegImageFile)
	        throws IOException, ImageReadException, ImageWriteException
	{
	    TiffOutputSet outputSet = null;
	 
	    // note that metadata might be null if no metadata is found.
	    IImageMetadata metadata = Sanselan.getMetadata(jpegImageFile);
	    JpegImageMetadata jpegMetadata = (JpegImageMetadata) metadata;
	    if (jpegMetadata != null)
	    {
	        // note that exif might be null if no Exif metadata is found.
	        TiffImageMetadata exif = jpegMetadata.getExif();
	 
	        if (exif != null)
	        {
	            outputSet = exif.getOutputSet();
	        }
	    }
	 
	    // if file does not contain any exif metadata, we create an empty
	    // set of exif metadata. Otherwise, we keep all of the other
	    // existing tags.
	    if (outputSet == null)
	        outputSet = new TiffOutputSet();
	 
	    // Return
	    return outputSet;
	}
	 
	private static TiffOutputDirectory getOrCreateExifDirectory(TiffOutputSet outputSet, TiffOutputDirectory outputDirectory)
	{
	    TiffOutputDirectory result = outputSet.findDirectory(outputDirectory.type);
	    if (result != null)
	        return result;
	    result = new TiffOutputDirectory(outputDirectory.type);
	    try
	    {
	        outputSet.addDirectory(result);
	    }
	    catch (ImageWriteException e)
	    {
	        return null;
	    }
	    return result;
	}
	*/
	
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

	public static String ROOT ="/storage/sdcard0/download"; 

	public void ButtonGo()
	{
		try
		{
			
			ToastMe("Go !");
			
	//		ResizeJpg("/mnt/sdcard/download/JPG/flip.jpg", "/mnt/sdcard/download/JPG/flip_resize.jpg", 50, 100, 100); 
	//		ResizeJpg("/mnt/sdcard/download/JPG/ok.jpg", "/mnt/sdcard/download/JPG/ok_resize.jpg", 50, 100, 100);
			
			// path : /mnt/sdcard/download/VietFull
			
			
	//		String sIn 	="/mnt/sdcard/download/JPG/big.jpg";
	//		String sOut ="/mnt/sdcard/download/JPG/big_resize.jpg";
	//		ResizeJpg2(sIn, sOut, 80, 1920);
	//		CopyExif(sIn, sOut);
			
			
			_ButtonGo.setClickable(false);
			_ButtonGo.setEnabled(false);
			
			// String sFilename = "/mnt/sdcard/download/test.log";
			//String sFilename = "/mnt/sdcard/download/GPS/GPS_20121209_122833.log"; // small log montreuil (22ko)
			//String sFilename =ROOT+"/GPS/GPS_20121225_133848.log"; // St just - Pin (1.8Mo)
			//String sFilename =ROOT+"/GPS/GPS_20121226_135637.log"; // balade a Pin (1Mo)
			
			//String sFilename =ROOT+"/GPS/picnic.log";
			//String sFilename =ROOT+"/GPS/SouthAfrica.log";
			String sFilename =ROOT+"/GPS/SouthAmerica.log";
	
			_GPSLog =new GPSLog();
			_GPSLog.Init(sFilename);
			
			asyncTaskUpdateProgress async =new asyncTaskUpdateProgress();
			async.execute(sFilename);
	
			ToastMe("End read");
			
		
//			ToastMe("End write");
		}
		catch (Exception e)
        {
		}
	
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
		@Override protected void onPostExecute(Void result) 			{ _ButtonGo.setClickable(true); _ButtonGo.setEnabled(true); ToastMe("Done !"); 	}
		@Override protected void onPreExecute() 						{ /* progress = 0;*/ 								}
		@Override protected void onProgressUpdate(Integer... values) 	{ _ProgressBar.setProgress(values[0]);  			}

		@Override
		protected Void doInBackground(String... arg0)
		{
			String sFilename =arg0[0];
	        try
	        {
				// -----------------------------

				File myFile = new File(sFilename);
				long	_nFileSize = myFile.length();
				long	_nFilePos  = 0;
				
				FileInputStream fIn = new FileInputStream(myFile);
				BufferedReader myReader = new BufferedReader( new InputStreamReader(fIn) );

				String aDataRow = "";
				while ((aDataRow = myReader.readLine()) != null)
				{
					MainActivity._GPSLog.FeedWithNmeaFrame(aDataRow);

					_nFilePos +=aDataRow.length() + 2;		// +2 = CR+LF
					double ratio =((double)_nFilePos) / ((double)_nFileSize);
					publishProgress((int)(100.0f*ratio));
				}
				
				myReader.close();
				
				// -----------------------------
				//String sGPXOut =MainActivity.ROOT+"/GPS/picnic";
				//String sGPXOut =MainActivity.ROOT+"/GPS/SouthAfrica";
				String sGPXOut =MainActivity.ROOT+"/GPS/SouthAmerica";
				MainActivity._GPSLog.WriteGPX(sGPXOut+".gpx");
				MainActivity._GPSLog.WriteKML(sGPXOut+".kml");
				// -----------------------------
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
