package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.widget.TextView;

public class CoinOpHiScoresActivity extends Activity
{   public native int    gethiscore(int whichscore);
    public native String gethiscoredate(int whichscore);
    public native void   sethiscore(int whichscore, int thescore, String thedate);
    
    private TextView   hs_1, hs_2, hs_3,
                       hs_4, hs_5, hs_6,
                       hsd_1, hsd_2, hsd_3,
                       hsd_4, hsd_5, hsd_6;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_coinophiscores);

        if (MainActivity.lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

                 hs_1           = findViewById(R.id.hs_1);
                 hs_2           = findViewById(R.id.hs_2);
                 hs_3           = findViewById(R.id.hs_3);
                 hs_4           = findViewById(R.id.hs_4);
                 hs_5           = findViewById(R.id.hs_5);
                 hs_6           = findViewById(R.id.hs_6);
                 hsd_1          = findViewById(R.id.hsd_1);
                 hsd_2          = findViewById(R.id.hsd_2);
                 hsd_3          = findViewById(R.id.hsd_3);
                 hsd_4          = findViewById(R.id.hsd_4);
                 hsd_5          = findViewById(R.id.hsd_5);
                 hsd_6          = findViewById(R.id.hsd_6);
        TextView button_clearhs = findViewById(R.id.button_clearhs);

        for (int i = 0; i < 51 + 6; i++)
        {   MainActivity.hiscore[i] = gethiscore(i);
            MainActivity.hiscoredate[i] = gethiscoredate(i);
        }
        doscores();
        
        button_clearhs.setOnClickListener((view) ->
        {   for (int i = 51; i < 51 + 6; i++)
            {   MainActivity.hiscore[i] = 0;
                MainActivity.hiscoredate[i] = "-";
                sethiscore(i, MainActivity.hiscore[i], MainActivity.hiscoredate[i]);
            }
            doscores();
        });
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {   super.onConfigurationChanged(newConfig);
        this.recreate();
    }
    
    private void doscores()
    {    hs_1.setText(String.valueOf(MainActivity.hiscore[51 + 0]));
         hs_2.setText(String.valueOf(MainActivity.hiscore[51 + 1]));
         hs_3.setText(String.valueOf(MainActivity.hiscore[51 + 2]));
         hs_4.setText(String.valueOf(MainActivity.hiscore[51 + 3]));
         hs_5.setText(String.valueOf(MainActivity.hiscore[51 + 4]));
         hs_6.setText(String.valueOf(MainActivity.hiscore[51 + 5]));
         
         hsd_1.setText(MainActivity.hiscoredate[51 + 0]);
         hsd_2.setText(MainActivity.hiscoredate[51 + 1]);
         hsd_3.setText(MainActivity.hiscoredate[51 + 2]);
         hsd_4.setText(MainActivity.hiscoredate[51 + 3]);
         hsd_5.setText(MainActivity.hiscoredate[51 + 4]);
         hsd_6.setText(MainActivity.hiscoredate[51 + 5]);
}   }
