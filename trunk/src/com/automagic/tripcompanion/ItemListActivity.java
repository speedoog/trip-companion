package com.automagic.tripcompanion;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.widget.Toast;


/**
 * An activity representing a list of Items. This activity
 * has different presentations for handset and tablet-size devices. On
 * handsets, the activity presents a list of items, which when touched,
 * lead to a {@link ItemDetailActivity} representing
 * item details. On tablets, the activity presents the list of items and
 * item details side-by-side using two vertical panes.
 * <p>
 * The activity makes heavy use of fragments. The list of items is a
 * {@link ItemListFragment} and the item details
 * (if present) is a {@link ItemDetailFragment}.
 * <p>
 * This activity also implements the required
 * {@link ItemListFragment.Callbacks} interface
 * to listen for item selections.
 */
public class ItemListActivity extends FragmentActivity
        implements ItemListFragment.Callbacks {

    /**
     * Whether or not the activity is in two-pane mode, i.e. running on a tablet
     * device.
     */
    private boolean mTwoPane;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_item_list);

        if (findViewById(R.id.item_detail_container) != null)
        {
            // The detail container view will be present only in the
            // large-screen layouts (res/values-large and
            // res/values-sw600dp). If this view is present, then the
            // activity should be in two-pane mode.
            mTwoPane = true;

            // In two-pane mode, list items should be given the
            // 'activated' state when touched.
            ((ItemListFragment) getSupportFragmentManager()
                    .findFragmentById(R.id.item_list))
                    .setActivateOnItemClick(true);
        }

        // TODO: If exposing deep links into your app, handle intents here.

    }

    /**
     * Callback method from {@link ItemListFragment.Callbacks}
     * indicating that the item with the given ID was selected.
     */
    @Override
    public void onItemSelected(String id) {
        if (mTwoPane) {
            // In two-pane mode, show the detail view in this activity by
            // adding or replacing the detail fragment using a
            // fragment transaction.
            Bundle arguments = new Bundle();
            arguments.putString(ItemDetailFragment.ARG_ITEM_ID, id);
            ItemDetailFragment fragment = new ItemDetailFragment();
            fragment.setArguments(arguments);
            getSupportFragmentManager().beginTransaction()
                    .replace(R.id.item_detail_container, fragment)
                    .commit();

        } else {
            // In single-pane mode, simply start the detail activity
            // for the selected item ID.
            Intent detailIntent = new Intent(this, ItemDetailActivity.class);
            detailIntent.putExtra(ItemDetailFragment.ARG_ITEM_ID, id);
            startActivity(detailIntent);
            
            //********************************************************
            
            //
            // from : http://www.java-samples.com/showtutorial.php?tutorialid=1523
            //
            
            String sFilename = "/mnt/sdcard/download/test.log";

            // 
            // GPS - NMEA sentence information
            // format : http://aprs.gids.nl/nmea/#rmc
            //
            
            try
            {
    			File myFile = new File(sFilename);
    			FileInputStream fIn = new FileInputStream(myFile);
    			BufferedReader myReader = new BufferedReader( new InputStreamReader(fIn) );
    			String aDataRow = "";
    			String aBuffer = "";
				int t=0;

    			while ((aDataRow = myReader.readLine()) != null)
    			{
    				String sArgs[];
    				sArgs =aDataRow.split(",");
    				
    				String sHeader =sArgs[0];
    				
    				if (sHeader.compareTo("$ADVER") == 0)
    				{
	    				++t;
    				}
    				else if (sHeader.compareToIgnoreCase("$GPRMC") == 0)
    				{
    					//
    					// $GPRMC - Recommended minimum specific GPS/Transit data (12 Args)
    					//
    					// eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
    					// 1    = UTC of position fix
    					// 2    = Data status (V=navigation receiver warning)
    					// 3    = Latitude of fix
    					// 4    = N or S
    					// 5    = Longitude of fix
    					// 6    = E or W
    					// 7    = Speed over ground in knots
    					// 8    = Track made good in degrees True
    					// 9    = UT date
    					// 10   = Magnetic variation degrees (Easterly var. subtracts from true course)
    					// 11   = E or W
    					// 12   = Checksum
    					
	    				++t;
    				}
    				else if (sHeader.compareToIgnoreCase("$GPGGA") == 0)
    				{
    					// 14 Args
	    				++t;
    				}
    			}
    			//detailIntent.setText(aBuffer);
    			myReader.close();
    			Toast.makeText(getBaseContext(), "Done reading SD 'mysdfile.txt'", Toast.LENGTH_SHORT).show();
    		} catch (Exception e) {
    			Toast.makeText(getBaseContext(), e.getMessage(), Toast.LENGTH_SHORT).show();
    		}
            
            /*
            File file = getFileStreamPath("/mnt/sdcard/download/test.log");
            if (file.exists())
            {
                Toast.makeText(this, "File exists in /mnt", Toast.LENGTH_SHORT).show();
            }
            */
            
//            String filename ="/mnt/sdcard/download/test.log";
//            FileInputStream fis(filename);
            
            /*
            InputStream inputStream = null;
			try {
				inputStream = openFileInput("/mnt/sdcard/download/test.log");
			} catch (FileNotFoundException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			*/
			
            /*
            InputStreamReader 	inputStreamReader 	= new InputStreamReader(fis);
            BufferedReader 		reader 				= new BufferedReader(inputStreamReader);
            try
            {
                String line;
                while ((line = reader.readLine()) != null)
                {
                     String[] RowData = line.split(",");
                     String date = RowData[0];
                     String value = RowData[1];
                    // do something with "data" and "value"
                }
            }
            catch (IOException ex)
            {
                // handle exception
            }
            finally
            {
                try
                {
                	inputStream.close();
                }
                catch (IOException e) {
                    // handle exception
                }
            }
            */
            
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
        }
    }
}
