package final_project.cis542.upenn.edu.caretaker;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.content.SharedPreferences;
import android.widget.TextView;

public class BatteryActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_battery);

        setTitle("Pebble System Information");

        Intent intent = getIntent();
        String bat = intent.getStringExtra(HomeActivity.BATTERY);

        // battery, CPU and memory data is passed via intent.
        // just split and fill in the blanks ;)

        String[] bats = bat.split(",");
        TextView text = (TextView) findViewById(R.id.CPUData);
        text.setText(bats[0]);
        text = (TextView) findViewById(R.id.MemoryData);
        text.setText(bats[1] + "%");
        text = (TextView) findViewById(R.id.BatteryData);
        text.setText(bats[2] + "%");


    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_battery, menu);
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

        finish();
    }
}
