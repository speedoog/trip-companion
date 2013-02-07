package automagic.tripcompanion;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Vector;

import android.app.Activity;
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

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		_ButtonGo = (Button) findViewById(R.id.GoButton);
		_ProgressBar = (ProgressBar) findViewById(R.id.progressBar1);

		// go button
		_ButtonGo.setOnClickListener( new View.OnClickListener() { @Override public void onClick(View view) { ButtonGo(); }} );
		
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
