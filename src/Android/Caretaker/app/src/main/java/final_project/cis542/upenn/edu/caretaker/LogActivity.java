package final_project.cis542.upenn.edu.caretaker;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;

import android.content.Intent;
import android.widget.ArrayAdapter;
import android.widget.*;
import android.view.View;
import android.widget.AdapterView.OnItemClickListener;

import java.util.Locale;

public class LogActivity extends Activity {

    LogActivity context = this;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log);

        setTitle("View Logs");

        // we get a click for one of the logs, get the coords and show in Google maps

        OnItemClickListener listener = new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String text = (String) (((ListAdapter) parent.getAdapter()).getItem(position));
                float lat = Float.parseFloat((text.split("LOC:")[1].split(","))[0]);
                float lon = Float.parseFloat((text.split("LOC:")[1].split(","))[1]);
                String uri = String.format(Locale.ENGLISH, "geo:%f,%f?z=%d&q=%f,%f (%s)",
                        lat, lon, 1, lat, lon, "Loc");
                Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
                context.startActivity(intent);
            }

        };

        // Get the logs dump
        Intent intent = getIntent();
        String log = intent.getStringExtra(HomeActivity.LOGS);

        // split into individual logs

        String[] logs = log.split("#");
        for (int i = 0; i < logs.length; i++) {

            // tag  them by time, type and location

            if (logs[i] != null && !logs[i].equals("")) {
                String time = logs[i].split(":")[0] + ":" + logs[i].split(":")[1] + ":" + logs[i].split(":")[2];
                String type = logs[i].split(":")[3].split("at")[0];
                String loc = logs[i].split(":")[3].split("at")[1];
                logs[i] = "TIME: " + time + "\nTYPE: " + type + "\nLOC: " + loc;
            }
        }
        ArrayAdapter<String> myArrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, logs);
        ListView myListView = (ListView) findViewById(R.id.my_list);
        myListView.setAdapter(myArrayAdapter);
        myListView.setOnItemClickListener(listener);
        myListView.setItemsCanFocus(true);

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_log, menu);
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
}
