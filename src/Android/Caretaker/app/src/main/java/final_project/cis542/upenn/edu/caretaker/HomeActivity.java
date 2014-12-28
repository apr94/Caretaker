package final_project.cis542.upenn.edu.caretaker;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.app.AlertDialog;
import android.content.DialogInterface;
import java.util.Locale;

import android.net.Uri;
import android.os.CountDownTimer;
import android.widget.Toast;


public class HomeActivity extends Activity {

    // global variables

    HomeActivity context = this;
    public final static String LOGS = "logs";
    public final static String BATTERY = "battery";
    CountDownTimer cdt;
    int alert_poll_time = 5000;
    int elapsed_time = 0;
    String responseString = null;
    float lat = 0;
    float lon = 0;

  // the alert_poll thread
    @Override
    protected void onCreate(Bundle savedInstanceState){

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);

        setTitle("Caretaker");

        cdt = new CountDownTimer(1000, 10) {

            public void onTick(long millisUntilFinished) {


                    elapsed_time += 10;

                    // check if time has elapsed up to granularity, then only make the GET request

                    if(elapsed_time >= alert_poll_time){
                        elapsed_time = 0;

                        // Get request to fetch the data for alerts

                        String url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/has_alert/";

                        try {
                            responseString = new GetMachine().execute(url).get();
                        }
                        catch(Exception E){}

                        // a real alert, fetch the coords

                        if (responseString != null && !responseString.equals("0")) {
                            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(HomeActivity.this);
                            alertDialogBuilder.setMessage("ALERT Received from Location: " + responseString + ". Click Map to View.");
                            lat = Float.parseFloat((responseString.split(","))[0]);
                            lon = Float.parseFloat((responseString.split(","))[1]);

                            // open in Google maps

                            alertDialogBuilder.setPositiveButton("Map",
                                    new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface arg0, int arg1) {

                                            String uri = String.format(Locale.ENGLISH, "geo:%f,%f?z=%d&q=%f,%f (%s)",
                                                    lat, lon, 1, lat, lon, "Loc");
                                            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
                                            context.startActivity(intent);

                                        }
                                    });
                            alertDialogBuilder.setNegativeButton("OK",
                                    new DialogInterface.OnClickListener(){

                                        @Override
                                        public void onClick(DialogInterface d, int arg1) {
                                            d.cancel();
                                        }
                                    });

                            // alert dialog to show that an alert is received.

                            AlertDialog alertDialog = alertDialogBuilder.create();
                            alertDialog.show();
                        }
                    }
            }
            // Timer elapsed
            public void onFinish() {
                cdt.start();
            }
        }.start();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_home, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    // Open up a map with the last known location

    public void onMapButtonClick(View view) throws Exception {

        String url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/get_loc/";
        String responseString = new GetMachine().execute(url).get();
        if (responseString == null) {
            Toast.makeText(getApplicationContext(), "Location could not be retrieved!",
                    Toast.LENGTH_LONG).show();
            return;
        }
        float lat = Float.parseFloat((responseString.split(","))[0]);
        float lon = Float.parseFloat((responseString.split(","))[1]);
        String uri = String.format(Locale.ENGLISH, "geo:%f,%f?z=%d&q=%f,%f (%s)",
                lat, lon, 1, lat, lon, "Loc");
        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
        context.startActivity(intent);

    }

    // open up the logs context

    public void onLogsButtonClick(View view) throws Exception {

        // gets logs first by making GET request

        String url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/get_log/";
        String responseString = new GetMachine().execute(url).get();
        if (responseString == null) {

            // no internet connection or server is down
            Toast.makeText(getApplicationContext(), "Logs could not be retrieved!",
                    Toast.LENGTH_LONG).show();
            return;
        }

        // start logs screen
        Intent intent = new Intent(this, LogActivity.class);
        intent.putExtra(LOGS, responseString);
        startActivity(intent);

    }
    // open battery context

    public void onBatteryButtonClick(View view) throws Exception {

        // get request

        String url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/get_battery/";
        String responseString = new GetMachine().execute(url).get();
        if (responseString == null) {
            Toast.makeText(getApplicationContext(), "Battery information could not be retrieved!",
                    Toast.LENGTH_LONG).show();
            return;
            // no internet connection
        }

        Intent intent = new Intent(this, BatteryActivity.class);
        intent.putExtra(BATTERY, responseString);
        startActivity(intent);
    }

    // open settings context
    public void onSettingsButtonClick(View view) throws Exception {

        Intent intent = new Intent(this, SettingsActivity.class);
        startActivity(intent);
    }

    // quit application
    public void onQuitButtonClick(View view) {
        finish();
    }

}
