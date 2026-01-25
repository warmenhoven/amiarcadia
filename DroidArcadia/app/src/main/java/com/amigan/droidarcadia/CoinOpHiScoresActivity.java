package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.widget.TextView;

public class CoinOpHiScoresActivity extends Activity
{   public native int  gethiscore(int whichscore);
    public native void sethiscore(int whichscore, int thescore);
    
    private TextView   hs_1, hs_2, hs_3,
                       hs_4, hs_5, hs_6;

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
        TextView button_clearhs = findViewById(R.id.button_clearhs);

        for (int i = 0; i < 51 + 6; i++)
        {   MainActivity.hiscore[i] = gethiscore(i);
        }
        doscores();
        
        button_clearhs.setOnClickListener((view) ->
        {   for (int i = 51; i < 51 + 6; i++)
            {   MainActivity.hiscore[i] = 0;
                sethiscore(i, MainActivity.hiscore[i]);
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
}   }
