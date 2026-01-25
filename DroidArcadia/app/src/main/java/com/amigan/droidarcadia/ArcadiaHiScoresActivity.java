package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.widget.TextView;

public class ArcadiaHiScoresActivity extends Activity
{   public native int  gethiscore(int whichscore);
    public native void sethiscore(int whichscore, int thescore);
    
    private TextView   hs_1,  hs_2,  hs_3,  hs_4,
                       hs_5,  hs_6,  hs_7,  hs_8,
                       hs_9,  hs_10, hs_11, hs_12,
                       hs_13, hs_14, hs_15, hs_16,
                       hs_17, hs_18, hs_19, hs_20,
                       hs_21, hs_22, hs_23, hs_24,
                       hs_25, hs_26, hs_27, hs_28,
                       hs_29, hs_30, hs_31, hs_32,
                       hs_33, hs_34, hs_35, hs_36,
                       hs_37, hs_38, hs_39, hs_40,
                       hs_41, hs_42, hs_43, hs_44,
                       hs_45, hs_46, hs_47, hs_48,
                       hs_49, hs_50, hs_51;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_arcadiahiscores);

        if (MainActivity.lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

                 hs_1           = findViewById(R.id.hs_1);
                 hs_2           = findViewById(R.id.hs_2);
                 hs_3           = findViewById(R.id.hs_3);
                 hs_4           = findViewById(R.id.hs_4);
                 hs_5           = findViewById(R.id.hs_5);
                 hs_6           = findViewById(R.id.hs_6);
                 hs_7           = findViewById(R.id.hs_7);
                 hs_8           = findViewById(R.id.hs_8);
                 hs_9           = findViewById(R.id.hs_9);
                 hs_10          = findViewById(R.id.hs_10);
                 hs_11          = findViewById(R.id.hs_11);
                 hs_12          = findViewById(R.id.hs_12);
                 hs_13          = findViewById(R.id.hs_13);
                 hs_14          = findViewById(R.id.hs_14);
                 hs_15          = findViewById(R.id.hs_15);
                 hs_16          = findViewById(R.id.hs_16);
                 hs_17          = findViewById(R.id.hs_17);
                 hs_18          = findViewById(R.id.hs_18);
                 hs_19          = findViewById(R.id.hs_19);
                 hs_20          = findViewById(R.id.hs_20);
                 hs_21          = findViewById(R.id.hs_21);
                 hs_22          = findViewById(R.id.hs_22);
                 hs_23          = findViewById(R.id.hs_23);
                 hs_24          = findViewById(R.id.hs_24);
                 hs_25          = findViewById(R.id.hs_25);
                 hs_26          = findViewById(R.id.hs_26);
                 hs_27          = findViewById(R.id.hs_27);
                 hs_28          = findViewById(R.id.hs_28);
                 hs_29          = findViewById(R.id.hs_29);
                 hs_30          = findViewById(R.id.hs_30);
                 hs_31          = findViewById(R.id.hs_31);
                 hs_32          = findViewById(R.id.hs_32);
                 hs_33          = findViewById(R.id.hs_33);
                 hs_34          = findViewById(R.id.hs_34);
                 hs_35          = findViewById(R.id.hs_35);
                 hs_36          = findViewById(R.id.hs_36);
                 hs_37          = findViewById(R.id.hs_37);
                 hs_38          = findViewById(R.id.hs_38);
                 hs_39          = findViewById(R.id.hs_39);
                 hs_40          = findViewById(R.id.hs_40);
                 hs_41          = findViewById(R.id.hs_41);
                 hs_42          = findViewById(R.id.hs_42);
                 hs_43          = findViewById(R.id.hs_43);
                 hs_44          = findViewById(R.id.hs_44);
                 hs_45          = findViewById(R.id.hs_45);
                 hs_46          = findViewById(R.id.hs_46);
                 hs_47          = findViewById(R.id.hs_47);
                 hs_48          = findViewById(R.id.hs_48);
                 hs_49          = findViewById(R.id.hs_49);
                 hs_50          = findViewById(R.id.hs_50);
                 hs_51          = findViewById(R.id.hs_51);
        TextView button_clearhs = findViewById(R.id.button_clearhs);

        for (int i = 0; i < 51 + 6; i++)
        {   MainActivity.hiscore[i] = gethiscore(i);
        }
        doscores();
        
        button_clearhs.setOnClickListener((view) ->
        {   for (int i = 0; i < 51 + 6; i++)
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
    {    hs_1.setText(String.valueOf(MainActivity.hiscore[ 0]));
         hs_2.setText(String.valueOf(MainActivity.hiscore[ 1]));
         hs_3.setText(String.valueOf(MainActivity.hiscore[ 2]));
         hs_4.setText(String.valueOf(MainActivity.hiscore[ 3]));
         hs_5.setText(String.valueOf(MainActivity.hiscore[ 4]));
         hs_6.setText(String.valueOf(MainActivity.hiscore[ 5]));
         hs_7.setText(String.valueOf(MainActivity.hiscore[ 6]));
         hs_8.setText(String.valueOf(MainActivity.hiscore[ 7]));
         hs_9.setText(String.valueOf(MainActivity.hiscore[ 8]));
        hs_10.setText(String.valueOf(MainActivity.hiscore[ 9]));
        hs_11.setText(String.valueOf(MainActivity.hiscore[10]));
        hs_12.setText(String.valueOf(MainActivity.hiscore[11]));
        hs_13.setText(String.valueOf(MainActivity.hiscore[12]));
        hs_14.setText(String.valueOf(MainActivity.hiscore[13]));
        hs_15.setText(String.valueOf(MainActivity.hiscore[14]));
        hs_16.setText(String.valueOf(MainActivity.hiscore[15]));
        hs_17.setText(String.valueOf(MainActivity.hiscore[16]));
        hs_18.setText(String.valueOf(MainActivity.hiscore[17]));
        hs_19.setText(String.valueOf(MainActivity.hiscore[18]));
        hs_20.setText(String.valueOf(MainActivity.hiscore[19]));
        hs_21.setText(String.valueOf(MainActivity.hiscore[20]));
        hs_22.setText(String.valueOf(MainActivity.hiscore[21]));
        hs_23.setText(String.valueOf(MainActivity.hiscore[22]));
        hs_24.setText(String.valueOf(MainActivity.hiscore[23]));
        hs_25.setText(String.valueOf(MainActivity.hiscore[24]));
        hs_26.setText(String.valueOf(MainActivity.hiscore[25]));
        hs_27.setText(String.valueOf(MainActivity.hiscore[26]));
        hs_28.setText(String.valueOf(MainActivity.hiscore[27]));
        hs_29.setText(String.valueOf(MainActivity.hiscore[28]));
        hs_30.setText(String.valueOf(MainActivity.hiscore[29]));
        hs_31.setText(String.valueOf(MainActivity.hiscore[30]));
        hs_32.setText(String.valueOf(MainActivity.hiscore[31]));
        hs_33.setText(String.valueOf(MainActivity.hiscore[32]));
        hs_34.setText(String.valueOf(MainActivity.hiscore[33]));
        hs_35.setText(String.valueOf(MainActivity.hiscore[34]));
        hs_36.setText(String.valueOf(MainActivity.hiscore[35]));
        hs_37.setText(String.valueOf(MainActivity.hiscore[36]));
        hs_38.setText(String.valueOf(MainActivity.hiscore[37]));
        hs_39.setText(String.valueOf(MainActivity.hiscore[38]));
        hs_40.setText(String.valueOf(MainActivity.hiscore[39]));
        hs_41.setText(String.valueOf(MainActivity.hiscore[40]));
        hs_42.setText(String.valueOf(MainActivity.hiscore[41]));
        hs_43.setText(String.valueOf(MainActivity.hiscore[42]));
        hs_44.setText(String.valueOf(MainActivity.hiscore[43]));
        hs_45.setText(String.valueOf(MainActivity.hiscore[44]));
        hs_46.setText(String.valueOf(MainActivity.hiscore[45]));
        hs_47.setText(String.valueOf(MainActivity.hiscore[46]));
        hs_48.setText(String.valueOf(MainActivity.hiscore[47]));
        hs_49.setText(String.valueOf(MainActivity.hiscore[48]));
        hs_50.setText(String.valueOf(MainActivity.hiscore[49]));
        hs_51.setText(String.valueOf(MainActivity.hiscore[50]));
}   }
