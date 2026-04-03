package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class OptionsActivity extends Activity
{   public native int     cgetoptions1();
    public native int     cgetoptions2();
    public native int     cloadcos(int whichtype, int thelength, byte[] thefile, boolean reconfigure);
    public native void    creset();
    public native byte[]  csavecos();
    public native void    csetoptions(int options1, int options2);
    public native void    csoundoff();
    public native int     getdigipos(int whichpos);
    public native int     getfirebuttons();
    public native int     getgame();
    public native boolean jsetravars(String jusername, String jpassword, boolean jcheevos, boolean jhardcore);
    public native void    redrawscreen();
    public native byte[]  savescreenshot();
    public native boolean skiplevel();

    // constants
    public static int     ARCADIA  = 0,
                          ZACCARIA = 3,
                          MALZAK   = 4;

    private String   fn_screenshot;
    private TextView button_quickload, button_colours, button_autofire,
                     button_handedness, button_machine, button_indicator,
                     button_swapped, button_region, button_levelskip,
                     button_demultiplex,
                     label_stretch, label_size, label_paddle,
                     label_speed, label_frameskip, label_volume,
                     label_sensitivity;
    private Switch   switch_artefacts, switch_collisions, switch_darkenbg,
                     switch_linebased, switch_dejitter, switch_fill,
                     switch_flagline, switch_invincibility, switch_lives,
                     switch_skies, switch_sound, switch_time;
    private SeekBar  seekbar_stretch, seekbar_size, seekbar_paddle,
                     seekbar_speed, seekbar_frameskip, seekbar_volume,
                     seekbar_sensitivity;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {   super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_options);
        if (MainActivity.lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

        MainActivity.semipaused = true;
        jsoundoff();
        csoundoff();

        TextView button_reset         = findViewById(R.id.button_reset);
                 button_quickload     = findViewById(R.id.button_quickload);
        TextView button_quicksave     = findViewById(R.id.button_quicksave);
        TextView button_screenshot    = findViewById(R.id.button_screenshot);
        TextView button_coinopoptions = findViewById(R.id.button_coinopoptions);
        TextView button_raoptions     = findViewById(R.id.button_raoptions);
                 button_colours       = findViewById(R.id.button_colours);
                 switch_artefacts     = findViewById(R.id.switch_artefacts);
                 switch_darkenbg      = findViewById(R.id.switch_darkenbg);
                 switch_flagline      = findViewById(R.id.switch_flagline);
                 switch_linebased     = findViewById(R.id.switch_linebased);
        Switch   switch_lock          = findViewById(R.id.switch_lock);
                 label_stretch        = findViewById(R.id.label_stretch);
                 seekbar_stretch      = findViewById(R.id.seekbar_stretch);
                 switch_dejitter      = findViewById(R.id.switch_dejitter);
                 switch_skies         = findViewById(R.id.switch_skies);
                 switch_fill          = findViewById(R.id.switch_fill);
                 button_autofire      = findViewById(R.id.button_autofire);
                 button_swapped       = findViewById(R.id.button_swapped);
        Switch   switch_sameplayer    = findViewById(R.id.switch_sameplayer);
        Switch   switch_ff            = findViewById(R.id.switch_ff);
                 button_handedness    = findViewById(R.id.button_handedness);
                 label_size           = findViewById(R.id.label_size);
                 seekbar_size         = findViewById(R.id.seekbar_size);
                 label_paddle         = findViewById(R.id.label_paddle);
                 seekbar_paddle       = findViewById(R.id.seekbar_paddle);
        Switch   switch_spring        = findViewById(R.id.switch_spring);
                 label_speed          = findViewById(R.id.label_speed);
                 seekbar_speed        = findViewById(R.id.seekbar_speed);
                 button_region        = findViewById(R.id.button_region);
                 button_indicator     = findViewById(R.id.button_indicator);
                 label_frameskip      = findViewById(R.id.label_frameskip);
                 seekbar_frameskip    = findViewById(R.id.seekbar_frameskip);
        Switch   switch_autoframeskip = findViewById(R.id.switch_autoframeskip);
                 label_sensitivity    = findViewById(R.id.label_sensitivity);
                 seekbar_sensitivity  = findViewById(R.id.seekbar_sensitivity);
                 button_machine       = findViewById(R.id.button_machine);
                 switch_sound         = findViewById(R.id.switch_sound);
                 label_volume         = findViewById(R.id.label_volume);
                 seekbar_volume       = findViewById(R.id.seekbar_volume);
        Switch   switch_retune        = findViewById(R.id.switch_retune);
                 button_demultiplex   = findViewById(R.id.button_demultiplex);
        TextView button_hiscores      = findViewById(R.id.button_hiscores);
                 switch_collisions    = findViewById(R.id.switch_collisions);
                 switch_lives         = findViewById(R.id.switch_lives);
                 switch_time          = findViewById(R.id.switch_time);
                 switch_invincibility = findViewById(R.id.switch_invincibility);
                 button_levelskip     = findViewById(R.id.button_levelskip);
        TextView button_gameinfo      = findViewById(R.id.button_gameinfo);
        TextView button_about         = findViewById(R.id.button_about);
        TextView button_back          = findViewById(R.id.button_back);

        jgetoptions();
        
        if (MainActivity.machine != ZACCARIA && MainActivity.machine != MALZAK)
        {   button_coinopoptions.setBackgroundColor(Color.parseColor("#555566"));
        }
        writegadget_colours();
        writegadget_artefacts();
        writegadget_darkenbg();
        writegadget_flagline();
        writegadget_linebased();
        if (MainActivity.lock)
        {   switch_lock.setBackgroundColor(Color.parseColor("#9988ff"));
        }
        switch_lock.setChecked(MainActivity.lock);
        writegadget_stretch();
        if (MainActivity.dejitter)
        {   switch_dejitter.setBackgroundColor(Color.parseColor("#00ff00"));
        }
        switch_dejitter.setChecked(MainActivity.dejitter);
        if (MainActivity.skies)
        {   switch_skies.setBackgroundColor(Color.parseColor("#00ff00"));
        }
        switch_skies.setChecked(MainActivity.skies);
        if (MainActivity.fill)
        {   switch_fill.setBackgroundColor(Color.parseColor("#00ff00"));
        }
        switch_fill.setChecked(MainActivity.fill);
        writegadget_autofire();
        writegadget_swapped();
        switch_sameplayer.setChecked(MainActivity.sameplayer);
        if (MainActivity.sameplayer)
        {   switch_sameplayer.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        switch_ff.setChecked(MainActivity.useff);
        if (MainActivity.useff)
        {   switch_ff.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        writegadget_handedness();
        writegadget_size();
        writegadget_paddle();
        if (MainActivity.spring)
        {   switch_spring.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        switch_spring.setChecked(MainActivity.spring);
        writegadget_speed();
        writegadget_region();
        writegadget_indicator();
        writegadget_frameskip();
        if (MainActivity.autoframeskip)
        {   switch_autoframeskip.setBackgroundColor(Color.parseColor("#00ffff"));
        }
        switch_autoframeskip.setChecked(MainActivity.autoframeskip);
        writegadget_sensitivity();
        writegadget_machine();
        if (MainActivity.sound)
        {   switch_sound.setBackgroundColor(Color.parseColor("#ffff00"));
            switch_sound.setCompoundDrawablesWithIntrinsicBounds(R.drawable.soundon, 0, 0, 0);
        }
        switch_sound.setChecked(MainActivity.sound);
        writegadget_volume();
        if (MainActivity.retune)
        {   switch_retune.setBackgroundColor(Color.parseColor("#ffff00"));
        }
        switch_retune.setChecked(MainActivity.retune);

        update_trainers();
        
        writegadget_demultiplex();

        if
        (   MainActivity.machine == ARCADIA
         || MainActivity.machine == ZACCARIA
         || MainActivity.machine == MALZAK
        )
        {   button_hiscores.setBackgroundColor(Color.parseColor("#ddcccc"));
        } else
        {   button_hiscores.setBackgroundColor(Color.parseColor("#555566"));
        }                
        
        button_reset.setOnClickListener((view) ->
        {   creset();
            button_reset.setBackgroundColor(Color.parseColor("#ffffff"));
            button_reset.setText(R.string.resetted);
            finish();
        });

        button_quickload.setOnClickListener((view) ->
        {   if (!MainActivity.cheevos || !MainActivity.hardcore)
            {   jloadcos(1);
                button_quickload.setBackgroundColor(Color.parseColor("#ffffff"));
                button_quickload.setText(R.string.quickloaded);
                finish();
            }
        });

        button_quicksave.setOnClickListener((view) ->
        {   jsavecos(1);
            button_quicksave.setBackgroundColor(Color.parseColor("#ffffff"));
            button_quicksave.setText(R.string.quicksaved);
            finish();
        });

        button_screenshot.setOnClickListener((view) ->
        {   File rootPath = new File("/sdcard/Pictures/DroidArcadia");
            // File rootPath = new File(Environment.getExternalStorageDirectory() + "/Pictures/DroidArcadia"); // gives /storage/emulated/0/Pictures/DroidArcadia
            if (!rootPath.exists())
            {   rootPath.mkdirs();
            }
            fn_screenshot = "/sdcard/Pictures/DroidArcadia/" + MainActivity.nextscrnshot + ".gif";
            // fn_screenshot = Environment.getExternalStorageDirectory() + "/Pictures/DroidArcadia/" + String.valueOf(MainActivity.nextscrnshot) + ".gif"; // gives /storage/emulated/0/Pictures/DroidArcadia/Foo.gif
            try
            {   FileOutputStream fos = new FileOutputStream(fn_screenshot);
                try
                {   byte thedata[] = savescreenshot();
                    fos.write(thedata, 0, thedata.length);
                    Toast.makeText(OptionsActivity.this, getResources().getString(R.string.savedscreenshotas) + " " + fn_screenshot, Toast.LENGTH_SHORT).show();
                } catch (Exception e)
                {   Toast.makeText(OptionsActivity.this, e.getMessage(), Toast.LENGTH_LONG).show();
                } finally
                {   fos.close();
            }   }
            catch (Exception e)
            {   Toast.makeText(OptionsActivity.this, e.getMessage(), Toast.LENGTH_LONG).show();
            }
            if (MainActivity.nextscrnshot == 65535)
            {   MainActivity.nextscrnshot = 1;
            } else
            {   MainActivity.nextscrnshot++;
            }
            button_screenshot.setBackgroundColor(Color.parseColor("#ffffff"));
            button_screenshot.setText(R.string.savedscreenshot);
            finish();
        });

        button_coinopoptions.setOnClickListener((view) ->
        {   if (MainActivity.machine == ZACCARIA || MainActivity.machine == MALZAK)
            {   Intent myIntent = new Intent(OptionsActivity.this, DIPsActivity.class);
                OptionsActivity.this.startActivity(myIntent);
            } // else do nothing
        });

        button_raoptions.setOnClickListener((view) ->
        {   Intent myIntent = new Intent(OptionsActivity.this, RAActivity.class);
            OptionsActivity.this.startActivity(myIntent);
        });

        button_colours.setOnClickListener((view) ->
        {   if (MainActivity.colours == 3)
            {   MainActivity.colours = 0;
            } else
            {   MainActivity.colours++;
            }
            writegadget_colours();
        });

        switch_artefacts.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.artefacts = isChecked;
            writegadget_artefacts();
        });

        switch_darkenbg.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.darkenbg = isChecked;
            writegadget_darkenbg();
        });

        switch_flagline.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.flagline = isChecked;
            writegadget_flagline();
        });

        seekbar_stretch.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_stretch, int progress, boolean fromUser)
            {   MainActivity.stretch = seekbar_stretch.getProgress();
                writegadget_stretch();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_stretch) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_stretch) { ; }
        });

        switch_dejitter.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.dejitter = isChecked;
            if (isChecked)
            {   switch_dejitter.setBackgroundColor(Color.parseColor("#00ff00"));
            } else
            {   switch_dejitter.setBackgroundColor(Color.parseColor("#00cc00"));
            }
        });

        switch_skies.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.skies = isChecked;
            if (isChecked)
            {   switch_skies.setBackgroundColor(Color.parseColor("#00ff00"));
            } else
            {   switch_skies.setBackgroundColor(Color.parseColor("#00cc00"));
            }
        });

        switch_fill.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.fill = isChecked;
            if (isChecked)
            {   switch_fill.setBackgroundColor(Color.parseColor("#00ff00"));
            } else
            {   switch_fill.setBackgroundColor(Color.parseColor("#00cc00"));
            }
        });

        button_autofire.setOnClickListener((view) ->
        {   if (MainActivity.autofire == 2)
            {   MainActivity.autofire = 0;
            } else
            {   MainActivity.autofire++;
            }
            writegadget_autofire();
        });

        button_swapped.setOnClickListener((view) ->
        {   MainActivity.swapped = !MainActivity.swapped;
            writegadget_swapped();
        });

        switch_sameplayer.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.sameplayer = isChecked;
            if (isChecked)
            {   switch_sameplayer.setBackgroundColor(Color.parseColor("#ffaa00"));
            } else
            {   switch_sameplayer.setBackgroundColor(Color.parseColor("#cc6600"));
            }
        });

        switch_ff.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.useff = isChecked;
            if (isChecked)
            {   switch_ff.setBackgroundColor(Color.parseColor("#ffaa00"));
            } else
            {   switch_ff.setBackgroundColor(Color.parseColor("#cc6600"));
            }
        });

        button_handedness.setOnClickListener((view) ->
        {   MainActivity.handedness = !MainActivity.handedness;
            writegadget_handedness();
        });

        seekbar_size.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_size, int progress, boolean fromUser)
            {   MainActivity.sizemethod = seekbar_size.getProgress();
                writegadget_size();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_size) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_size) { ; }
        });

        seekbar_paddle.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_paddle, int progress, boolean fromUser)
            {   MainActivity.layout = seekbar_paddle.getProgress();
                writegadget_paddle();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_paddle) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_paddle) { ; }
        });

        switch_spring.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.spring = isChecked;
            if (isChecked)
            {   switch_spring.setBackgroundColor(Color.parseColor("#ffaa00"));
            } else
            {   switch_spring.setBackgroundColor(Color.parseColor("#cc6600"));
            }
        });

        seekbar_speed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_speed, int progress, boolean fromUser)
            {   MainActivity.speed = seekbar_speed.getProgress();
                writegadget_speed();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_speed) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_speed) { ; }
        });

        button_region.setOnClickListener((view) ->
        {   if (MainActivity.machine == 0) // ARCADIA
            {   MainActivity.ispal = !MainActivity.ispal;
                writegadget_region();
            }
        });

        button_indicator.setOnClickListener((view) ->
        {   if (MainActivity.indicator == 3)
            {   MainActivity.indicator = 0;
            } else
            {   MainActivity.indicator++;
            }
            writegadget_indicator();
        });

        seekbar_frameskip.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_frameskip, int progress, boolean fromUser)
            {   MainActivity.frameskip = seekbar_frameskip.getProgress();
                writegadget_frameskip();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_frameskip) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_frameskip) { ; }
        });

        switch_autoframeskip.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.autoframeskip = isChecked;
            if (isChecked)
            {   switch_autoframeskip.setBackgroundColor(Color.parseColor("#00ffff"));
            } else
            {   switch_autoframeskip.setBackgroundColor(Color.parseColor("#00dddd"));
            }
        });

        seekbar_sensitivity.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_sensitivity, int progress, boolean fromUser)
            {   MainActivity.sensitivity = seekbar_sensitivity.getProgress();
                writegadget_sensitivity();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_sensitivity) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_sensitivity) { ; }
        });

        button_machine.setOnClickListener((view) ->
        {   if      (MainActivity.machine ==  0) { MainActivity.machine = 1;                           } // Arcadia      -> Interton
            else if (MainActivity.machine ==  1) { MainActivity.machine = 2;                           } // Interton     -> Elektor
            else if (MainActivity.machine ==  2) { MainActivity.machine = 3; MainActivity.memmap =  9; } // Elektor      -> Astro Wars
            else if (MainActivity.memmap  ==  9) {                           MainActivity.memmap = 10; } // Astro Wars   -> Galaxia
            else if (MainActivity.memmap  == 10) {                           MainActivity.memmap = 11; } // Galaxia      -> Laser Battle
            else if (MainActivity.memmap  == 11) {                           MainActivity.memmap = 12; } // Laser Battle -> Lazarian
            else if (MainActivity.memmap  == 12) { MainActivity.machine = 4; MainActivity.memmap = 13; } // Lazarian     -> Malzak 1
            else if (MainActivity.memmap  == 13) {                           MainActivity.memmap = 14; } // Malzak 1     -> Malzak 2
            else if (MainActivity.memmap  == 14) { MainActivity.machine = 0;                           } // Malzak 2     -> Arcadia
            writegadget_machine();
        });

        switch_sound.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.sound = isChecked;
            if (isChecked)
            {   switch_sound.setBackgroundColor(Color.parseColor("#ffff00"));
                switch_sound.setCompoundDrawablesWithIntrinsicBounds(R.drawable.soundon, 0, 0, 0);
            } else
            {   switch_sound.setBackgroundColor(Color.parseColor("#cccc00"));
                switch_sound.setCompoundDrawablesWithIntrinsicBounds(R.drawable.soundoff, 0, 0, 0);
                for (int i = 0; i < MainActivity.GUESTCHANNELS; i++)
                {   if (MainActivity.playing[i])
                    {   MainActivity.audio[i].stop();
                        MainActivity.playing[i] = false;
            }   }   }
        });

        seekbar_volume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {   @Override
            public void onProgressChanged(SeekBar seekbar_volume, int progress, boolean fromUser)
            {   MainActivity.volume = seekbar_volume.getProgress();
                writegadget_volume();
            }

            @Override public void onStartTrackingTouch(SeekBar seekbar_volume) { ; }
            @Override public void onStopTrackingTouch( SeekBar seekbar_volume) { ; }
        });

        switch_retune.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.retune = isChecked;
            if (isChecked)
            {   switch_retune.setBackgroundColor(Color.parseColor("#ffff00"));
            } else
            {   switch_retune.setBackgroundColor(Color.parseColor("#cccc00"));
            }
        });

        switch_collisions.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.collisions = isChecked;
            if (MainActivity.cheevos && MainActivity.hardcore)
            {   switch_collisions.setBackgroundColor(Color.parseColor("#555566"));
            } else if (isChecked)
            {   switch_collisions.setBackgroundColor(Color.parseColor("#ff00ff"));
            } else
            {   switch_collisions.setBackgroundColor(Color.parseColor("#cc00cc"));
            }
            if (isChecked)
            {   switch_collisions.setCompoundDrawablesWithIntrinsicBounds(R.drawable.collisionson, 0, 0, 0);
            } else
            {   switch_collisions.setCompoundDrawablesWithIntrinsicBounds(R.drawable.collisionsoff, 0, 0, 0);
            }
        });

        button_demultiplex.setOnClickListener((view) ->
        {   if (MainActivity.demultiplex == 2)
            {   MainActivity.demultiplex = 0;
            } else
            {   MainActivity.demultiplex++;
            }
            writegadget_demultiplex();
        });

        switch_lives.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.lives = isChecked;
            if (MainActivity.cheevos && MainActivity.hardcore)
            {   switch_lives.setBackgroundColor(Color.parseColor("#555566"));
            } else if (isChecked)
            {   switch_lives.setBackgroundColor(Color.parseColor("#ff0000"));
            } else
            {   switch_lives.setBackgroundColor(Color.parseColor("#cc0000"));
            }
        });

        switch_time.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.time = isChecked;
            if (MainActivity.cheevos && MainActivity.hardcore)
            {   switch_time.setBackgroundColor(Color.parseColor("#555566"));
            } else if (isChecked)
            {   switch_time.setBackgroundColor(Color.parseColor("#ff0000"));
            } else
            {   switch_time.setBackgroundColor(Color.parseColor("#cc0000"));
            }
        });

        switch_invincibility.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.invincibility = isChecked;
            if (MainActivity.cheevos && MainActivity.hardcore)
            {   switch_invincibility.setBackgroundColor(Color.parseColor("#555566"));
            } else if (isChecked)
            {   switch_invincibility.setBackgroundColor(Color.parseColor("#ff0000"));
            } else
            {   switch_invincibility.setBackgroundColor(Color.parseColor("#cc0000"));
            }
        });

        switch_linebased.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.linebased = isChecked;
            writegadget_linebased();
        });

        switch_lock.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.lock = isChecked;
            if (isChecked)
            {   switch_lock.setBackgroundColor(Color.parseColor("#9988ff"));
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
            } else
            {   switch_lock.setBackgroundColor(Color.parseColor("#6655ff"));
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            }
        });

        button_hiscores.setOnClickListener((view) ->
        {   if (MainActivity.machine == ARCADIA)
            {   Intent myIntent = new Intent(OptionsActivity.this, ArcadiaHiScoresActivity.class);
                OptionsActivity.this.startActivity(myIntent);
            } else if (MainActivity.machine == ZACCARIA || MainActivity.machine == MALZAK)
            {   Intent myIntent = new Intent(OptionsActivity.this, CoinOpHiScoresActivity.class);
                OptionsActivity.this.startActivity(myIntent);
            } // else do nothing
        });

        button_levelskip.setOnClickListener((view) ->
        {   if (skiplevel())
            {   button_levelskip.setBackgroundColor(Color.parseColor("#ff0000"));
                button_levelskip.setText(R.string.skippedlevel);
                finish();
            } else
            {   button_levelskip.setText(getResources().getString(R.string.cantskiplevel));
            }
        });

        button_gameinfo.setOnClickListener((view) ->
        {   Intent myIntent = new Intent(OptionsActivity.this, GameInfoActivity.class);
            OptionsActivity.this.startActivity(myIntent);
        });

        button_about.setOnClickListener((view) ->
        {   Intent myIntent = new Intent(OptionsActivity.this, AboutActivity.class);
            OptionsActivity.this.startActivity(myIntent);
        });

        button_back.setOnClickListener((view) -> finish());
    }

    @Override
    protected void onDestroy()
    {   super.onDestroy();

        int options1, options2;

        options1 = 0;
        if (MainActivity.linebased    ) options1 |=  1       ;                    // bit       0 (1 bit )
        if (MainActivity.artefacts    ) options1 |= (1 <<  1);                    // bit       1 (1 bit )
                                        options1 |= (MainActivity.autofire << 2); // bits  3.. 2 (2 bits)
        if (MainActivity.useff        ) options1 |= (1 <<  4);
        if (MainActivity.ispal        ) options1 |= (1 <<  5);
        if (MainActivity.sound        ) options1 |= (1 <<  6);
        if (MainActivity.retune       ) options1 |= (1 <<  7);
        if (MainActivity.collisions   ) options1 |= (1 <<  8);
                                                 // (1 <<  9); (spare)
        if (MainActivity.lives        ) options1 |= (1 << 10);
        if (MainActivity.time         ) options1 |= (1 << 11);
        if (MainActivity.invincibility) options1 |= (1 << 12);
                                        options1 |= (MainActivity.layout      << 13); // bits 15..13 (3 bits)
                                        options1 |= (MainActivity.volume      << 16); // bits 19..16 (4 bits)
                                        options1 |= (MainActivity.speed       << 20); // bits 23..20 (4 bits)
        if (MainActivity.skies        ) options1 |= (1                        << 24); // bit      24 (1 bit )
        if (MainActivity.spring       ) options1 |= (1                        << 25); // bit      25 (1 bit )
        if (MainActivity.swapped      ) options1 |= (1                        << 26); // bit      26 (1 bit )
        if (MainActivity.fill         ) options1 |= (1                        << 27); // bit      27 (1 bit )
        if (MainActivity.dejitter     ) options1 |= (1                        << 28); // bit      28 (1 bit )
                                        options1 |= (MainActivity.colours     << 29); // bits 30..29 (2 bits)
        if (MainActivity.lock         ) options1 |= (1                        << 31); // bit      31 (1 bit )

        options2 = 0;
                                        options2 |= (MainActivity.machine          ); // bits  0.. 2 (3 bits)
                                        options2 |= (MainActivity.memmap      <<  3); // bits  6.. 3 (4 bits)
        if (MainActivity.autocoin     ) options2 |= (1                        <<  7); // bit       7 (1 bit )
        if (MainActivity.darkenbg     ) options2 |= (1                        <<  8); // bit       8 (1 bit )
        if (MainActivity.flagline     ) options2 |= (1                        <<  9); // bit       9 (1 bit )
        if (MainActivity.sameplayer   ) options2 |= (1                        << 10); // bit      10 (1 bit )
                                        options2 |= (MainActivity.sensitivity << 11); // bits 13..11 (3 bits)
                                        options2 |= (MainActivity.demultiplex << 14); // bits 15..14 (2 bits)

        csetoptions(options1, options2);
        jgetoptions(); // read them back in case C code has changed them (eg. region)
    
        int rc = getfirebuttons();
        int wtf;
        wtf = 0x000F; MainActivity.key1 = MainActivity.keytable_to_activity[ rc & wtf       ];
        wtf = 0x00F0; MainActivity.key2 = MainActivity.keytable_to_activity[(rc & wtf) >>  4];
        wtf = 0x0F00; MainActivity.key3 = MainActivity.keytable_to_activity[(rc & wtf) >>  8];
        wtf = 0xF000; MainActivity.key4 = MainActivity.keytable_to_activity[(rc & wtf) >> 12];

        MainActivity.redrawkeypad      = true;
        MainActivity.resizecontrollers = true;
        MainActivity.semipaused        = false;
        redrawscreen();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {   super.onConfigurationChanged(newConfig);
        this.recreate();
    }
    
    @Override
    protected void onResume()
    {   super.onResume();
    
        update_trainers();
    }

    public void jloadcos(int whichtype)
    {   String filename;
    
        if (whichtype == 0) filename = "Autosave.cos"; else filename = "Quicksave.cos";
        try (FileInputStream fis = openFileInput(filename);)
        {   long   filesize = fis.available();
            byte[] allBytes = new byte[(int) filesize];

            if (fis.read(allBytes, 0, (int) filesize) != filesize)
            {   fis.close();
                Toast.makeText(OptionsActivity.this, getResources().getString(R.string.cantread) + " " + filename + "!", Toast.LENGTH_LONG).show();
            } else
            {   fis.close();
                int rc = cloadcos(whichtype, (int) filesize, allBytes, (whichtype == 0) ? false : true);
                if (rc != 0)
                {   Toast.makeText
                    (    OptionsActivity.this,
                         getResources().getString(R.string.cantparse)
                       + " "
                       + filename
                       + ", "
                       + getResources().getString(R.string.error)
                       + " "
                       + rc
                       + "!",
                         Toast.LENGTH_LONG
                    ).show();
        }   }   }
        catch (IOException e)
        {   if (whichtype == 1)
            {   Toast.makeText(OptionsActivity.this, filename + " " + getResources().getString(R.string.doesntexist) + "!", Toast.LENGTH_LONG).show();
        }   }

        MainActivity.whichgame = getgame();
        if (MainActivity.autofire >= 1)
        {   MainActivity.softkeys[MainActivity.key1] = false;
            if (MainActivity.whichgame == 173) // means 3D Attack
            {   MainActivity.softkeys[MainActivity.key2] = false;
        }   }
        jgetoptions();
        int rc = getfirebuttons();
        int wtf;
        wtf = 0x000F; MainActivity.key1 = MainActivity.keytable_to_activity[ rc & wtf       ];
        wtf = 0x00F0; MainActivity.key2 = MainActivity.keytable_to_activity[(rc & wtf) >>  4];
        wtf = 0x0F00; MainActivity.key3 = MainActivity.keytable_to_activity[(rc & wtf) >>  8];
        wtf = 0xF000; MainActivity.key4 = MainActivity.keytable_to_activity[(rc & wtf) >> 12];
        MainActivity.topleft     = getdigipos(0);
        MainActivity.centred     = getdigipos(1);
        MainActivity.ox = MainActivity.oy = MainActivity.centred * 256;
        MainActivity.bottomright = getdigipos(2);
        // MainActivity.sensitivity = getsensitivity(); is not needed
        MainActivity.redrawkeypad  = true;
        MainActivity.progresswidth =
        MainActivity.trackerwidth  = 0;
        MainActivity.hardcore = jsetravars(MainActivity.username, MainActivity.password, MainActivity.cheevos, MainActivity.hardcore);
        redrawscreen();
    }
    
    public void jsavecos(int whichtype)
    {   String filename;
        byte   thedata[] = csavecos();
        int    filesize  = thedata.length;

        if (whichtype == 0) filename = "Autosave.cos"; else filename = "Quicksave.cos";
        try (FileOutputStream fos = openFileOutput(filename, MODE_PRIVATE);)
        {   fos.write(thedata, 0, filesize);
            // fos.close(); is redundant
        }
        catch (IOException e)
        {   Toast.makeText(OptionsActivity.this, getResources().getString(R.string.cantsave) + " " + filename + "!", Toast.LENGTH_LONG).show();
    }   }
    
    private void jsoundoff()
    {   int i;

        for (i = 0; i < MainActivity.GUESTCHANNELS; i++)
        {   if (MainActivity.playing[i])
            {   MainActivity.audio[i].stop();
                MainActivity.playing[i] = false;
    }   }   }
    
    private void writegadget_colours()
    {   if (MainActivity.colours == MainActivity.COLOURS_PURE)
        {   button_colours.setBackgroundColor(Color.parseColor("#ffaaaa"));
        } else
        {   button_colours.setBackgroundColor(Color.parseColor("#ffcccc"));
        }
        if      (MainActivity.colours == MainActivity.COLOURS_PURE     ) button_colours.setText(R.string.colours_pure);
        else if (MainActivity.colours == MainActivity.COLOURS_PVI      ) button_colours.setText(R.string.colours_pvi);
        else if (MainActivity.colours == MainActivity.COLOURS_UVI      ) button_colours.setText(R.string.colours_uvi);
        else if (MainActivity.colours == MainActivity.COLOURS_GREYSCALE) button_colours.setText(R.string.colours_greyscale);
    }
    
    private void writegadget_artefacts()
    {   if (MainActivity.artefacts)
        {   switch_artefacts.setBackgroundColor(Color.parseColor("#ffcccc"));
        } else
        {   switch_artefacts.setBackgroundColor(Color.parseColor("#ffaaaa"));
        }
        switch_artefacts.setChecked(MainActivity.artefacts);
    }
    
    private void writegadget_darkenbg()
    {   switch_darkenbg.setChecked(MainActivity.darkenbg);
        if (MainActivity.machine == 1 || MainActivity.machine == 2) // INTERTON, ELEKTOR
        {   switch_darkenbg.setEnabled(true);
            if (MainActivity.darkenbg)
            {   switch_darkenbg.setBackgroundColor(Color.parseColor("#ffcccc"));
            } else
            {   switch_darkenbg.setBackgroundColor(Color.parseColor("#ffaaaa"));
        }   }
        else
        {   switch_darkenbg.setEnabled(false);
            switch_darkenbg.setBackgroundColor(Color.parseColor("#555566"));
    }   }
       
    private void writegadget_linebased()
    {   switch_linebased.setChecked(MainActivity.linebased);
        if (MainActivity.linebased)
        {   switch_linebased.setBackgroundColor(Color.parseColor("#aaffaa"));
            switch_linebased.setText(R.string.linebased);
        } else
        {   switch_linebased.setBackgroundColor(Color.parseColor("#88ff88"));
            switch_linebased.setText(R.string.pixelbased);
    }   }
    
    private void writegadget_flagline()
    {   switch_flagline.setChecked(MainActivity.flagline);
        if (MainActivity.machine == 0) // ARCADIA
        {   switch_flagline.setEnabled(true);
            if (MainActivity.flagline)
            {   switch_flagline.setBackgroundColor(Color.parseColor("#ffcccc"));
            } else
            {   switch_flagline.setBackgroundColor(Color.parseColor("#ffaaaa"));
        }   }
        else
        {   switch_flagline.setEnabled(false);
            switch_flagline.setBackgroundColor(Color.parseColor("#555566"));
    }   }
        
    private void writegadget_stretch()
    {   if (MainActivity.stretch == 0)
        {   label_stretch.setBackgroundColor(Color.parseColor("#6655ff"));
            seekbar_stretch.setBackgroundColor(Color.parseColor("#6655ff"));
        } else
        {   label_stretch.setBackgroundColor(Color.parseColor("#9988ff"));
            seekbar_stretch.setBackgroundColor(Color.parseColor("#9988ff"));
        }
        if      (MainActivity.stretch == 0) label_stretch.setText(R.string.stretching_off);
        else if (MainActivity.stretch == 1) label_stretch.setText(R.string.stretching_small);
        else if (MainActivity.stretch == 2) label_stretch.setText(R.string.stretching_large);
        else if (MainActivity.stretch == 3) label_stretch.setText(R.string.stretching_maximum);
        seekbar_stretch.setProgress(MainActivity.stretch);
    }
        
    private void writegadget_autofire()
    {   if (MainActivity.autofire == 0)
        {   button_autofire.setBackgroundColor(Color.parseColor("#cc6600"));
            button_autofire.setCompoundDrawablesWithIntrinsicBounds(R.drawable.autofireoff, 0, 0, 0);
        } else
        {   button_autofire.setBackgroundColor(Color.parseColor("#ffaa00"));
            button_autofire.setCompoundDrawablesWithIntrinsicBounds(R.drawable.autofireon, 0, 0, 0);
        }
        if      (MainActivity.autofire == 0) button_autofire.setText(R.string.autofire_off);
        else if (MainActivity.autofire == 1) button_autofire.setText(R.string.autofire_pressed);
        else if (MainActivity.autofire == 2) button_autofire.setText(R.string.autofire_always);
    }

    private void writegadget_swapped()
    {   if (MainActivity.swapped)
        {   button_swapped.setBackgroundColor(Color.parseColor("#ffaa00"));
            button_swapped.setText(R.string.controller_right);
        } else
        {   button_swapped.setBackgroundColor(Color.parseColor("#cc6600"));
            button_swapped.setText(R.string.controller_left);
    }   }
    
    private void writegadget_handedness()
    {   if (MainActivity.handedness)
        {   button_handedness.setBackgroundColor(Color.parseColor("#ffaa00"));
            button_handedness.setText(R.string.layout_keypadpaddle);
        } else
        {   button_handedness.setBackgroundColor(Color.parseColor("#cc6600"));
            button_handedness.setText(R.string.layout_paddlekeypad);
    }   }

    private void writegadget_size()
    {   if (MainActivity.sizemethod == MainActivity.SIZE_OFF)
        {   label_size.setBackgroundColor(Color.parseColor("#cc6600"));
            seekbar_size.setBackgroundColor(Color.parseColor("#cc6600"));
        } else
        {   label_size.setBackgroundColor(Color.parseColor("#ffaa00"));
            seekbar_size.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        if      (MainActivity.sizemethod == MainActivity.SIZE_OFF  ) label_size.setText(R.string.size_off);
        else if (MainActivity.sizemethod == MainActivity.SIZE_28MM ) label_size.setText(R.string.size_28mm);
        else if (MainActivity.sizemethod == MainActivity.SIZE_160DP) label_size.setText(R.string.size_160dp);
        else if (MainActivity.sizemethod == MainActivity.SIZE_240PX) label_size.setText(R.string.size_240px);
        else if (MainActivity.sizemethod == MainActivity.SIZE_MAX  ) label_size.setText(R.string.size_maximum);
        seekbar_size.setProgress(MainActivity.sizemethod);
    }

    private void writegadget_paddle()
    {   if (MainActivity.layout == MainActivity.WAYS_8)
        {   label_paddle.setBackgroundColor(Color.parseColor("#cc6600"));
            seekbar_paddle.setBackgroundColor(Color.parseColor("#cc6600"));
        } else
        {   label_paddle.setBackgroundColor(Color.parseColor("#ffaa00"));
            seekbar_paddle.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        if      (MainActivity.layout == MainActivity.WAYS_0     ) label_paddle.setText(R.string.paddle_0);
        else if (MainActivity.layout == MainActivity.WAYS_2H    ) label_paddle.setText(R.string.paddle_2h);
        else if (MainActivity.layout == MainActivity.WAYS_2V    ) label_paddle.setText(R.string.paddle_2v);
        else if (MainActivity.layout == MainActivity.WAYS_4O    ) label_paddle.setText(R.string.paddle_4o);
        else if (MainActivity.layout == MainActivity.WAYS_4D    ) label_paddle.setText(R.string.paddle_4d);
        else if (MainActivity.layout == MainActivity.WAYS_8     ) label_paddle.setText(R.string.paddle_8);
        else if (MainActivity.layout == MainActivity.WAYS_ANALOG) label_paddle.setText(R.string.paddle_analog);
        seekbar_paddle.setProgress(MainActivity.layout);
    }

    private void writegadget_machine()
    {   if      (MainActivity.machine ==  0) button_machine.setText("Machine: Emerson Arcadia 2001");
        else if (MainActivity.machine ==  1) button_machine.setText("Machine: Interton VC 4000");
        else if (MainActivity.machine ==  2) button_machine.setText("Machine: Elektor TVGC");
        else if (MainActivity.machine ==  3)
        {   if      (MainActivity.memmap  ==  9) button_machine.setText("Machine: Astro Wars");
            else if (MainActivity.memmap  == 10) button_machine.setText("Machine: Galaxia");
            else if (MainActivity.memmap  == 11) button_machine.setText("Machine: Laser Battle");
            else if (MainActivity.memmap  == 12) button_machine.setText("Machine: Lazarian");
            else                                 button_machine.setText(String.format("Machine: Unknown Zaccaria %d!", MainActivity.memmap)); // should never happen
        } else if (MainActivity.machine == 4)
        {   if      (MainActivity.memmap  == 13) button_machine.setText("Machine: Malzak 1");
            else if (MainActivity.memmap  == 14) button_machine.setText("Machine: Malzak 2");
            else                                 button_machine.setText(String.format("Unknown Kitronix %d!", MainActivity.memmap)); // should never happen
        } else
        {                                        button_machine.setText(String.format("Unknown machine %d!", MainActivity.machine)); // should never happen
    }   }
    
    private void writegadget_speed()
    {   if (MainActivity.cheevos && MainActivity.hardcore && MainActivity.speed < MainActivity.SPEED_4QUARTERS)
        {   MainActivity.speed = MainActivity.SPEED_4QUARTERS;
        }

        if (MainActivity.speed == MainActivity.SPEED_4QUARTERS)
        {   label_speed.setBackgroundColor(Color.parseColor("#00dddd"));
            seekbar_speed.setBackgroundColor(Color.parseColor("#00dddd"));
        } else
        {   label_speed.setBackgroundColor(Color.parseColor("#00ffff"));
            seekbar_speed.setBackgroundColor(Color.parseColor("#00ffff"));
        }
        if      (MainActivity.speed == MainActivity.SPEED_1QUARTER  ) label_speed.setText(String.format("%s: 25%%",       getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_2QUARTERS ) label_speed.setText(String.format("%s: 50%%",       getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_3QUARTERS ) label_speed.setText(String.format("%s: 75%%",       getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_4QUARTERS ) label_speed.setText(String.format("%s: 100%% (%s)", getResources().getString(R.string.speed), getResources().getString(R.string.normal)));
        else if (MainActivity.speed == MainActivity.SPEED_5QUARTERS ) label_speed.setText(String.format("%s: 125%%",      getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_6QUARTERS ) label_speed.setText(String.format("%s: 150%%",      getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_8QUARTERS ) label_speed.setText(String.format("%s: 200%%",      getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_16QUARTERS) label_speed.setText(String.format("%s: 400%%",      getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_32QUARTERS) label_speed.setText(String.format("%s: 800%%",      getResources().getString(R.string.speed)));
        else if (MainActivity.speed == MainActivity.SPEED_TURBO     ) label_speed.setText(String.format("%s: %s",         getResources().getString(R.string.speed), getResources().getString(R.string.turbo )));
        seekbar_speed.setProgress(MainActivity.speed);
    }

    private void writegadget_indicator()
    {   if (MainActivity.indicator == 0)
        {   button_indicator.setBackgroundColor(Color.parseColor("#00dddd"));
            button_indicator.setText(getResources().getString(R.string.indicator) + ": " + getResources().getString(R.string.off2));
        } else if (MainActivity.indicator == MainActivity.INDICATOR_FPS)
        {   button_indicator.setBackgroundColor(Color.parseColor("#00ffff"));
            button_indicator.setText(getResources().getString(R.string.indicator) + ": " + getResources().getString(R.string.fps));
        } else if (MainActivity.indicator == MainActivity.INDICATOR_KHZ)
        {   button_indicator.setBackgroundColor(Color.parseColor("#00ffff"));
            button_indicator.setText(getResources().getString(R.string.indicator) + ": " + getResources().getString(R.string.khz));
        } else if (MainActivity.indicator == MainActivity.INDICATOR_PERCENT)
        {   button_indicator.setBackgroundColor(Color.parseColor("#00ffff"));
            button_indicator.setText(getResources().getString(R.string.indicator) + ": " + getResources().getString(R.string.percentage));
    }   }

    private void writegadget_region()
    {   if (MainActivity.ispal)
        {   if (MainActivity.machine == 0) // ARCADIA
            {   button_region.setBackgroundColor(Color.parseColor("#00ffff"));
            } else
            {   button_region.setBackgroundColor(Color.parseColor("#555566"));
            }                
            button_region.setText(R.string.region_pal);
        } else
        {   button_region.setBackgroundColor(Color.parseColor("#00dddd"));
            button_region.setText(R.string.region_ntsc);
    }   }

    private void writegadget_frameskip()
    {   if (MainActivity.frameskip == 1)
        {   label_frameskip.setText(                                String.format("%s: 1/1 (%s)", getResources().getString(R.string.frameskipping), getResources().getString(R.string.off   )));
            label_frameskip.setBackgroundColor(Color.parseColor("#00dddd"));
            seekbar_frameskip.setBackgroundColor(Color.parseColor("#00dddd"));
        } else
        {   label_frameskip.setText(                                String.format("%s: 1/%d",     getResources().getString(R.string.frameskipping), MainActivity.frameskip));
            label_frameskip.setBackgroundColor(Color.parseColor("#00ffff"));
            seekbar_frameskip.setBackgroundColor(Color.parseColor("#00ffff"));
        }
        seekbar_frameskip.setProgress(MainActivity.frameskip);
    }
        
    private void writegadget_volume()
    {   if (MainActivity.volume == 0)
        {   label_volume.setText(    String.format("%s: 0/8 (%s)", getResources().getString(R.string.volume       ), getResources().getString(R.string.silent)));
            label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume0, 0, 0, 0);
            label_volume.setBackgroundColor(Color.parseColor("#cccc00"));
            seekbar_volume.setBackgroundColor(Color.parseColor("#cccc00"));
        } else
        {   if (MainActivity.volume == 1)
            {   label_volume.setText(String.format("%s: 1/8 (%s)", getResources().getString(R.string.volume       ), getResources().getString(R.string.quiet )));
                label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume1, 0, 0, 0);
            } else if (MainActivity.volume == 8)
            {   label_volume.setText(String.format("%s: 8/8 (%s)", getResources().getString(R.string.volume       ), getResources().getString(R.string.loud  )));
                label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume8, 0, 0, 0);
            } else
            {   label_volume.setText(String.format("%s: %d/8",     getResources().getString(R.string.volume       ), MainActivity.volume));
                if (MainActivity.volume == 2) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume2, 0, 0, 0);
                if (MainActivity.volume == 3) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume3, 0, 0, 0);
                if (MainActivity.volume == 4) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume4, 0, 0, 0);
                if (MainActivity.volume == 5) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume5, 0, 0, 0);
                if (MainActivity.volume == 6) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume6, 0, 0, 0);
                if (MainActivity.volume == 7) label_volume.setCompoundDrawablesWithIntrinsicBounds(R.drawable.volume7, 0, 0, 0);
            }
            label_volume.setBackgroundColor(Color.parseColor("#ffff00"));
            seekbar_volume.setBackgroundColor(Color.parseColor("#ffff00"));
        }

        seekbar_volume.setProgress(MainActivity.volume);
    }

    private void writegadget_sensitivity()
    {   if (MainActivity.sensitivity == 1)
        {   label_sensitivity.setText(getResources().getString(R.string.sensitivity_vlow));
        } else if (MainActivity.sensitivity == 2)
        {   label_sensitivity.setText(getResources().getString(R.string.sensitivity_low));
        } else if (MainActivity.sensitivity == 3)
        {   label_sensitivity.setText(getResources().getString(R.string.sensitivity_medium));
        } else if (MainActivity.sensitivity == 4)
        {   label_sensitivity.setText(getResources().getString(R.string.sensitivity_high));
        } else if (MainActivity.sensitivity == 5)
        {   label_sensitivity.setText(getResources().getString(R.string.sensitivity_vhigh));
        }
        if (MainActivity.sensitivity == 3)
        {   label_sensitivity.setBackgroundColor(Color.parseColor("#cc6600"));
            seekbar_sensitivity.setBackgroundColor(Color.parseColor("#cc6600"));
        } else
        {   label_sensitivity.setBackgroundColor(Color.parseColor("#ffaa00"));
            seekbar_sensitivity.setBackgroundColor(Color.parseColor("#ffaa00"));
        }
        seekbar_sensitivity.setProgress(MainActivity.sensitivity);
    }

    private void writegadget_demultiplex()
    {   if (MainActivity.demultiplex == 0)
        {   button_demultiplex.setBackgroundColor(Color.parseColor("#cc00cc"));
            button_demultiplex.setText(getResources().getString(R.string.multiplex));
        } else
        {   button_demultiplex.setBackgroundColor(Color.parseColor("#ff00ff"));
            if (MainActivity.demultiplex == 1)
            {   button_demultiplex.setText(getResources().getString(R.string.transparent));
            } else if (MainActivity.demultiplex == 2)
            {   button_demultiplex.setText(getResources().getString(R.string.opaque));
    }   }   }

    private void update_trainers()
    {   if (MainActivity.cheevos && MainActivity.hardcore)
        {   // This shouldn't be necessary (RAActivity handles it) but for some reason it seems to be :-(
            MainActivity.collisions    = true;
            MainActivity.lives         =
            MainActivity.time          =
            MainActivity.invincibility = false;
            
            button_quickload.setEnabled(    false);
            switch_collisions.setEnabled(   false);
            switch_lives.setEnabled(        false);
            switch_time.setEnabled(         false);
            switch_invincibility.setEnabled(false);
            button_levelskip.setEnabled(    false);

                                            button_quickload.setBackgroundColor(    Color.parseColor("#555566"));
                                            switch_collisions.setBackgroundColor(   Color.parseColor("#555566"));
                                            switch_lives.setBackgroundColor(        Color.parseColor("#555566"));
                                            switch_time.setBackgroundColor(         Color.parseColor("#555566"));
                                            switch_invincibility.setBackgroundColor(Color.parseColor("#555566"));
                                            button_levelskip.setBackgroundColor(    Color.parseColor("#555566"));

            writegadget_speed();
        } else
        {   button_quickload.setEnabled(    true);
            switch_collisions.setEnabled(   true);
            switch_lives.setEnabled(        true);
            switch_time.setEnabled(         true);
            switch_invincibility.setEnabled(true);
            button_levelskip.setEnabled(    true);

                                            button_quickload.setBackgroundColor(    Color.parseColor("#ddcccc"));
            if (MainActivity.collisions   ) switch_collisions.setBackgroundColor(   Color.parseColor("#ff00ff")); else switch_collisions.setBackgroundColor(   Color.parseColor("#cc00cc"));
            if (MainActivity.lives        ) switch_lives.setBackgroundColor(        Color.parseColor("#ff0000")); else switch_lives.setBackgroundColor(        Color.parseColor("#cc0000"));
            if (MainActivity.time         ) switch_time.setBackgroundColor(         Color.parseColor("#ff0000")); else switch_time.setBackgroundColor(         Color.parseColor("#cc0000"));
            if (MainActivity.invincibility) switch_invincibility.setBackgroundColor(Color.parseColor("#ff0000")); else switch_invincibility.setBackgroundColor(Color.parseColor("#cc0000"));
                                            button_levelskip.setBackgroundColor(    Color.parseColor("#ff0000"));
        }

        switch_collisions.setCompoundDrawablesWithIntrinsicBounds(MainActivity.collisions ? R.drawable.collisionson : R.drawable.collisionsoff, 0, 0, 0);

        switch_collisions.setChecked(   MainActivity.collisions);
        switch_lives.setChecked(        MainActivity.lives);
        switch_time.setChecked(         MainActivity.time);
        switch_invincibility.setChecked(MainActivity.invincibility);
    }

    private void jgetoptions()
    {   int options,
            wtf;

        options = cgetoptions1();
        wtf =   1       ; if ((options & wtf) != 0) MainActivity.linebased     = true; else MainActivity.linebased     = false;
        wtf = ( 1 <<  1); if ((options & wtf) != 0) MainActivity.artefacts     = true; else MainActivity.artefacts     = false;
        wtf = ( 3 <<  2);                           MainActivity.autofire      = (options & wtf) >>  2; // bits  3.. 2 (2 bits)
        wtf = ( 1 <<  4); if ((options & wtf) != 0) MainActivity.useff         = true; else MainActivity.useff         = false;
        wtf = ( 1 <<  5); if ((options & wtf) != 0) MainActivity.ispal         = true; else MainActivity.ispal         = false;
        wtf = ( 1 <<  6); if ((options & wtf) != 0) MainActivity.sound         = true; else MainActivity.sound         = false;
        wtf = ( 1 <<  7); if ((options & wtf) != 0) MainActivity.retune        = true; else MainActivity.retune        = false;
        wtf = ( 1 <<  8); if ((options & wtf) != 0) MainActivity.collisions    = true; else MainActivity.collisions    = false;
     // wtf = ( 1 <<  9); (spare)
        wtf = ( 1 << 10); if ((options & wtf) != 0) MainActivity.lives         = true; else MainActivity.lives         = false;
        wtf = ( 1 << 11); if ((options & wtf) != 0) MainActivity.time          = true; else MainActivity.time          = false;
        wtf = ( 1 << 12); if ((options & wtf) != 0) MainActivity.invincibility = true; else MainActivity.invincibility = false;
        wtf = ( 7 << 13);                           MainActivity.layout        = (options & wtf) >> 13; // bits 15..13 (3 bits)
        wtf = (15 << 16);                           MainActivity.volume        = (options & wtf) >> 16; // bits 19..16 (4 bits)
        wtf = (15 << 20);                           MainActivity.speed         = (options & wtf) >> 20; // bits 23..20 (4 bits)
        wtf = ( 1 << 24); if ((options & wtf) != 0) MainActivity.skies         = true; else MainActivity.skies         = false; // bit 24
        wtf = ( 1 << 25); if ((options & wtf) != 0) MainActivity.spring        = true; else MainActivity.spring        = false; // bit 25
        wtf = ( 1 << 26); if ((options & wtf) != 0) MainActivity.swapped       = true; else MainActivity.swapped       = false; // bit 26
        wtf = ( 1 << 27); if ((options & wtf) != 0) MainActivity.fill          = true; else MainActivity.fill          = false; // bit 27
        wtf = ( 1 << 28); if ((options & wtf) != 0) MainActivity.dejitter      = true; else MainActivity.dejitter      = false; // bit 28
        wtf = ( 3 << 29);                           MainActivity.colours       = (options & wtf) >> 29; // bits 30..29 (2 bits)
        wtf = ( 1 << 31); if ((options & wtf) != 0) MainActivity.lock          = true; else MainActivity.lock          = false; // bit 31

        options = cgetoptions2();
        wtf =   7       ;                           MainActivity.machine       = (options & wtf)      ; // bits  2.. 0 (3 bits)
        wtf = (15 <<  3);                           MainActivity.memmap        = (options & wtf) >>  3; // bits  6.. 3 (4 bits)
        wtf = ( 1 <<  7); if ((options & wtf) != 0) MainActivity.autocoin      = true; else MainActivity.autocoin      = false; // bit  7 (1 bit)
        wtf = ( 1 <<  8); if ((options & wtf) != 0) MainActivity.darkenbg      = true; else MainActivity.darkenbg      = false; // bit  8 (1 bit)
        wtf = ( 1 <<  9); if ((options & wtf) != 0) MainActivity.flagline      = true; else MainActivity.flagline      = false; // bit  9 (1 bit)
        wtf = ( 1 << 10); if ((options & wtf) != 0) MainActivity.sameplayer    = true; else MainActivity.sameplayer    = false; // bit 10 (1 bit)
        wtf = ( 7 << 11);                           MainActivity.sensitivity   = (options & wtf) >> 11; // bits 13..11 (3 bits)
        wtf = ( 3 << 14);                           MainActivity.demultiplex   = (options & wtf) >> 14; // bits 15..14 (2 bits)
}   }
