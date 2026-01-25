package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.TypedValue;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.net.HttpURLConnection;
import java.net.URL;

public class RAActivity extends Activity
{   public native String  getgamegfxurl();
    public native int     getgameid();
    public native boolean jsetravars(String jusername, String jpassword, boolean jcheevos, boolean jhardcore);
    public native void    showachievements();

    private int        gameid;
    private ImageView  image_game;

    private static int          nextslot;
    private static LinearLayout layout_ra;

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_raoptions);
        if (MainActivity.lock) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

                  layout_ra        = findViewById(R.id.layout_ra);
        Switch    switch_cheevos   = findViewById(R.id.switch_cheevos);
        Switch    switch_hardcore  = findViewById(R.id.switch_hardcore);
        EditText  string_username  = findViewById(R.id.username);
        EditText  string_password  = findViewById(R.id.password);
        TextView  button_back      = findViewById(R.id.button_back);
                  image_game       = findViewById(R.id.image_game);
        TextView  button_userlink  = findViewById(R.id.button_userlink);
        TextView  button_gamelink  = findViewById(R.id.button_gamelink);
                  
        if (MainActivity.cheevos)
        {   switch_cheevos.setBackgroundColor(Color.parseColor("#ddcccc"));
            switch_cheevos.setText(R.string.ra_on);
        } else
        {   switch_cheevos.setText(R.string.ra_off);
        }
        switch_cheevos.setChecked(MainActivity.cheevos);

        if (MainActivity.hardcore)
        {   switch_hardcore.setBackgroundColor(Color.parseColor("#ddcccc"));
            switch_hardcore.setText(R.string.hardcore_on);
        } else
        {   switch_hardcore.setText(R.string.hardcore_off);
        }
        switch_hardcore.setChecked(MainActivity.hardcore);

        string_username.setText(MainActivity.username);

        string_password.setText(MainActivity.password);

        String imageUrl = getgamegfxurl();
        if (!imageUrl.isEmpty())
        {   new DownloadImageTask1(this).execute(imageUrl);
        }
        
        if (!MainActivity.username.isEmpty())
        {   button_userlink.setText("https://retroachievements.org/user/" + MainActivity.username);
        }
        
        gameid = getgameid();
        if (gameid != 0)
        {   button_gamelink.setText("https://retroachievements.org/game/" + gameid);
        }

        nextslot = 6;
        showachievements();
        if (nextslot == 6)
        {   addcheevo("", "(No Achievements)", "", "");
        }

        switch_cheevos.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.cheevos = isChecked;
            if (isChecked)
            {   switch_cheevos.setBackgroundColor(Color.parseColor("#ddcccc"));
                switch_cheevos.setText(R.string.ra_on);
            } else
            {   switch_cheevos.setBackgroundColor(Color.parseColor("#aaaaaa"));
                switch_cheevos.setText(R.string.ra_off);
            }
        });

        switch_hardcore.setOnCheckedChangeListener((buttonView, isChecked) ->
        {   MainActivity.hardcore = isChecked;
            if (isChecked)
            {   switch_hardcore.setBackgroundColor(Color.parseColor("#ddcccc"));
                switch_hardcore.setText(R.string.hardcore_on);
            } else
            {   switch_hardcore.setBackgroundColor(Color.parseColor("#aaaaaa"));
                switch_hardcore.setText(R.string.hardcore_off);
            }
        });

        string_username.setOnEditorActionListener((v, actionId, event) ->
        {   if (actionId == EditorInfo.IME_ACTION_NEXT)
            {   string_password.requestFocus();
                return true;  // Indicate that the action was handled
            }
            return false;
        });

        string_username.addTextChangedListener(new TextWatcher()
        {   @Override
            public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) { }

            @Override
            public void onTextChanged(CharSequence charSequence, int start, int before, int after)
            {   MainActivity.username = string_username.getText().toString();
                if (MainActivity.username.isEmpty())
                {   button_userlink.setText("-");
                } else
                {   button_userlink.setText("https://retroachievements.org/user/" + MainActivity.username);
            }   }

            @Override
            public void afterTextChanged(Editable editable) { }
        });

        string_password.setOnEditorActionListener((v, actionId, event) ->
        {   if (actionId == EditorInfo.IME_ACTION_DONE)
            {   InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                return true;  // Indicate that the action was handled
            }
            return false;  // If actionId is not IME_ACTION_DONE, return false to let the system handle it
        });

        string_password.addTextChangedListener(new TextWatcher()
        {   @Override
            public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) { }
            @Override
            public void onTextChanged(CharSequence charSequence, int start, int before, int after)
            {   MainActivity.password = string_password.getText().toString();
            }
            @Override
            public void afterTextChanged(Editable editable) { }
        });

        button_userlink.setOnClickListener((view) ->
        {   if (!MainActivity.username.isEmpty())
            {   Intent userlink_intent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://retroachievements.org/user/" + MainActivity.username));
                startActivity(userlink_intent);
            }
        });
        
        button_gamelink.setOnClickListener((view) ->
        {   if (gameid != 0)
            {   Intent gamelink_intent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://retroachievements.org/game/" + gameid));
                startActivity(gamelink_intent);
            }
        });
                
        button_back.setOnClickListener((view) ->
        {   if (MainActivity.cheevos && (MainActivity.username.isEmpty() || MainActivity.password.isEmpty()))
            {   Toast.makeText(RAActivity.this, "Username or password cannot be empty!", Toast.LENGTH_LONG).show();
            } else
            {   finish();
            }
        });
    }

    @SuppressWarnings("deprecation")
    protected void onDestroy()
    {   if (MainActivity.cheevos && MainActivity.hardcore)
        {   MainActivity.collisions = true;
            MainActivity.lives = MainActivity.time = MainActivity.invincibility = false;
            if (MainActivity.speed < MainActivity.SPEED_4QUARTERS)
            {   MainActivity.speed = MainActivity.SPEED_4QUARTERS;
        }   }
        MainActivity.hardcore = jsetravars(MainActivity.username, MainActivity.password, MainActivity.cheevos, MainActivity.hardcore);

        super.onDestroy();
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onBackPressed()
    {   if (MainActivity.cheevos && (MainActivity.username.isEmpty() || MainActivity.password.isEmpty()))
        {   Toast.makeText(RAActivity.this, "Username or password cannot be empty!", Toast.LENGTH_LONG).show();
        } else
        {   super.onBackPressed();
    }   }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {   super.onConfigurationChanged(newConfig);
        this.recreate();
    }

    @SuppressWarnings("deprecation")
    private static class DownloadImageTask1 extends android.os.AsyncTask<String, Void, Bitmap>
    {   private final WeakReference<RAActivity> activityWeakRef;

        DownloadImageTask1(RAActivity activity)
        {   activityWeakRef = new WeakReference<>(activity);
        }

        @Override
        protected Bitmap doInBackground(String... urls)
        {   Bitmap bitmap = null;
            String imageUrl = urls[0];

            try
            {   HttpURLConnection connection = (HttpURLConnection) new URL(imageUrl).openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream inputStream = connection.getInputStream();
                bitmap = BitmapFactory.decodeStream(inputStream);
            } catch (Exception e)
            {   // e.printStackTrace();
            }

            return bitmap;
        }

        @Override
        protected void onPostExecute(Bitmap result)
        {   RAActivity activity = activityWeakRef.get();
            if (activity != null)
            {   activity.runOnUiThread(() -> activity.image_game.setImageBitmap(result));
    }   }   }

    @SuppressWarnings("deprecation")
    private static class DownloadImageTask2 extends android.os.AsyncTask<String, Void, Bitmap>
    {   private final WeakReference<ImageView> imageViewRef;

        DownloadImageTask2(ImageView imageView)
        {   imageViewRef = new WeakReference<>(imageView);
        }

        @Override
        protected Bitmap doInBackground(String... urls)
        {   Bitmap bitmap = null;
            try
            {   HttpURLConnection connection = (HttpURLConnection) new URL(urls[0]).openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream inputStream = connection.getInputStream();
                bitmap = BitmapFactory.decodeStream(inputStream);
            } catch (Exception e)
            {   ;
            }
            return bitmap;
        }

        @Override
        protected void onPostExecute(Bitmap result)
        {   ImageView imageView = imageViewRef.get();
            if (imageView != null && result != null)
            {   imageView.setImageBitmap(result);
    }   }   }

    @SuppressWarnings("deprecation")
    public static void addcheevo(String jurl, String jtitle, String jdesc, String jprog)
    {   if (layout_ra == null) return;
        Context context = layout_ra.getContext();

        int margin = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 5, context.getResources().getDisplayMetrics());

        LinearLayout rowLayout = new LinearLayout(context);
        rowLayout.setOrientation(LinearLayout.HORIZONTAL);
        LinearLayout.LayoutParams rowParams = new LinearLayout.LayoutParams
        (   LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        rowParams.setMargins(margin, margin, margin, margin);
        rowLayout.setLayoutParams(rowParams);

        ImageView imageView = new ImageView(context);
        if (jurl.isEmpty())
        {   imageView.setImageResource(0);
        } else
        {   new DownloadImageTask2(imageView).execute(jurl);
        }
        LinearLayout.LayoutParams imgParams = new LinearLayout.LayoutParams
        (   LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        imgParams.setMargins(0, 0, margin, 0);  // left, top, right, bottom
        imageView.setLayoutParams(imgParams);

        LinearLayout textLayout = new LinearLayout(context);
        textLayout.setOrientation(LinearLayout.VERTICAL);
        textLayout.setLayoutParams(new LinearLayout.LayoutParams
        (   LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        ));

        TextView toptextView = new TextView(context);
        toptextView.setTextSize(20);
        if (jdesc.isEmpty()) // ie. if a header row
        {   toptextView.setTextColor(Color.parseColor("#ffffff"));
            toptextView.setText(jtitle);
        } else
        {   if      (jprog.equals("Unsupported")) toptextView.setTextColor(Color.parseColor("#ff8888"));
            else if (jprog.equals("Unlocked"   )) toptextView.setTextColor(Color.parseColor("#88ff88"));
            else if (jprog.equals("Locked"     )) toptextView.setTextColor(Color.parseColor("#ffff88"));
            else                                  toptextView.setTextColor(Color.parseColor("#8888ff"));
            toptextView.setText(jtitle + " - " + jprog);
        }
        toptextView.setTypeface(null, Typeface.BOLD);
        toptextView.setLayoutParams(new LinearLayout.LayoutParams
        (   LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        ));
        textLayout.addView(toptextView);

        if (!jdesc.isEmpty())
        {   TextView bottomtextView = new TextView(context);
            bottomtextView.setTextSize(16);
            bottomtextView.setTextColor(0xFF888899);
            bottomtextView.setText(jdesc);
            bottomtextView.setLayoutParams(new LinearLayout.LayoutParams
            (   LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
            ));
            textLayout.addView(bottomtextView);
        }

        rowLayout.addView(imageView);
        rowLayout.addView(textLayout);

        layout_ra.addView(rowLayout, nextslot++);
}   }
