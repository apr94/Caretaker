package final_project.cis542.upenn.edu.caretaker;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;


public class SettingsActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        setTitle("Caretaker Settings");

        // used to load the preferences for poll time and log size from memory

        SharedPreferences scores = getSharedPreferences("data", 0);
        //((EditText) findViewById(R.id.LogData)).setText(scores.getString("logize", "5"));

        //((EditText) findViewById(R.id.PollData)).setText(scores.getString("polltime", "500"));

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_settings, menu);
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

    public void onQuitButtonClick(View view) {

        // Commit the new preferences to memory
        SharedPreferences scores = getSharedPreferences("data",0);
        SharedPreferences.Editor editor = scores.edit();

        EditText text = (EditText) findViewById(R.id.PollData);
        String pt = text.getText().toString();

        text = (EditText) findViewById(R.id.LogData);
        String ls = text.getText().toString();

        if(pt == null || pt.equals("")){
            pt = "1000";
        }
        if(ls == null || ls.equals("")){
            pt = "10";
        }

        // polltime = time between each poll for alerts
        // logsize = number of logs displayed at once

        editor.putString("polltime", pt);
        editor.putString("logsize", ls);

        editor.apply();


        finish();
    }
}
