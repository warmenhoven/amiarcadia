package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.widget.TextView;

public class ArcadiaHiScoresActivity extends Activity
{   public native int    gethiscore(int whichscore);
    public native String gethiscoredate(int whichscore);
    public native void   sethiscore(int whichscore, int thescore, String thedate);
    
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
                       hs_49, hs_50, hs_51,
                       hsd_1,  hsd_2,  hsd_3,  hsd_4,
                       hsd_5,  hsd_6,  hsd_7,  hsd_8,
                       hsd_9,  hsd_10, hsd_11, hsd_12,
                       hsd_13, hsd_14, hsd_15, hsd_16,
                       hsd_17, hsd_18, hsd_19, hsd_20,
                       hsd_21, hsd_22, hsd_23, hsd_24,
                       hsd_25, hsd_26, hsd_27, hsd_28,
                       hsd_29, hsd_30, hsd_31, hsd_32,
                       hsd_33, hsd_34, hsd_35, hsd_36,
                       hsd_37, hsd_38, hsd_39, hsd_40,
                       hsd_41, hsd_42, hsd_43, hsd_44,
                       hsd_45, hsd_46, hsd_47, hsd_48,
                       hsd_49, hsd_50, hsd_51;

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
                 hsd_1          = findViewById(R.id.hsd_1);
                 hsd_2          = findViewById(R.id.hsd_2);
                 hsd_3          = findViewById(R.id.hsd_3);
                 hsd_4          = findViewById(R.id.hsd_4);
                 hsd_5          = findViewById(R.id.hsd_5);
                 hsd_6          = findViewById(R.id.hsd_6);
                 hsd_7          = findViewById(R.id.hsd_7);
                 hsd_8          = findViewById(R.id.hsd_8);
                 hsd_9          = findViewById(R.id.hsd_9);
                 hsd_10         = findViewById(R.id.hsd_10);
                 hsd_11         = findViewById(R.id.hsd_11);
                 hsd_12         = findViewById(R.id.hsd_12);
                 hsd_13         = findViewById(R.id.hsd_13);
                 hsd_14         = findViewById(R.id.hsd_14);
                 hsd_15         = findViewById(R.id.hsd_15);
                 hsd_16         = findViewById(R.id.hsd_16);
                 hsd_17         = findViewById(R.id.hsd_17);
                 hsd_18         = findViewById(R.id.hsd_18);
                 hsd_19         = findViewById(R.id.hsd_19);
                 hsd_20         = findViewById(R.id.hsd_20);
                 hsd_21         = findViewById(R.id.hsd_21);
                 hsd_22         = findViewById(R.id.hsd_22);
                 hsd_23         = findViewById(R.id.hsd_23);
                 hsd_24         = findViewById(R.id.hsd_24);
                 hsd_25         = findViewById(R.id.hsd_25);
                 hsd_26         = findViewById(R.id.hsd_26);
                 hsd_27         = findViewById(R.id.hsd_27);
                 hsd_28         = findViewById(R.id.hsd_28);
                 hsd_29         = findViewById(R.id.hsd_29);
                 hsd_30         = findViewById(R.id.hsd_30);
                 hsd_31         = findViewById(R.id.hsd_31);
                 hsd_32         = findViewById(R.id.hsd_32);
                 hsd_33         = findViewById(R.id.hsd_33);
                 hsd_34         = findViewById(R.id.hsd_34);
                 hsd_35         = findViewById(R.id.hsd_35);
                 hsd_36         = findViewById(R.id.hsd_36);
                 hsd_37         = findViewById(R.id.hsd_37);
                 hsd_38         = findViewById(R.id.hsd_38);
                 hsd_39         = findViewById(R.id.hsd_39);
                 hsd_40         = findViewById(R.id.hsd_40);
                 hsd_41         = findViewById(R.id.hsd_41);
                 hsd_42         = findViewById(R.id.hsd_42);
                 hsd_43         = findViewById(R.id.hsd_43);
                 hsd_44         = findViewById(R.id.hsd_44);
                 hsd_45         = findViewById(R.id.hsd_45);
                 hsd_46         = findViewById(R.id.hsd_46);
                 hsd_47         = findViewById(R.id.hsd_47);
                 hsd_48         = findViewById(R.id.hsd_48);
                 hsd_49         = findViewById(R.id.hsd_49);
                 hsd_50         = findViewById(R.id.hsd_50);
                 hsd_51         = findViewById(R.id.hsd_51);
        TextView button_clearhs = findViewById(R.id.button_clearhs);

        for (int i = 0; i < 51 + 6; i++)
        {   MainActivity.hiscore[i] = gethiscore(i);
            MainActivity.hiscoredate[i] = gethiscoredate(i);
        }
        doscores();
        
        button_clearhs.setOnClickListener((view) ->
        {   for (int i = 0; i < 51; i++)
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

        hsd_1.setText(MainActivity.hiscoredate[0]);
        hsd_2.setText(MainActivity.hiscoredate[1]);
        hsd_3.setText(MainActivity.hiscoredate[2]);
        hsd_4.setText(MainActivity.hiscoredate[3]);
        hsd_5.setText(MainActivity.hiscoredate[4]);
        hsd_6.setText(MainActivity.hiscoredate[5]);
        hsd_7.setText(MainActivity.hiscoredate[6]);
        hsd_8.setText(MainActivity.hiscoredate[7]);
        hsd_9.setText(MainActivity.hiscoredate[8]);
        hsd_10.setText(MainActivity.hiscoredate[9]);
        hsd_11.setText(MainActivity.hiscoredate[10]);
        hsd_12.setText(MainActivity.hiscoredate[11]);
        hsd_13.setText(MainActivity.hiscoredate[12]);
        hsd_14.setText(MainActivity.hiscoredate[13]);
        hsd_15.setText(MainActivity.hiscoredate[14]);
        hsd_16.setText(MainActivity.hiscoredate[15]);
        hsd_17.setText(MainActivity.hiscoredate[16]);
        hsd_18.setText(MainActivity.hiscoredate[17]);
        hsd_19.setText(MainActivity.hiscoredate[18]);
        hsd_20.setText(MainActivity.hiscoredate[19]);
        hsd_21.setText(MainActivity.hiscoredate[20]);
        hsd_22.setText(MainActivity.hiscoredate[21]);
        hsd_23.setText(MainActivity.hiscoredate[22]);
        hsd_24.setText(MainActivity.hiscoredate[23]);
        hsd_25.setText(MainActivity.hiscoredate[24]);
        hsd_26.setText(MainActivity.hiscoredate[25]);
        hsd_27.setText(MainActivity.hiscoredate[26]);
        hsd_28.setText(MainActivity.hiscoredate[27]);
        hsd_29.setText(MainActivity.hiscoredate[28]);
        hsd_30.setText(MainActivity.hiscoredate[29]);
        hsd_31.setText(MainActivity.hiscoredate[30]);
        hsd_32.setText(MainActivity.hiscoredate[31]);
        hsd_33.setText(MainActivity.hiscoredate[32]);
        hsd_34.setText(MainActivity.hiscoredate[33]);
        hsd_35.setText(MainActivity.hiscoredate[34]);
        hsd_36.setText(MainActivity.hiscoredate[35]);
        hsd_37.setText(MainActivity.hiscoredate[36]);
        hsd_38.setText(MainActivity.hiscoredate[37]);
        hsd_39.setText(MainActivity.hiscoredate[38]);
        hsd_40.setText(MainActivity.hiscoredate[39]);
        hsd_41.setText(MainActivity.hiscoredate[40]);
        hsd_42.setText(MainActivity.hiscoredate[41]);
        hsd_43.setText(MainActivity.hiscoredate[42]);
        hsd_44.setText(MainActivity.hiscoredate[43]);
        hsd_45.setText(MainActivity.hiscoredate[44]);
        hsd_46.setText(MainActivity.hiscoredate[45]);
        hsd_47.setText(MainActivity.hiscoredate[46]);
        hsd_48.setText(MainActivity.hiscoredate[47]);
        hsd_49.setText(MainActivity.hiscoredate[48]);
        hsd_50.setText(MainActivity.hiscoredate[49]);
        hsd_51.setText(MainActivity.hiscoredate[50]);
}   }
