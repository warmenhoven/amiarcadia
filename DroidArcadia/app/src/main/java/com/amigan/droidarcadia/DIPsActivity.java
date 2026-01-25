package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

public class DIPsActivity extends Activity {
    public native int cgetdips();

    public native void csetdips(int thedips);

    // constants
    public static int COINA_HALF = 0,
            COINA_1 = 1,
            COINA_2 = 2,
            COINA_3 = 3,
            COINA_5 = 4,
            COINB_HALF = 0,
            COINB_1 = 1,
            COINB_2 = 2,
            COINB_3 = 3,
            COINB_5 = 4,
            COINB_7 = 5,
            LIVES_2 = 0,
            LIVES_3 = 1,
            LIVES_4 = 2,
            LIVES_5 = 3,
            LIVES_6 = 4,
            LIVES_INFINITE = 5,
            HISCORE_0 = 0,
            HISCORE_5000 = 1,
            HISCORE_7500 = 2,
            HISCORE_10000 = 3,
            HISCORE_12500 = 4,
            MEMMAP_ASTROWARS = 9,
            MEMMAP_GALAXIA = 10,
            MEMMAP_LASERBATTLE = 11,
            MEMMAP_LAZARIAN = 12,
            ZACCARIA        = 3;

    private TextView label_coinadip, label_coinbdip, label_livesdip, label_hiscoredip;
    private SeekBar seekbar_coinadip, seekbar_coinbdip, seekbar_livesdip, seekbar_hiscoredip;
    private int coinadip, coinbdip, livesdip, hiscoredip;
    private boolean gridgad, collgad, freezegad, rapidfiregad;
    private Switch switch_calibrationgrid,
            switch_detectcollisions,
            switch_freeze,
            switch_rapidfire,
            switch_autocoin;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dips);
        if (MainActivity.lock) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

          label_coinadip         = findViewById(R.id.label_coinadip);
        seekbar_coinadip         = findViewById(R.id.seekbar_coinadip);
          label_coinbdip         = findViewById(R.id.label_coinbdip);
        seekbar_coinbdip         = findViewById(R.id.seekbar_coinbdip);
          label_livesdip         = findViewById(R.id.label_livesdip);
        seekbar_livesdip         = findViewById(R.id.seekbar_livesdip);
          label_hiscoredip       = findViewById(R.id.label_hiscoredip);
        seekbar_hiscoredip       = findViewById(R.id.seekbar_hiscoredip);
         switch_calibrationgrid  = findViewById(R.id.switch_calibrationgrid);
         switch_detectcollisions = findViewById(R.id.switch_detectcollisions);
         switch_freeze           = findViewById(R.id.switch_freeze);
         switch_rapidfire        = findViewById(R.id.switch_rapidfire);
         switch_autocoin         = findViewById(R.id.switch_autocoin);
        TextView button_back     = findViewById(R.id.button_back);

        jgetdips();

        writegadget_coinadip();
        writegadget_coinbdip();
        writegadget_livesdip();
        writegadget_hiscoredip();
        writegadget_calibrationgrid();
        writegadget_detectcollisions();
        writegadget_freeze();
        writegadget_rapidfire();

        if (MainActivity.autocoin)
        {   switch_autocoin.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        switch_autocoin.setChecked(MainActivity.autocoin);

        seekbar_coinadip.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekbar_coinadip, int progress, boolean fromUser) {
                int newdip = seekbar_coinadip.getProgress();

                if
                (    MainActivity.machine != ZACCARIA
                 || (MainActivity.memmap  != MEMMAP_LAZARIAN && newdip == COINA_HALF)
                 || (MainActivity.memmap  == MEMMAP_LAZARIAN && newdip == COINA_5)
                )
                {   ;
                } else
                {   coinadip = newdip;
                }
                writegadget_coinadip();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekbar_coinadip) {
                ;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekbar_coinadip) {
                ;
            }
        });

        seekbar_coinbdip.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekbar_coinbdip, int progress, boolean fromUser) {
                int newdip = seekbar_coinbdip.getProgress();

                if
                (     MainActivity.machine != ZACCARIA
                 || ((MainActivity.memmap  == MEMMAP_ASTROWARS   || MainActivity.memmap == MEMMAP_GALAXIA ) && newdip > COINB_1)
                 || ((MainActivity.memmap  == MEMMAP_LASERBATTLE || MainActivity.memmap == MEMMAP_LAZARIAN) && newdip < COINB_2)
                )
                {   ;
                } else
                {   coinbdip = newdip;
                }
                writegadget_coinbdip();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekbar_coinbdip) {
                ;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekbar_coinbdip) {
                ;
            }
        });

        seekbar_livesdip.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekbar_livesdip, int progress, boolean fromUser) {
                int newdip = seekbar_livesdip.getProgress();

                if
                (     MainActivity.machine != ZACCARIA
                 || ((MainActivity.memmap == MEMMAP_ASTROWARS || MainActivity.memmap == MEMMAP_GALAXIA) && newdip != LIVES_3 && newdip != LIVES_5)
                 ||  (MainActivity.memmap == MEMMAP_LASERBATTLE && newdip == LIVES_4)
                 ||  (MainActivity.memmap == MEMMAP_LAZARIAN && newdip > LIVES_5)
                ) {
                    ;
                } else {
                    livesdip = newdip;
                }
                writegadget_livesdip();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekbar_livesdip) {
                ;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekbar_livesdip) {
                ;
            }
        });

        seekbar_hiscoredip.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekbar_hiscoredip, int progress, boolean fromUser) {
                if (MainActivity.memmap == MEMMAP_ASTROWARS) {
                    hiscoredip = seekbar_hiscoredip.getProgress();
                }
                writegadget_hiscoredip();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekbar_hiscoredip) {
                ;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekbar_hiscoredip) {
                ;
            }
        });

        switch_calibrationgrid.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   if (MainActivity.memmap == MEMMAP_LAZARIAN)
            {   gridgad = isChecked;
            }
            writegadget_calibrationgrid();
        });

        switch_detectcollisions.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   if (MainActivity.memmap == MEMMAP_LASERBATTLE || MainActivity.memmap == MEMMAP_LAZARIAN)
            {   collgad = isChecked;
            }
            writegadget_detectcollisions();
        });

        switch_freeze.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   if (MainActivity.machine == ZACCARIA && MainActivity.memmap != MEMMAP_LASERBATTLE)
            {   freezegad = isChecked;
            }
            writegadget_freeze();
        });

        switch_rapidfire.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   if (MainActivity.memmap == MEMMAP_LAZARIAN)
            {   rapidfiregad = isChecked;
            }
            writegadget_rapidfire();
        });

        switch_autocoin.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.autocoin = isChecked;
            if (isChecked)
            {   switch_autocoin.setBackgroundColor(Color.parseColor("#ffaa00"));
            } else
            {   switch_autocoin.setBackgroundColor(Color.parseColor("#cc6600"));
            }
        });

        button_back.setOnClickListener((view) -> finish());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        jsetdips();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        this.recreate();
    }

    private void jgetdips() {
        int thedips = cgetdips();

        switch (MainActivity.memmap) {
            case 9: // means MEMMAP_ASTROWARS
                if ((thedips & 0x800) == 0x800) {
                    livesdip = LIVES_5;
                } else {
                    livesdip = LIVES_3;
                }
                coinadip = 1 + ((thedips & 0x0300) >> 8); // 1..4 (ie. 1, 2, 3 or 5 lives)
                coinbdip = (thedips & 0x0400) >> 10; // 0..1 (ie. half or 1 credit)
                if ((thedips & 1) == 1) {
                    hiscoredip = 1 + ((thedips & 6) >> 1);
                } else {
                    hiscoredip = 0;
                }
                freezegad = ((thedips & 0x8000) != 0x8000);
                gridgad =
                rapidfiregad = false;
                collgad = true;
                break;
            case 10: // means MEMMAP_GALAXIA
                if ((thedips & 8) == 8) {
                    livesdip = LIVES_5;
                } else {
                    livesdip = LIVES_3;
                }
                hiscoredip = 0;
                coinadip = 1 + (thedips & 0x03); // 1..4 (ie. 1, 2, 3 or 5 lives)
                coinbdip = (thedips & 0x04) >> 2; // 0..1 (ie. half or 1 credit)
                freezegad = ((thedips & 0x80) != 0x80);
                gridgad =
                rapidfiregad = false;
                collgad = true;
                break;
            case 11: // means MEMMAP_LASERBATTLE
                switch (thedips & 0x70) {
                    case 0x00:
                        livesdip = LIVES_2; // 2 lives
                        break;
                    case 0x10:
                        livesdip = LIVES_3; // 3 lives
                        break;
                    case 0x20:
                        livesdip = LIVES_5; // 5 lives
                        break;
                    case 0x30:
                        livesdip = LIVES_6; // 6 lives
                        break;
                    default:
                        livesdip = LIVES_INFINITE; // infinite lives
                }
                coinadip = 1 + (thedips & 0x03);
                coinbdip = 2 + ((thedips & 0x0C) >> 2);
                collgad = ((thedips & 0x80) == 0x80);
                freezegad =
                gridgad =
                rapidfiregad = false;
                hiscoredip = 0;
            break;
            case 12: // means MEMMAP_LAZARIAN
                livesdip     =  (thedips & 0x3000) >> 12     ; // 0..3 (ie. 2..5 lives)
                coinadip     =  (thedips & 0x0300) >>  8     ; // 0..3
                coinbdip = 2 + ((thedips & 0x0C00) >> 10    ); // 2..5
                freezegad    = ((thedips & 0x0004) == 0x0004);
                collgad      = ((thedips & 0x8000) == 0x8000);
                rapidfiregad = ((thedips & 0x0002) == 0x0002);
                gridgad      = ((thedips & 0x4000) == 0x4000);
                hiscoredip = 0;
            break;
            case 13: // means MEMMAP_MALZAK1
            case 14: // means MEMMAP_MALZAK2
                coinadip     = COINA_1;
                coinbdip     = COINB_1;
                livesdip     = LIVES_3;
                hiscoredip   = HISCORE_0;
                collgad      = true;
                freezegad    =
                gridgad      =
                rapidfiregad = false;
        }

        MainActivity.autocoin = ((thedips & 0x10000) == 0x10000);
    }

    private void jsetdips() {
        int thedips = 0;

        switch (MainActivity.memmap) {
            case 9: // means MEMMAP_ASTROWARS
                if (livesdip == LIVES_5) thedips |= 0x800;
                thedips |= (coinadip - 1) << 8; // bits  9..8
                thedips |= coinbdip << 10; // bit  10
                if (hiscoredip != 0) {
                    thedips |= 1;
                    thedips |= (hiscoredip - 1) << 1;
                }
                if (!freezegad)
                {   thedips |= 0x8000; // bit 15
                }
                break;
            case 10: // means MEMMAP_GALAXIA
                if (livesdip == LIVES_5) thedips |= 8;
                thedips |= coinadip - 1; // bits 1..0
                thedips |= coinbdip << 2; // bit  2
                if (!freezegad)
                {   thedips |= 0x80; // bit 7
                }
                break;
            case 11: // means MEMMAP_LASERBATTLE
                if (livesdip == LIVES_2) ;
                else if (livesdip == LIVES_3) thedips |= 0x10;
                else if (livesdip == LIVES_5) thedips |= 0x20;
                else if (livesdip == LIVES_6) thedips |= 0x30;
                else if (livesdip == LIVES_INFINITE) thedips |= 0x40;
                thedips |= coinadip - 1; // bits 1..0
                thedips |= (coinbdip - 2) << 2; // bits 3..2
                if (collgad) {
                    thedips |= 0x80;
                }
            break;
            case 12: // means MEMMAP_LAZARIAN
                thedips |= livesdip << 12;
                thedips |= coinadip << 8;
                thedips |= (coinbdip - 2) << 10;
                if (freezegad) {
                    thedips |= 0x0004;
                }
                if (collgad) {
                    thedips |= 0x8000;
                }
                if (rapidfiregad) {
                    thedips |= 0x0002;
                }
                if (gridgad) {
                    thedips |= 0x4000;
                }
        }

        if (MainActivity.autocoin) {
            thedips |= 0x10000;
        }
        csetdips(thedips);
    }

    private void writegadget_coinadip() {
        if (MainActivity.machine != ZACCARIA)
        {     label_coinadip.setBackgroundColor(Color.parseColor("#555555"));
            seekbar_coinadip.setBackgroundColor(Color.parseColor("#555555"));
        } else if (coinadip == COINA_5) {
              label_coinadip.setBackgroundColor(Color.parseColor("#ffaa00"));
            seekbar_coinadip.setBackgroundColor(Color.parseColor("#ffaa00"));
        } else {
              label_coinadip.setBackgroundColor(Color.parseColor("#cc6600"));
            seekbar_coinadip.setBackgroundColor(Color.parseColor("#cc6600"));
        }
        if (coinadip == COINA_HALF) label_coinadip.setText(R.string.coina_half);
        else if (coinadip == COINA_1) label_coinadip.setText(R.string.coina_1);
        else if (coinadip == COINA_2) label_coinadip.setText(R.string.coina_2);
        else if (coinadip == COINA_3) label_coinadip.setText(R.string.coina_3);
        else if (coinadip == COINA_5) label_coinadip.setText(R.string.coina_5);
        seekbar_coinadip.setProgress(coinadip);
    }

    private void writegadget_coinbdip() {
        if (MainActivity.machine != ZACCARIA)
        {     label_coinbdip.setBackgroundColor(Color.parseColor("#555555"));
            seekbar_coinbdip.setBackgroundColor(Color.parseColor("#555555"));
        } else if (coinbdip == COINB_1) {
              label_coinbdip.setBackgroundColor(Color.parseColor("#ffaa00"));
            seekbar_coinbdip.setBackgroundColor(Color.parseColor("#ffaa00"));
        } else {
              label_coinbdip.setBackgroundColor(Color.parseColor("#cc6600"));
            seekbar_coinbdip.setBackgroundColor(Color.parseColor("#cc6600"));
        }
        if (coinbdip == COINB_HALF) label_coinbdip.setText(R.string.coinb_half);
        else if (coinbdip == COINB_1) label_coinbdip.setText(R.string.coinb_1);
        else if (coinbdip == COINB_2) label_coinbdip.setText(R.string.coinb_2);
        else if (coinbdip == COINB_3) label_coinbdip.setText(R.string.coinb_3);
        else if (coinbdip == COINB_5) label_coinbdip.setText(R.string.coinb_5);
        else if (coinbdip == COINB_7) label_coinbdip.setText(R.string.coinb_7);
        seekbar_coinbdip.setProgress(coinbdip);
    }

    private void writegadget_livesdip() {
        if (MainActivity.machine != ZACCARIA)
        {     label_livesdip.setBackgroundColor(Color.parseColor("#555555"));
            seekbar_livesdip.setBackgroundColor(Color.parseColor("#555555"));
        } else if (livesdip == LIVES_5) {
              label_livesdip.setBackgroundColor(Color.parseColor("#ff0000"));
            seekbar_livesdip.setBackgroundColor(Color.parseColor("#ff0000"));
        } else {
              label_livesdip.setBackgroundColor(Color.parseColor("#cc0000"));
            seekbar_livesdip.setBackgroundColor(Color.parseColor("#cc0000"));
        }
        if      (livesdip == LIVES_2       ) label_livesdip.setText(R.string.lives_2);
        else if (livesdip == LIVES_3       ) label_livesdip.setText(R.string.lives_3);
        else if (livesdip == LIVES_4       ) label_livesdip.setText(R.string.lives_4);
        else if (livesdip == LIVES_5       ) label_livesdip.setText(R.string.lives_5);
        else if (livesdip == LIVES_6       ) label_livesdip.setText(R.string.lives_6);
        else if (livesdip == LIVES_INFINITE) label_livesdip.setText(R.string.lives_infinite);
        seekbar_livesdip.setProgress(livesdip);
    }

    private void writegadget_hiscoredip() {
        if (MainActivity.memmap != MEMMAP_ASTROWARS) {
              label_hiscoredip.setBackgroundColor(Color.parseColor("#555555"));
            seekbar_hiscoredip.setBackgroundColor(Color.parseColor("#555555"));
        } else if (hiscoredip == HISCORE_0) {
              label_hiscoredip.setBackgroundColor(Color.parseColor("#cccccc"));
            seekbar_hiscoredip.setBackgroundColor(Color.parseColor("#cccccc"));
        } else {
              label_hiscoredip.setBackgroundColor(Color.parseColor("#aaaaaa"));
            seekbar_hiscoredip.setBackgroundColor(Color.parseColor("#aaaaaa"));
        }
        if (hiscoredip == HISCORE_0) label_hiscoredip.setText(R.string.hiscoredip_0);
        else if (hiscoredip == HISCORE_5000) label_hiscoredip.setText(R.string.hiscoredip_5000);
        else if (hiscoredip == HISCORE_7500) label_hiscoredip.setText(R.string.hiscoredip_7500);
        else if (hiscoredip == HISCORE_10000) label_hiscoredip.setText(R.string.hiscoredip_10000);
        else if (hiscoredip == HISCORE_12500) label_hiscoredip.setText(R.string.hiscoredip_12500);
        seekbar_hiscoredip.setProgress(hiscoredip);
    }

    private void writegadget_calibrationgrid()
    {   if (MainActivity.memmap != MEMMAP_LAZARIAN)
        {   switch_calibrationgrid.setBackgroundColor(Color.parseColor("#555555"));
        } else if (gridgad)
        {   switch_calibrationgrid.setBackgroundColor(Color.parseColor("#00ff00"));
        } else
        {   switch_calibrationgrid.setBackgroundColor(Color.parseColor("#00cc00"));
        }
        switch_calibrationgrid.setChecked(gridgad);
    }

    private void writegadget_detectcollisions()
    {   if (MainActivity.memmap != MEMMAP_LASERBATTLE && MainActivity.memmap != MEMMAP_LAZARIAN)
        {   switch_detectcollisions.setBackgroundColor(Color.parseColor("#555555"));
        } else if (collgad)
        {   switch_detectcollisions.setBackgroundColor(Color.parseColor("#ff00ff"));
        } else
        {   switch_detectcollisions.setBackgroundColor(Color.parseColor("#cc00cc"));
        }
        switch_detectcollisions.setChecked(collgad);
    }

    private void writegadget_freeze()
    {   if (MainActivity.machine != ZACCARIA || MainActivity.memmap == MEMMAP_LASERBATTLE)
        {   switch_freeze.setBackgroundColor(Color.parseColor("#555555"));
        } else if (freezegad)
        {   switch_freeze.setBackgroundColor(Color.parseColor("#00ffff"));
        } else
        {   switch_freeze.setBackgroundColor(Color.parseColor("#00dddd"));
        }
        switch_freeze.setChecked(freezegad);
    }

    private void writegadget_rapidfire() {
        if (MainActivity.memmap != MEMMAP_LAZARIAN) {
            switch_rapidfire.setBackgroundColor(Color.parseColor("#555555"));
        } else if (rapidfiregad) {
            switch_rapidfire.setBackgroundColor(Color.parseColor("#ffaa00"));
        } else {
            switch_rapidfire.setBackgroundColor(Color.parseColor("#cc6600"));
        }
        switch_rapidfire.setChecked(rapidfiregad);
    }
}
