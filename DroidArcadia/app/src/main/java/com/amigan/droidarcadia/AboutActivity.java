package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.net.Uri;

import java.time.LocalDate;
import java.time.Month;
import java.time.format.DateTimeFormatter;
import java.time.format.FormatStyle;

public class AboutActivity extends Activity
{   @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about);

        if (MainActivity.lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

        TextView about_nameandversion = findViewById(R.id.about_nameandversion);
        TextView about_releasedate    = findViewById(R.id.about_releasedate);
        TextView about_runningon      = findViewById(R.id.about_runningon);
        TextView about_cpu            = findViewById(R.id.about_cpu);
        TextView link1_button         = findViewById(R.id.link1_button);
        TextView link2_button         = findViewById(R.id.link2_button);
        
        about_nameandversion.setText("DroidArcadia 4.42 beta 2");
        /* Toast.makeText(AboutActivity.this, "Versions are " + Build.VERSION.SDK_INT + "," + Build.VERSION_CODES.O, Toast.LENGTH_LONG).show(); // "25,26" for Nox
        if
        (   !Build.HARDWARE.toLowerCase().contains("nox")
         && !Build.PRODUCT.toLowerCase().contains("nox")
         && !Build.BOARD.toLowerCase().contains("nox")
         && Build.VERSION.SDK_INT >= Build.VERSION_CODES.O // ie. less than Android 8.0 (less than API 26)
        ) // Nox returns VERSION.SDK_INT of 25 but thinks 25 >= 26!?
        { */
            LocalDate releasedate = LocalDate.of(2026, Month.JANUARY, 21);
            DateTimeFormatter dateFormatter = DateTimeFormatter.ofLocalizedDate(FormatStyle.LONG);
            String formatteddate = releasedate.format(dateFormatter);
            about_releasedate.setText(formatteddate);
        // }

        int longest = 0;
        link1_button.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);
        link2_button.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);
        if (link1_button.getMeasuredWidth() > longest) longest = link1_button.getMeasuredWidth();
        if (link2_button.getMeasuredWidth() > longest) longest = link2_button.getMeasuredWidth();

        link1_button.setLayoutParams(new LinearLayout.LayoutParams(longest, link1_button.getMeasuredHeight()));
        link2_button.setLayoutParams(new LinearLayout.LayoutParams(longest, link2_button.getMeasuredHeight()));
        if (android.os.Process.is64Bit())
        {   about_runningon.setText(getResources().getString(R.string.runningon) + ": Android " + Build.VERSION.RELEASE + " (64-bit)");
        } else
        {   about_runningon.setText(getResources().getString(R.string.runningon) + ": Android " + Build.VERSION.RELEASE + " (32-bit)");
        }    
        about_cpu.setText(getResources().getString(R.string.architecture) + ": " + Build.SUPPORTED_ABIS[0]);

        link1_button.setOnClickListener((view) ->
        {   Intent link1_intent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://amigan.1emu.net/releases/"));
            startActivity(link1_intent);
        });

        link2_button.setOnClickListener((view) ->
        {   Intent link2_intent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://amigan.yatho.com"));
            startActivity(link2_intent);
        });
}   }
