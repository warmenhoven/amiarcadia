package com.amigan.droidarcadia;

import static android.view.View.GONE;
import static android.view.View.VISIBLE;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.InputDeviceCompat;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Insets;
import android.graphics.Paint;
import android.graphics.Rect;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.net.Uri;
import android.os.Bundle;
import android.os.CombinedVibration;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewStub;
import android.view.WindowInsets;
import android.view.WindowMetrics;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("droidarcadia");
    }

    // constants
    public static final int _3DATTACKPOS       =  105,
                            COLOURS_PURE       =    0,
                            COLOURS_PVI        =    1,
                            COLOURS_UVI        =    2,
                            COLOURS_GREYSCALE  =    3,
                            GUESTCHANNELS      =   16,
                            INDICATOR_OFF      =    0,
                            INDICATOR_FPS      =    1,
                            INDICATOR_KHZ      =    2,
                            INDICATOR_PERCENT  =    3,
                            MEMMAP_LASERBATTLE =   11,
                            MEMMAP_LAZARIAN    =   12,
                            SENSITIVITY_DEFAULT =   3,
                            SIZE_OFF           =    0,
                            SIZE_28MM          =    1,
                            SIZE_160DP         =    2,
                            SIZE_240PX         =    3,
                            SIZE_MAX           =    4,
                            SPEED_1QUARTER     =    0,
                            SPEED_2QUARTERS    =    1,
                            SPEED_3QUARTERS    =    2,
                            SPEED_4QUARTERS    =    3,
                            SPEED_5QUARTERS    =    4,
                            SPEED_6QUARTERS    =    5,
                            SPEED_8QUARTERS    =    6,
                            SPEED_16QUARTERS   =    7,
                            SPEED_32QUARTERS   =    8,
                            SPEED_TURBO        =    9,
                            TEXTX              =    8,
                            TEXTY              =   25,
                            TOTALCHANNELS      =   16 + 6,
                            WAYS_0             =    0,
                            WAYS_2H            =    1,
                            WAYS_2V            =    2,
                            WAYS_4O            =    3,
                            WAYS_4D            =    4,
                            WAYS_8             =    5,
                            WAYS_ANALOG        =    6;

    // variables
    public  static boolean  artefacts         = false,
                            autocoin          = true,
                            autoframeskip     = false,
                            cheevos           = false, // must be false!
                            collisions        = true,
                            darkenbg          = true,
                            demultiplex       = true,
                            dejitter          = false,
                            fill              = false,
                            flagline          = true,
                            fresh_start       = true,
                            fresh_a           = true,
                            fresh_b           = true,
                            handedness        = false,
                            hardcore          = true,
                            invincibility     = false,
                            ispal             = true,
                            linebased         = false,
                            lives             = false,
                            lock              = true,
                            onebutton,
                            redrawkeypad      = false,
                            resizecontrollers = false,
                            retune            = false,
                            sameplayer        = true,
                            semipaused        = false,
                            skies             = false,
                            sound             = true,
                            spring            = true,
                            swapped           = false,
                            time              = false,
                            useff             = true,
                            hardkeys[]        = new boolean[12],
                            playing[]         = new boolean[TOTALCHANNELS],
                            softkeys[]        = new boolean[12],
                            hardxmoved        = false,
                            hardymoved        = false,
                            softmoved         = false;
    public  static int      nextscrnshot      = 1,
                            hiscore[]         = new int[51 + 6],
                            autofire          = 0,
                            availwidth,
                            availheight,
                            colours           = COLOURS_PURE,
                            hardxdelta        = 0,
                            hardydelta        = 0,
                            destwidth, destheight,
                            eachwidth, eachheight,
                            frameskip         = 1,
                            hardplayer,
                            hardx             = 32767,
                            hardy             = 32767,
                            hx                = -1,
                            hy                = -1,
                            indicator         = INDICATOR_OFF,
                            key1, key2, key3, key4,
                            keystate_start    = 0,
                            keystate_a        = 0,
                            keystate_b        = 0,
                            keystate_c        = 0,
                            layout            = WAYS_8,
                            machine           = 0,
                            memmap            = 0,
                            ox, oy,
                            rumbling          = 0,
                            sensitivity       = SENSITIVITY_DEFAULT,
                            size,
                            sizemethod        = SIZE_160DP,
                            speed             = SPEED_4QUARTERS,
                            softplayer,
                            softx, softy,
                            sourcewidth,
                            sourceheight,
                            stretch           = 0,
                            thekeypad[]       = new int[2],
                            topleft, centred, bottomright,
                            volume            = 8,
                            widewidth,
                            width, height,
                            whichgame;
    public  static float    dptopx, mmtopx;
    public  static double   scaleby           = 1;
    public  static LeftTouchView   lefttouch;
    public  static RightTouchView  righttouch;
    public         ViewStub        leftvs, rightvs;
    public  static Vibrator        vibrator;
    public  static VibratorManager vm;
    public  static AudioTrack[]    audio;
    public  static String          imageUrl,
                                   jprogressurl,
                                   ratoaststr,
                                   password   = "",
                                   progressstr,
                                   trackerstr,
                                   username   = "";
    public  static String[]        keystring;
    private long            newtime,
                            waitfor,
                            waittill          = 0;
    private int             badframes         = 0,
                            dispint,
                            frames            = 0,
                            narrowest,
                            oldfpsframes      = 0,
                            oldrumbling;
    private double          dispfloat,
                            elapsedframes,
                            elapsedtime,
                            fpsamount,
                            oldfpstime;
    private Bitmap          bitmap;
    private ByteBuffer      bytebuffer;
    private ImageView       toastimage;
    private LinearLayout    controller,
                            screenlayout;
    private Paint           mp, // main paint
                            tp; // text paint
    private Rect            grect,
                            sourcerect,
                            destrect;
    private TextView        button_start,
                            button_a,
                            button_b,
                            button_c,
                            button_load,
                            button_pause,
                            button_options,
                            button_quit;

    private static Bitmap   toastbitmap,
                            scaledprogress,
                            scaledtracker,
                            unscaledprogress;

    private static Toast    customToast;
    public  static int      progresswidth     = 0,
                            progressheight    = 0,
                            trackerwidth      = 0,
                            trackerheight     = 0;

    private final Paint     overlaypaint      = new Paint();
    
    public  boolean         paused            = false;
    public  Canvas          canvas;
    public  SurfaceHolder   holder;
    public  SurfaceView     screen;
        
    public  static String[] consolestring;
    public  static int      keytable_to_activity[] =
    { 10, // 0
       0, // 1
       1, // 2
       2, // 3
       3, // 4
       4, // 5
       5, // 6
       6, // 7
       7, // 8
       8, // 9
       9, // Cl
      11, // En
      -1, // x1
      -1, // x2
      -1, // x3
      -1  // x4
    };

    public static Context appContext;

    public native int     doautofire();
    public native boolean canpause();
    public native int     cgetoptions1();
    public native int     cgetoptions2();
    public native int     cloadcos(int whichtype, int thelength, byte[] thefile, boolean reconfigure);
    // public native void cloadnvram(byte[] thefile);
    public native byte[]  csavecos();
    // public native byte[] csavenvram();
    public native void    csetoptions(int options1, int options2);
    public native void    csoundoff();
    public native int     emulate();
    public native void    endofframe();
    // public native int  getcycles();
    public native int     getdigipos(int whichpos);
    public native int     getff();
    public native int     getfirebuttons();
    public native String  getbadgegfxurl();
    public native int     getgame();
    public native String  getgamegfxurl();
    public native int     gethiscore(int whichscore);
    public native byte[]  getkeyname(int whichkey);
    public native String  getmessage();
    public native String  getprogressstr();
    public native String  getprogressurl();
    public native int     getracolour();
    public native int     getsensitivity();
    public native short[] getsoundbuffer(int whichchan);
    public native int     getstarting();
    public native int     getstopping();
    public native String  gettrackerstr();
    public native boolean isbadgegfx();
    public native boolean isprogressing();
    public native boolean istracking();
    public native boolean jsetravars(String jusername, String jpassword, boolean jcheevos, boolean jhardcore);
    public native int     loadgame(String passedname, int thelength, byte[] thefile);
    public native void    redrawscreen();
    public native int     setconsolekey(int whichkey);
    public native void    setframebuffer(ByteBuffer buf);
    public native void    sethiscore(int whichscore, int thescore);
    public native void    setinput(int player, int thekeypad, int cx, int cy);
    public native void    jshutdownraclient();
    public native void    clientidle();
    
    private final Handler thehandler = new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {   super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        appContext = getApplicationContext();

        int i;
        
        for (i = 0; i < TOTALCHANNELS; i++)
        {   playing[i] = false;
        }
        for (i = 0; i < 12; i++)
        {   hardkeys[i] = softkeys[i] = false;
        }
        for (i = 0; i < 51 + 6; i++)
        {   hiscore[i] = 0;
        }

        sourcerect = new Rect(0, 0, 0, 0 );
        destrect   = new Rect(0, 0, 0, 0 );
        bytebuffer = ByteBuffer.allocateDirect(240 * 269 * 2);
        bitmap     = Bitmap.createBitmap(240, 269, Bitmap.Config.RGB_565);
        setframebuffer(bytebuffer);

        screen = findViewById(R.id.screen);
        holder = screen.getHolder();
        holder.addCallback(this);

        mp = new Paint(Paint.ANTI_ALIAS_FLAG);
        mp.setColor(Color.BLACK);
        tp = new Paint(Paint.ANTI_ALIAS_FLAG);
        tp.setTextSize(20);
        tp.setTextAlign(Paint.Align.LEFT); // probably not needed?
        grect = new Rect();
        tp.getTextBounds("#", 0, 1, grect);

        ff_init();
        
        button_load  = findViewById(R.id.button_load);
        button_pause = findViewById(R.id.button_pause);
        button_options = findViewById(R.id.button_options);
        button_quit  = findViewById(R.id.button_quit);
        button_start = findViewById(R.id.button_start);
        button_a     = findViewById(R.id.button_a);
        button_b     = findViewById(R.id.button_b);
        button_c     = findViewById(R.id.button_c);
        controller   = findViewById(R.id.controller);
        screenlayout = findViewById(R.id.screenlayout);
        leftvs       = findViewById(R.id.leftvs);
        rightvs      = findViewById(R.id.rightvs);
        leftvs.inflate();
        rightvs.inflate();
        lefttouch    = findViewById(R.id.lefttouch);  // causes spurious warnings as these don't exist in the XML
        righttouch   = findViewById(R.id.righttouch); // (because they are inflated at runtime)
        if (lefttouch == null || righttouch == null)
        {   sizemethod = SIZE_OFF;
        }

        loadconfig();
        if (lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }
        sizecontrollers();
        loadhs();

        if (paused)
        {   button_pause.setBackgroundColor(Color.parseColor("#00ffff"));
            button_pause.setText(R.string.unpause); // not enough room for "Unpause"
        }

        String language = getResources().getConfiguration().getLocales().get(0).getLanguage();
        // Toast.makeText(MainActivity.this, "Language is " + language, Toast.LENGTH_LONG).show();
        if (language.equals("el"))
        {   getsizes();
            if (narrowest <= 480)
            {   button_load.setTextSize(   15); // more important
                button_pause.setTextSize(  15); // less important
                button_options.setTextSize(15); // less important
                button_quit.setTextSize(   15); // less important
        }   }

        button_load.setOnClickListener((view) ->
        {   ff_cancel();
            jsoundoff();
            csoundoff();
            semipaused = true;

            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            String[] mimeTypes = {"application/octet-stream", "application/zip"};
            intent.setType("*/*");
            intent.putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes);
            ASLActivityResultLauncher.launch(intent);
        });

        button_pause.setOnClickListener((view) ->
        {   if (paused)
            {   paused = false;
                button_pause.setBackgroundColor(Color.parseColor("#00dddd"));
                button_pause.setText(R.string.pause);
            } else if (canpause())
            {   paused = true;
                button_pause.setBackgroundColor(Color.parseColor("#00ffff"));
                button_pause.setText(R.string.unpause); // not enough room for "Unpause"
                ff_cancel();
                jsoundoff();
                csoundoff();
            }
        });

        button_options.setOnClickListener((view) ->
        {   saveconfig();
            savehs();
            ff_cancel(); // this doesn't seem to be effective
            Intent myIntent = new Intent(MainActivity.this, OptionsActivity.class);
            MainActivity.this.startActivity(myIntent);
        });

        button_quit.setOnClickListener((view) ->
        {   ff_cancel();
            jsoundoff();
            csoundoff();
            jshutdownraclient();
            jsavecos(0);
            saveconfig();
            savehs();
            setResult(Activity.RESULT_OK);
            finish();
            System.exit(0);
        });

        button_start.setOnClickListener((view) ->
        {   keystate_start = setconsolekey(0);
            button_start.setBackgroundColor(Color.parseColor("#ffffff"));
        });

        button_a.setOnClickListener((view) ->
        {   keystate_a = setconsolekey(1);
            button_a.setBackgroundColor(Color.parseColor("#ffffff"));
        });

        button_b.setOnClickListener((view) ->
        {   keystate_b = setconsolekey(2);
            button_b.setBackgroundColor(Color.parseColor("#ffffff"));
        });

        button_c.setOnClickListener((view) ->
        {   keystate_c = setconsolekey(3);
            button_c.setBackgroundColor(Color.parseColor("#ffffff"));
        });

        screen.setOnClickListener((view) ->
        {   if (stretch == 3) // full
            {   stretch = 0; // off
            } else
            {   if (stretch == 0 && ispal) // off
                {   stretch = 2; // off -> stretch 4:3 large
                } else
                {   stretch++;
            }   }
        });
        
        // Get the intent that started this activity
        Intent  intent = getIntent();
        boolean loaded = false;
        if (intent != null)
        {   Uri data = intent.getData();
            if (data != null)
            {   String intenttype = intent.getType();
                if
                (   intenttype != null
                 && (   intenttype.equals("application/octet-stream")
                     || intenttype.equals("application/zip")
                )   )
                {   jloadrom(intent);
                    loaded = true;
        }   }   }
        if (!loaded)
        {   jloadcos(0);
        }
        dokeynames(); // important, otherwise we can crash at startup!
        
        audio = new AudioTrack[TOTALCHANNELS];
        for (i = 0; i < TOTALCHANNELS; i++)
        {   audio[i] = new AudioTrack
            (   new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_GAME)
                    .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                    .build(),
                new AudioFormat.Builder()
                    .setSampleRate(11025)
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                    .build(),
                11025 * 2 * 2, // bufferSizeInBytes: 11025 hertz * 2 seconds * 2 bytes
                AudioTrack.MODE_STATIC,
                AudioManager.AUDIO_SESSION_ID_GENERATE // sessionId
            );
            if (audio[i] == null)
            {   Toast.makeText(MainActivity.this, getResources().getString(R.string.cantallocatechannel) + " " + i + "!", Toast.LENGTH_LONG).show();
        }   }

        /* if (true) // make this into a proper option and apply it to all activities
        {   getWindow().addFlags(  WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        } else
        {   getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);    
        } */

        hardcore = jsetravars(username, password, cheevos, hardcore); // calls rainit() if necessary

        oldfpstime = SystemClock.elapsedRealtimeNanos();
        thehandler.removeCallbacks(thethread);
        thehandler.postDelayed(thethread, 0);
    }

    @Override
    public boolean dispatchKeyEvent(android.view.KeyEvent ke)
    {   if
        (   ((ke.getSource() & InputDeviceCompat.SOURCE_JOYSTICK) == InputDeviceCompat.SOURCE_JOYSTICK)
         || ((ke.getSource() & InputDeviceCompat.SOURCE_GAMEPAD ) == InputDeviceCompat.SOURCE_GAMEPAD )
         || ((ke.getSource() & InputDeviceCompat.SOURCE_KEYBOARD) == InputDeviceCompat.SOURCE_KEYBOARD) // eg. remote control
        )
        {   int keycode = ke.getKeyCode();

            if (ke.getAction() == KeyEvent.ACTION_DOWN)
            {   if      (keycode == KeyEvent.KEYCODE_BUTTON_A     ) hardkeys[key4] = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_B     ) hardkeys[key3] = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_X     ) hardkeys[key2] = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_Y
                      || keycode == KeyEvent.KEYCODE_DPAD_CENTER
                      || keycode == KeyEvent.KEYCODE_BUTTON_R1    ) hardkeys[key1] = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_START  && fresh_start) { fresh_start = false; keystate_start = setconsolekey(0); button_start.setBackgroundColor(Color.YELLOW); }
                else if (keycode == KeyEvent.KEYCODE_BUTTON_SELECT && fresh_a    ) { fresh_a     = false; keystate_a     = setconsolekey(1); button_a.setBackgroundColor(    Color.YELLOW); } // A/UC
                else if (keycode == KeyEvent.KEYCODE_BUTTON_L1     && fresh_b    ) { fresh_b     = false; keystate_b     = setconsolekey(2); button_b.setBackgroundColor(    Color.YELLOW); } // B/LC
                else if (keycode == KeyEvent.KEYCODE_DPAD_LEFT    ) { hardxmoved = true; if (layout == WAYS_ANALOG) hardxdelta = -1; else hardx = 256 * topleft;     }
                else if (keycode == KeyEvent.KEYCODE_DPAD_RIGHT   ) { hardxmoved = true; if (layout == WAYS_ANALOG) hardxdelta =  1; else hardx = 256 * bottomright; }
                else if (keycode == KeyEvent.KEYCODE_DPAD_UP      ) { hardymoved = true; if (layout == WAYS_ANALOG) hardydelta = -1; else hardy = 256 * topleft;     }
                else if (keycode == KeyEvent.KEYCODE_DPAD_DOWN    ) { hardymoved = true; if (layout == WAYS_ANALOG) hardydelta =  1; else hardy = 256 * bottomright; }
                else                                                return super.dispatchKeyEvent(ke);
                if (sameplayer && sizemethod != SIZE_OFF)
                {   lefttouch.invalidate();
                    righttouch.invalidate();
                }
                return true;
            } else if (ke.getAction() == KeyEvent.ACTION_UP)
            {   if      (keycode == KeyEvent.KEYCODE_BUTTON_A     ) hardkeys[key4] = false;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_B     ) hardkeys[key3] = false;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_X     ) hardkeys[key2] = false;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_Y
                      || keycode == KeyEvent.KEYCODE_DPAD_CENTER
                      || keycode == KeyEvent.KEYCODE_BUTTON_R1    ) hardkeys[key1] = false;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_START ) fresh_start    = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_SELECT) fresh_a        = true;
                else if (keycode == KeyEvent.KEYCODE_BUTTON_L1    ) fresh_b        = true;
                else if (keycode == KeyEvent.KEYCODE_DPAD_LEFT
                      || keycode == KeyEvent.KEYCODE_DPAD_RIGHT   ) { hardxmoved = false; if (layout == WAYS_ANALOG) hardxdelta = 0; else hardx = 256 * centred; }
                else if (keycode == KeyEvent.KEYCODE_DPAD_UP
                      || keycode == KeyEvent.KEYCODE_DPAD_DOWN    ) { hardymoved = false; if (layout == WAYS_ANALOG) hardydelta = 0; else hardy = 256 * centred; }
                else                                                return super.dispatchKeyEvent(ke);

                if (sameplayer && sizemethod != SIZE_OFF)
                {   lefttouch.invalidate();
                    righttouch.invalidate();
                }
                return true;
        }   }

        return super.dispatchKeyEvent(ke);
    }

    @Override
    public void onBackPressed()
    {   ff_cancel();
        jsoundoff();
        csoundoff();
        jsavecos(0);
        // jsavenvram();
        saveconfig();
        savehs();
        setResult(Activity.RESULT_OK);
        finish();
        System.exit(0);
    }

    @Override
    public boolean dispatchGenericMotionEvent(android.view.MotionEvent me)
    {   float fx, fy;
        int   action = me.getAction();

        if
        (   (me.getSource() & InputDeviceCompat.SOURCE_JOYSTICK) != InputDeviceCompat.SOURCE_JOYSTICK
         && (me.getSource() & InputDeviceCompat.SOURCE_GAMEPAD ) != InputDeviceCompat.SOURCE_GAMEPAD
         && (me.getSource() & InputDeviceCompat.SOURCE_KEYBOARD) != InputDeviceCompat.SOURCE_KEYBOARD // eg. remote control
         && (me.getSource() & InputDeviceCompat.SOURCE_DPAD    ) != InputDeviceCompat.SOURCE_DPAD
        )
        {   return super.dispatchGenericMotionEvent(me);
        }

        if (action == MotionEvent.ACTION_MOVE)
        {   fx = (me.getAxisValue(MotionEvent.AXIS_X) + (float) 1.0) * (float) 32767.0; // -1.0..+1.0 -> 0.0..65535.0
            fy = (me.getAxisValue(MotionEvent.AXIS_Y) + (float) 1.0) * (float) 32767.0; // -1.0..+1.0 -> 0.0..65535.0
            if (layout == WAYS_ANALOG)
            {   if      (fx < 32767.0 - 3000.0) { hardxdelta = -1;                hardxmoved = true;  }
                else if (fx > 32767.0 + 3000.0) { hardxdelta =  1;                hardxmoved = true;  }
                else                            { hardxdelta =  0;                hardxmoved = false; }
                if      (fy < 32767.0 - 3000.0) { hardydelta = -1;                hardymoved = true;  }
                else if (fy > 32767.0 + 3000.0) { hardydelta =  1;                hardymoved = true;  }
                else                            { hardydelta =  0;                hardymoved = false; }
            } else
            {   if      (fx < 32767.0 - 3000.0) { hardx      = 256 * topleft;     hardxmoved = true;  }
                else if (fx > 32767.0 + 3000.0) { hardx      = 256 * bottomright; hardxmoved = true;  }
                else                            { hardx      = 256 * centred;     hardxmoved = false; }
                if      (fy < 32767.0 - 3000.0) { hardy      = 256 * topleft;     hardymoved = true;  }
                else if (fy > 32767.0 + 3000.0) { hardy      = 256 * bottomright; hardymoved = true;  }
                else                            { hardy      = 256 * centred;     hardymoved = false; }
        }   }

        if (sizemethod != SIZE_OFF)
        {   lefttouch.invalidate();
            righttouch.invalidate();
        }
        
        return true;
    }
    
    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {   DrawScreen(holder); // maybe not needed
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int frmt, int w, int h)
    {   DrawScreen(holder); // maybe not needed
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {}

    // You can do the assignment inside onAttach or onCreate, ie. before the activity is displayed
    ActivityResultLauncher<Intent> ASLActivityResultLauncher = registerForActivityResult(
        new ActivityResultContracts.StartActivityForResult(),
        result ->
        {   if (result.getResultCode() == Activity.RESULT_OK)
            {   // There are no request codes
                Intent data = result.getData();
                if (data != null)
                {   jloadrom(data);
            }   }
            semipaused = false;
        });

    @Override
    public void onSaveInstanceState(Bundle savedInstanceState)
    {   super.onSaveInstanceState(savedInstanceState);

        jsavecos(0);
        saveconfig();
        savehs();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {   ff_cancel();
        jsoundoff();
        csoundoff();
        for (int i = 0; i < TOTALCHANNELS; i++)
        {   audio[i].release();
        }
        if (thehandler != null && thethread != null)
        {   thehandler.removeCallbacks(thethread);
        }
        super.onConfigurationChanged(newConfig);
        this.recreate();
        // don't call this.finish()!
    }
    
    private final Runnable thethread = new Runnable()
    {   public void run()
        {   if (paused || semipaused)
            {   ff_cancel();
                if (cheevos)
                {   clientidle(); // only needs to be done about once per second, not ten times per second
                }
                thehandler.removeCallbacks(thethread);
                thehandler.postDelayed(this, 100);
                return;
            } // implied else

            int i;

            // this is wait_first()
            newtime = SystemClock.elapsedRealtimeNanos();
            if (speed != SPEED_TURBO)
            {   if (ispal) waitfor = 20000000;
                else       waitfor = 16666667;
                if      (speed == SPEED_1QUARTER  ) /*  25% */ waitfor *= 4;
                else if (speed == SPEED_2QUARTERS ) /*  50% */ waitfor *= 2;
                else if (speed == SPEED_3QUARTERS ) /*  75% */ waitfor =  (waitfor * 4) / 3; // each frame takes 133.3'% of its normal time. Ie. in 4 frames worth of normal time, we've done 3 frames
                else if (speed == SPEED_5QUARTERS ) /* 125% */ waitfor =  (waitfor * 4) / 5; // each frame takes  80   % of its normal time. Ie. in 4 frames worth of normal time, we've done 5 frames
                else if (speed == SPEED_6QUARTERS ) /* 150% */ waitfor =  (waitfor * 2) / 3; // each frame takes  66.6'% of its normal time. Ie. in 4 frames worth of normal time, we've done 6 frames
                else if (speed == SPEED_8QUARTERS ) /* 200% */ waitfor /= 2;
                else if (speed == SPEED_16QUARTERS) /* 400% */ waitfor /= 4;
                else if (speed == SPEED_32QUARTERS) /* 800% */ waitfor /= 8;
                waittill += waitfor;
            
                if (waittill < newtime) // already too late
                {   waittill = newtime + waitfor;
            }   }

            emulate();
            if (frames % frameskip == 0)
            {   DrawScreen(holder);
            }
            if (resizecontrollers)
            {   resizecontrollers = false;
                sizecontrollers();
            }
                
            thekeypad[0] = thekeypad[1] = 0;
            softplayer = swapped ? 1 : 0;
            if (sameplayer)
            {   hardplayer =     softplayer;
            } else
            {   hardplayer = 1 - softplayer;
            }
            for (i = 0; i < 12; i++)
            {   if (softkeys[i])
                {   thekeypad[softplayer] |= (1 << i);
                }
                if (hardkeys[i])
                {   thekeypad[hardplayer] |= (1 << i);
            }   }
            // assert(key1 != -1);
            if
            (   autofire >= 1
            && (autofire == 2 || softkeys[key1] || hardkeys[key1] || ( whichgame == _3DATTACKPOS                                  && (softkeys[key2] || hardkeys[key2]))
                                                                  || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (softkeys[key4] || hardkeys[key4]))
            ))
            {   int result = doautofire();
                if (result >= 2) // 3D Attack
                {   if (result == 3) thekeypad[softplayer] |=  (1 << key1);
                    else             thekeypad[softplayer] &= ~(1 << key1);
                    if (result == 4) thekeypad[softplayer] |=  (1 << key2);
                    else             thekeypad[softplayer] &= ~(1 << key2);
                } else if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                {   if (result == 1) thekeypad[softplayer] |=  (1 << key4);
                    else             thekeypad[softplayer] &= ~(1 << key4);
                } else
                {   if (result == 1)
                    {                thekeypad[softplayer] |=  (1 << key1);
                                     thekeypad[hardplayer] |=  (1 << key1);
                    } else
                    {                thekeypad[softplayer] &= ~(1 << key1);
                                     thekeypad[hardplayer] &= ~(1 << key1);
            }   }   }

            if (layout == WAYS_ANALOG && (hardxmoved || hardymoved))
            {   hardx += hardxdelta * sensitivity * 256;
                if (hardx < 0) hardx = 0; else if (hardx > 65535) hardx = 65535;
                hardy += hardydelta * sensitivity * 256;
                if (hardy < 0) hardy = 0; else if (hardy > 65535) hardy = 65535;
                if (sameplayer && sizemethod != SIZE_OFF)
                {   if (!MainActivity.handedness) // left is paddle
                    {   lefttouch.invalidate();
                    } else // right is paddle
                    {   righttouch.invalidate();
            }   }   }

            if (sameplayer)
            {   // assert(softplayer == hardplayer);
                if (hardxmoved || hardymoved)
                {   setinput(hardplayer, thekeypad[hardplayer], hardx, hardy);
                    ox = hardx;
                    oy = hardy;
                } else if (softmoved)
                {   setinput(softplayer, thekeypad[softplayer], softx, softy);
                    ox = softx;
                    oy = softy;
                } else
                {   if (spring || layout != WAYS_ANALOG)
                    {   setinput(softplayer, thekeypad[softplayer], centred * 256, centred * 256);
                        ox = hardx = centred * 256;
                        oy = hardy = centred * 256;
                    } else
                    {   setinput(softplayer, thekeypad[softplayer], ox           , oy           );
                }   }
                setinput((softplayer == 1) ? 0 : 1, 0, centred * 256, centred * 256);
            } else
            {   setinput(hardplayer, thekeypad[hardplayer], hardx, hardy);
                if (softmoved)
                {   setinput(softplayer, thekeypad[softplayer], softx, softy);
                    ox = softx;
                    oy = softy;
                } else if (spring || layout != WAYS_ANALOG)
                {   setinput(softplayer, thekeypad[softplayer], centred * 256, centred * 256);
                    ox = centred * 256;
                    oy = centred * 256;
                } else
                {   setinput(softplayer, thekeypad[softplayer], ox        , oy           );
            }   }

            if (redrawkeypad)
            {   redrawkeypad = false;
                dokeynames();
                if (sizemethod != SIZE_OFF)
                {   lefttouch.invalidate();
                    righttouch.invalidate();
            }   }
            
            if (keystate_start > 0)
            {   keystate_start--;
                if (keystate_start == 0)
                {   button_start.setBackgroundColor(Color.parseColor("#aabbaa"));
            }   }
            if (keystate_a > 0)
            {   keystate_a--;
                if (keystate_a == 0)
                {   button_a.setBackgroundColor(Color.parseColor("#889988"));
            }   }
            if (keystate_b > 0)
            {   keystate_b--;
                if (keystate_b == 0)
                {   int orientation = getResources().getConfiguration().orientation;

                    if (orientation == Configuration.ORIENTATION_PORTRAIT)
                    {   button_b.setBackgroundColor(Color.parseColor("#889988"));
                    } else // if (orientation == Configuration.ORIENTATION_LANDSCAPE)
                    {   button_b.setBackgroundColor(Color.parseColor("#aabbaa"));
            }   }   }
            if (keystate_c > 0)
            {   keystate_c--;
                if (keystate_c == 0)
                {   int orientation = getResources().getConfiguration().orientation;

                    if (orientation == Configuration.ORIENTATION_PORTRAIT)
                    {   button_c.setBackgroundColor(Color.parseColor("#aabbaa"));
                    } else // if (orientation == Configuration.ORIENTATION_LANDSCAPE)
                    {   button_c.setBackgroundColor(Color.parseColor("#889988"));
            }   }   }

            oldrumbling = rumbling;
            int rumble = getff();
            if (rumble == 0)
            {   if (rumbling > 0)
                {   rumbling--;
            }   }
            else
            {   rumbling = rumble;
            }
            if (oldrumbling == 0 && rumbling > 0)
            {   final VibrationEffect ve = VibrationEffect.createOneShot(999999999, VibrationEffect.DEFAULT_AMPLITUDE); // about 1 million seconds
                ff_cancel();
                if (android.os.Build.VERSION.SDK_INT >= 31) // Android 12+
                {   CombinedVibration cv = CombinedVibration.createParallel(ve);
                    vm.vibrate(cv);
                } else
                {   vibrator.vibrate(ve);
            }   }
            else if (oldrumbling > 0 && rumbling == 0)
            {   ff_cancel();
            }
                    
            endofframe();
            frames++;

            if (MainActivity.sound)
            {   int rc1 = getstarting();
                int rc2 = getstopping();
                int wtf;

                for (i = 0; i < TOTALCHANNELS; i++)
                {   wtf = 1 << i;
                    if ((rc1 & wtf) != 0)
                    {   if (playing[i])
                        {   audio[i].stop();
                        }

                        short thedata[] = getsoundbuffer(i);

                        if (i < GUESTCHANNELS)
                        {   audio[i].setLoopPoints(0, thedata.length, -1); // frame size is sample size in bytes multiplied by channel count
                        }
                        audio[i].write(thedata, 0, thedata.length);
                        audio[i].play();
                        playing[i] = true;
                    } else if (playing[i])
                    {   if ((rc2 & wtf) != 0)
                        {   audio[i].stop();
                            playing[i] = false;
            }   }   }   }

            // this is wait_second()
        
            newtime = SystemClock.elapsedRealtimeNanos();
            if (speed == SPEED_TURBO)
            {   thehandler.removeCallbacks(thethread);
                thehandler.postDelayed(this, 0);
            } else
            {   if
                (   autoframeskip
                 && frameskip < 10
                 && newtime   > waittill
                )
                {   badframes++;
                    if (badframes >= 50)
                    {   badframes = 0;
                        frameskip++;
                }   }
                else
                {   badframes = 0;
                }
                thehandler.removeCallbacks(thethread);
                thehandler.postDelayed(this, (waittill - newtime) / 1000000); // nanoseconds -> milliseconds
    }   }   };

    @SuppressWarnings("deprecation")
    private void DrawScreen(SurfaceHolder holder)
    {   canvas = holder.lockCanvas();
        if (canvas != null) // important!
        {   if (machine <= 2) // Arcadia, Interton, Elektor
            {   sourcewidth  = 164;
                widewidth = sourcewidth * 2;
                if (ispal)
                {   sourceheight = 269;
                } else
                {   sourceheight = 226;
            }   }
            else
            {   sourcewidth  =
                widewidth    =
                sourceheight = 240;
            }
            sourcerect.right  = sourcewidth;
            sourcerect.bottom = sourceheight;
            availwidth  = canvas.getWidth();
            availheight = canvas.getHeight();
            if (stretch == 0 || (stretch == 1 && machine >= 3)) // off (ie. integer scaling only)
            {   size = Math.min(availwidth / widewidth, availheight / sourceheight);
                destwidth  = widewidth    * size;
                destheight = sourceheight * size;
            } else if (stretch == 1) // 4:3 small
            {   size = Math.min(availwidth / widewidth, availheight /          269);
                destwidth  = widewidth * size;
                destheight =       269 * size;
            } else if (stretch == 2) // 4:3 large
            {   if (availwidth * 3 / 4 <= availheight)
                {   destwidth  = availwidth;
                    destheight = availwidth  * 3 / 4;
                } else
                {   destwidth  = availheight * 4 / 3;
                    destheight = availheight;
            }   }
            else if (stretch == 3) // full
            {   destwidth  = availwidth;
                destheight = availheight;
            }
            destrect.left   =  (availwidth  - destwidth ) / 2;
            destrect.top    =  (availheight - destheight) / 2;
            destrect.right  = ((availwidth  - destwidth ) / 2) + destwidth;
            destrect.bottom = ((availheight - destheight) / 2) + destheight;

            bytebuffer.rewind();
            bitmap.copyPixelsFromBuffer(bytebuffer);
            canvas.drawBitmap(bitmap, sourcerect, destrect, null);

            if (destrect.left   >           0) canvas.drawRect(             0,               0, destrect.left , availheight,  mp); // left
            if (destrect.right  <  availwidth) canvas.drawRect(destrect.right,               0, availwidth,     availheight,  mp); // right
            if (destrect.top    >           0) canvas.drawRect(destrect.left ,               0, destrect.right, destrect.top, mp); // top
            if (destrect.bottom < availheight) canvas.drawRect(destrect.left , destrect.bottom, destrect.right, availheight,  mp); // bottom

            if (cheevos)
            {   if (isprogressing() && progresswidth > 0 && scaledprogress != null)
                {   progressstr = getprogressstr();
                    overlaypaint.setAlpha(192); // 75% opaque
                    canvas.drawBitmap(scaledprogress, 10, availheight - 20 - (progressheight * 2), overlaypaint);
                    tp.setColor(Color.argb(192,   0,   0, 0));
                    canvas.drawText(progressstr, 22 + progresswidth, availheight - 20 - progressheight, tp);
                    tp.setColor(Color.argb(192, 255, 192, 0)); // orange
                    canvas.drawText(progressstr, 20 + progresswidth, availheight - 22 - progressheight, tp);
                }

                if (istracking() && trackerwidth > 0 && scaledtracker != null)
                {   trackerstr = gettrackerstr();
                    overlaypaint.setAlpha(192); // 75% opaque
                    canvas.drawBitmap(scaledtracker , 10,        availheight - 10 - trackerheight, overlaypaint);
                    tp.setColor(Color.argb(192,   0,   0, 0));
                    canvas.drawText(trackerstr, 22 + trackerwidth , availheight - 10, tp);
                    tp.setColor(Color.argb(192, 255, 192, 0)); // orange
                    canvas.drawText(trackerstr, 20 + trackerwidth , availheight - 12, tp);
            }   }

            newtime = SystemClock.elapsedRealtimeNanos();
            if (newtime - oldfpstime >= 1000000000) // one billion nanoseconds (ie. one second)
            {   elapsedtime       = newtime - oldfpstime;   // elapsed time   since last update (in nanoseconds)
                if (elapsedtime > 0.0)
                {   elapsedframes = frames  - oldfpsframes; // elapsed frames since last update
                    oldfpstime    = newtime;
                    oldfpsframes  = frames;
                    fpsamount     = elapsedframes / (elapsedtime / 1000000000); // this adjusts it to account for time since last update
            }   }

            if (indicator == INDICATOR_FPS)
            {   if (elapsedtime == 0.0)
                {   shadowtext(getResources().getString(R.string.fps) + ": -");
                } else
                {   dispfloat = fpsamount + 0.5; // for rounding
                    dispint = (int) dispfloat;
                    shadowtext(getResources().getString(R.string.fps) + ": " + dispint);
            }   }
            else if (indicator == INDICATOR_KHZ)
            {   if (elapsedtime == 0.0)
                {   shadowtext(getResources().getString(R.string.khz) + ": -");
                } else
                {   // dispfloat = ((((ispal ? 50.0 : 60.0) * machines[machine].cpf) / 1,000) * cpl) / 227;
                    dispfloat =  fpsamount / (ispal ? 50.0 : 60.0);
                    dispfloat *= (ispal ? 886.724 : 894.886);
                    dispfloat += 0.5; // for rounding
                    dispint   =  (int) dispfloat;
                    shadowtext(getResources().getString(R.string.khz) + ": " + dispint);
            }   }
            else if (indicator == INDICATOR_PERCENT)
            {   if (elapsedtime == 0.0)
                {   shadowtext("-%");
                } else
                {   dispfloat =  (fpsamount * 100.0) / (ispal ? 50.0 : 60.0);
                    dispfloat += 0.5; // for rounding
                    dispint   =  (int) dispfloat;
                    shadowtext(dispint + "%");
            }   }
            // else
            // {   shadowtext(String.valueOf(Thread.activeCount()));
            // }

            if (cheevos)
            {   ratoaststr = getmessage();
                if (!ratoaststr.isEmpty())
                {   customToast = new Toast(getApplicationContext());
                    LayoutInflater inflater = getLayoutInflater();
                    View customView = inflater.inflate(R.layout.customtoast, null);
                    toastimage = customView.findViewById(R.id.toast_image);
                    TextView textView = customView.findViewById(R.id.toast_text);
                    if (isbadgegfx())
                    {   imageUrl = getbadgegfxurl();
                        if (!imageUrl.isEmpty())
                        {   new DownloadBadgeGfxTask(this).execute(imageUrl);
                    }   }
                    else
                    {   imageUrl = getgamegfxurl();
                        if (!imageUrl.isEmpty())
                        {   new DownloadGameGfxTask(this).execute(imageUrl);
                    }   }
                    textView.setText(ratoaststr);
                    customToast.setView(customView);
                    customToast.setDuration(Toast.LENGTH_LONG);
                    int racolour = getracolour();
                    if      (racolour == 1) textView.setBackgroundColor(Color.parseColor("#FFAAAA")); // red
                    else if (racolour == 2) textView.setBackgroundColor(Color.parseColor("#AAAAFF")); // blue
                    else if (racolour == 3) textView.setBackgroundColor(Color.parseColor("#FFFFAA")); // yellow
                    else if (racolour == 4) textView.setBackgroundColor(Color.parseColor("#AAFFAA")); // green
                    else if (racolour == 5) textView.setBackgroundColor(Color.parseColor("#FFD4AA")); // orange
                    if (imageUrl.isEmpty())
                    {   customToast.show();
                }   }

                if (isprogressing())
                {   jprogressurl = getprogressurl();
                    if (!jprogressurl.isEmpty())
                    {   new DownloadProgressGfxTask(this).execute(jprogressurl);
                        jprogressurl = "";
            }   }   }
            
            holder.unlockCanvasAndPost(canvas);
        } /* else
        {   Toast.makeText(MainActivity.this, "Can't lock screen!", Toast.LENGTH_SHORT).show();
        } */
    }
    
    private void shadowtext(String thetext)
    {   tp.setColor(Color.argb(192,   0,   0, 0));
        canvas.drawText(thetext, TEXTX + 2, TEXTY + 2, tp);
        tp.setColor(Color.argb(192, 255, 192, 0)); // orange
        canvas.drawText(thetext, TEXTX    , TEXTY    , tp);
    }

    private void jsoundoff()
    {   int i;
    
        for (i = 0; i < TOTALCHANNELS; i++)
        {   if (playing[i])
            {   audio[i].stop();
                playing[i] = false;
    }   }   }

    private void loadconfig()
    {   try (FileInputStream fis = openFileInput("DroidArcadia.ini"))
        {   if (fis.read() == 25)                //  0 version byte (25 means V4.41+)
            {   linebased     = fis.read() == 1; //  1
                stretch       = fis.read()     ; //  2
                autofire      = fis.read()     ; //  3
                swapped       = fis.read() == 1; //  4
                useff         = fis.read() == 1; //  5
                ispal         = fis.read() == 1; //  6
                sound         = fis.read() == 1; //  7
                retune        = fis.read() == 1; //  8
                collisions    = fis.read() == 1; //  9
                demultiplex   = fis.read() == 1; // 10
                lives         = fis.read() == 1; // 11
                time          = fis.read() == 1; // 12
                invincibility = fis.read() == 1; // 13
                layout        = fis.read()     ; // 14
                indicator     = fis.read()     ; // 15
                machine       = fis.read()     ; // 16
                frameskip     = fis.read()     ; // 17
                volume        = fis.read()     ; // 18
                paused        = fis.read() == 1; // 19
                speed         = fis.read()     ; // 20
                skies         = fis.read() == 1; // 21
                artefacts     = fis.read() == 1; // 22
                fill          = fis.read() == 1; // 23
                dejitter      = fis.read() == 1; // 24
                handedness    = fis.read() == 1; // 25
                colours       = fis.read()     ; // 26
                sizemethod    = fis.read()     ; // 27
                spring        = fis.read() == 1; // 28
                nextscrnshot  = fis.read() * 256; // 29
                nextscrnshot += fis.read()     ; // 30
                lock          = fis.read() == 1; // 31
                autoframeskip = fis.read() == 1; // 32
                memmap        = fis.read()     ; // 33
                autocoin      = fis.read() == 1; // 34
                cheevos       = fis.read() == 1; // 35
                hardcore      = fis.read() == 1; // 36
                darkenbg      = fis.read() == 1; // 37
                flagline      = fis.read() == 1; // 38
                sameplayer    = fis.read() == 1; // 39
                sensitivity   = fis.read()     ; // 40
                try (BufferedReader reader = new BufferedReader(new InputStreamReader(fis, StandardCharsets.UTF_8)))
                {   username = reader.readLine();
                    if (username == null) username = "";
                    password = reader.readLine();
                    if (password == null) password = "";
                } catch (IOException e)
                {   ;
                }
                // don't call fis.close()!
                jsetoptions();
            } /* else
            {   ; // don't call fis.close()!
            } */
        }
        catch (IOException e)
        {   ; // configuration file does not exist
    }   }

    private void saveconfig()
    {   try (FileOutputStream fos = openFileOutput("DroidArcadia.ini", MODE_PRIVATE))
        {   fos.write(25);                                      //  0 version byte (25 means V4.41+)
            if (linebased    ) fos.write(1); else fos.write(0); //  1
            fos.write((byte) stretch);                          //  2
            fos.write((byte) autofire);                         //  3
            if (swapped      ) fos.write(1); else fos.write(0); //  4
            if (useff        ) fos.write(1); else fos.write(0); //  5
            if (ispal        ) fos.write(1); else fos.write(0); //  6
            if (sound        ) fos.write(1); else fos.write(0); //  7
            if (retune       ) fos.write(1); else fos.write(0); //  8
            if (collisions   ) fos.write(1); else fos.write(0); //  9
            if (demultiplex  ) fos.write(1); else fos.write(0); // 10
            if (lives        ) fos.write(1); else fos.write(0); // 11
            if (time         ) fos.write(1); else fos.write(0); // 12
            if (invincibility) fos.write(1); else fos.write(0); // 13
            fos.write((byte) layout);                           // 14
            fos.write((byte) indicator);                        // 15
            fos.write((byte) machine);                          // 16
            fos.write((byte) frameskip);                        // 17
            fos.write((byte) volume);                           // 18
            if (paused       ) fos.write(1); else fos.write(0); // 19
            fos.write((byte) speed);                            // 20
            if (skies        ) fos.write(1); else fos.write(0); // 21
            if (artefacts    ) fos.write(1); else fos.write(0); // 22
            if (fill         ) fos.write(1); else fos.write(0); // 23
            if (dejitter     ) fos.write(1); else fos.write(0); // 24
            if (handedness   ) fos.write(1); else fos.write(0); // 25
            fos.write((byte) colours);                          // 26
            fos.write((byte) sizemethod);                       // 27
            if (spring       ) fos.write(1); else fos.write(0); // 28
            fos.write((byte) (nextscrnshot / 256));             // 29
            fos.write((byte) (nextscrnshot % 256));             // 30
            if (lock         ) fos.write(1); else fos.write(0); // 31
            if (autoframeskip) fos.write(1); else fos.write(0); // 32
            fos.write((byte) memmap);                           // 33
            if (autocoin     ) fos.write(1); else fos.write(0); // 34
            if (cheevos      ) fos.write(1); else fos.write(0); // 35
            if (hardcore     ) fos.write(1); else fos.write(0); // 36
            if (darkenbg     ) fos.write(1); else fos.write(0); // 37
            if (flagline     ) fos.write(1); else fos.write(0); // 38
            if (sameplayer   ) fos.write(1); else fos.write(0); // 39
            fos.write((byte) sensitivity);                      // 40
            if (username == null) username = "";
            fos.write(username.getBytes(StandardCharsets.UTF_8));
            fos.write('\n'); // delimiter to separate username from password
            if (password == null) password = "";
            fos.write(password.getBytes(StandardCharsets.UTF_8));
            // Toast.makeText(MainActivity.this, "Saved configuration.", Toast.LENGTH_SHORT).show();
            // don't call fos.close()!
            jsetoptions();
        } catch (IOException e)
        {   ; // can't create/open configuration file
    }   }

    private void loadhs()
    {   int i,
            rc;

        try (FileInputStream fis = openFileInput("DroidArcadia.hgh"))
        {   rc = fis.read();
            if (rc == 4) // version byte (4 means V4.41+)
            {   for (i = 0; i < 51 + 6; i++)
                {   rc =  fis.read() * 65536;
                    rc += fis.read() *   256;
                    rc += fis.read();
                    hiscore[i] = rc;
                }
                // Toast.makeText(MainActivity.this, "Loaded high score table V4.", Toast.LENGTH_SHORT).show();
            } else if (rc == 3) // version byte (3 means V3.0-4.4)
            {   for (i = 0; i <= 1; i++)
                {   rc =  fis.read() * 65536;
                    rc += fis.read() *   256;
                    rc += fis.read();
                    hiscore[i] = rc;
                }
                for (i = 2; i < 50 + 6; i++)
                {   rc =  fis.read() * 65536;
                    rc += fis.read() *   256;
                    rc += fis.read();
                    hiscore[i + 1] = rc;
                }
                // Toast.makeText(MainActivity.this, "Loaded high score table V3.", Toast.LENGTH_SHORT).show();
            }
            fis.close();
            for (i = 0; i < 51 + 6; i++)
            {   sethiscore(i, hiscore[i]);
        }   }
        catch (IOException e)
        {   ; // high score table does not exist
    }   }

    private void savehs()
    {   int i;
    
        for (i = 0; i < 51 + 6; i++)
        {   hiscore[i] = gethiscore(i);
        }
        
        try (FileOutputStream fos = openFileOutput("DroidArcadia.hgh", MODE_PRIVATE);)
        {   fos.write(4); // version byte (4 means V4.41+)
            for (i = 0; i < 51 + 6; i++)
            {   fos.write( hiscore[i] / 65536);
                fos.write((hiscore[i] % 65536) / 256);
                fos.write( hiscore[i]   % 256);
            }
            // Toast.makeText(MainActivity.this, "Saved high score table.", Toast.LENGTH_SHORT).show();

            fos.close();

            for (i = 0; i < 51 + 6; i++)
            {   sethiscore(i, hiscore[i]);
        }   }
        catch (IOException e)
        {   ; // can't create/open high score table
    }   }
    
    public void jloadcos(int whichtype)
    {   String filename;
    
        if (whichtype == 0) filename = "Autosave.cos"; else filename = "Quicksave.cos";
        try (FileInputStream fis = openFileInput(filename))
        {   long   filesize = fis.available();
            byte[] allBytes = new byte[(int) filesize];

            if (fis.read(allBytes, 0, (int) filesize) != filesize)
            {   fis.close();
                Toast.makeText(MainActivity.this, getResources().getString(R.string.cantread) + " " + filename + "!", Toast.LENGTH_LONG).show();
            } else
            {   fis.close();
                int rc = cloadcos(whichtype, (int) filesize, allBytes, (whichtype == 0) ? false : true);
                if (rc != 0)
                {   Toast.makeText
                    (    MainActivity.this,
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
            {   Toast.makeText(MainActivity.this, filename + " " + getResources().getString(R.string.doesntexist) + "!", Toast.LENGTH_LONG).show();
        }   }

        whichgame = getgame();
        softkeys[key1] = softkeys[key2] = false;
        jgetoptions();
        int rc = getfirebuttons();
        int wtf;
        wtf = 0x000F; key1 = keytable_to_activity[ rc & wtf       ];
        wtf = 0x00F0; key2 = keytable_to_activity[(rc & wtf) >>  4];
        wtf = 0x0F00; key3 = keytable_to_activity[(rc & wtf) >>  8];
        wtf = 0xF000; key4 = keytable_to_activity[(rc & wtf) >> 12];
        topleft     = getdigipos(0);
        centred     = getdigipos(1);
        ox = oy = centred * 256;
        bottomright   = getdigipos(2);
        sensitivity   = getsensitivity();
        redrawkeypad  = true;
        progresswidth =
        trackerwidth  = 0;
        hardcore = jsetravars(username, password, cheevos, hardcore);
        redrawscreen();
    }

    public void dokeynames()
    {   int i;
    
        consolestring = new String[4];
        for (i = 0; i < 4; i++)
        {   consolestring[i] = new String(getkeyname(i + 12), StandardCharsets.ISO_8859_1);
        }
        button_start.setText(consolestring[0]);
        button_a.setText(    consolestring[1]);
        button_b.setText(    consolestring[2]);
        button_c.setText(    consolestring[3]);

        onebutton = (key1 == key2 && key1 == key3 && key1 == key4);

        keystring = new String[12];
        for (i = 0; i < 12; i++)
        {   keystring[i] = new String(getkeyname(i), StandardCharsets.ISO_8859_1);
    }   }

    public void jsavecos(int whichtype)
    {   String filename;
        byte   thedata[] = csavecos();
        int    filesize  = thedata.length;

        if (whichtype == 0) filename = "Autosave.cos"; else filename = "Quicksave.cos";
        try (FileOutputStream fos = openFileOutput(filename, MODE_PRIVATE))
        {   fos.write(thedata, 0, filesize);
        }
        catch (IOException e)
        {   Toast.makeText(MainActivity.this, getResources().getString(R.string.cantsave) + " " + filename + "!", Toast.LENGTH_LONG).show();
    }   }
    
    private void jsetoptions()
    {   int options1, options2;

        options1 = 0;
        if (linebased    ) options1 |=  1       ;
        if (artefacts    ) options1 |= (1 <<  1);
                           options1 |= (autofire << 2); // bits  3.. 2 (2 bits)
        if (useff        ) options1 |= (1 <<  4);
        if (ispal        ) options1 |= (1 <<  5);
        if (sound        ) options1 |= (1 <<  6);
        if (retune       ) options1 |= (1 <<  7);
        if (collisions   ) options1 |= (1 <<  8);
        if (demultiplex  ) options1 |= (1 <<  9);
        if (lives        ) options1 |= (1 << 10);
        if (time         ) options1 |= (1 << 11);
        if (invincibility) options1 |= (1 << 12);
                           options1 |= (layout      << 13); // bits 15..13 (3 bits)
                           options1 |= (volume      << 16); // bits 19..16 (4 bits)
                           options1 |= (speed       << 20); // bits 23..20 (4 bits)
        if (skies        ) options1 |= (1           << 24); // bit      24 (1 bit )
        if (spring       ) options1 |= (1           << 25); // bit      25 (1 bit )
        if (swapped      ) options1 |= (1           << 26); // bit      26 (1 bit )
        if (fill         ) options1 |= (1           << 27); // bit      27 (1 bit )
        if (dejitter     ) options1 |= (1           << 28); // bit      28 (1 bit )
                           options1 |= (colours     << 29); // bits 30..29 (2 bits)
        if (lock         ) options1 |= (1           << 31); // bit      31 (1 bit )
    
        options2 = 0;
                           options2 |= (machine          ); // bits  2.. 0 (3 bits)
                           options2 |= (memmap      <<  3); // bits  6.. 3 (4 bits)
        if (autocoin    )  options2 |= (1           <<  7); // bit       7 (1 bit )
        if (darkenbg    )  options2 |= (1           <<  8); // bit       8 (1 bit )
        if (flagline    )  options2 |= (1           <<  9); // bit       9 (1 bit )
        if (sameplayer  )  options2 |= (1           << 10); // bit      10 (1 bit )
                           options2 |= (sensitivity << 11); // bits 13..11 (3 bits)
                           
        csetoptions(options1, options2);
    }
    
    private void jgetoptions()
    {   int options,
            wtf;

        options = cgetoptions1();
        wtf =   1       ; MainActivity.linebased     = ((options & wtf) != 0); // bit       0 (1 bit )
        wtf = ( 1 <<  1); MainActivity.artefacts     = ((options & wtf) != 0); // bit       1 (1 bit )
        wtf = ( 3 <<  2); MainActivity.autofire      =  (options & wtf) >> 2 ; // bits  3.. 2 (2 bits)
        wtf = ( 1 <<  4); MainActivity.useff         = ((options & wtf) != 0);
        wtf = ( 1 <<  5); MainActivity.ispal         = ((options & wtf) != 0);
        wtf = ( 1 <<  6); MainActivity.sound         = ((options & wtf) != 0);
        wtf = ( 1 <<  7); MainActivity.retune        = ((options & wtf) != 0);
        wtf = ( 1 <<  8); MainActivity.collisions    = ((options & wtf) != 0);
        wtf = ( 1 <<  9); MainActivity.demultiplex   = ((options & wtf) != 0);
        wtf = ( 1 << 10); MainActivity.lives         = ((options & wtf) != 0);
        wtf = ( 1 << 11); MainActivity.time          = ((options & wtf) != 0);
        wtf = ( 1 << 12); MainActivity.invincibility = ((options & wtf) != 0);
        wtf = ( 7 << 13); MainActivity.layout        =  (options & wtf) >> 13; // bits 15..13 (3 bits)
        wtf = (15 << 16); MainActivity.volume        =  (options & wtf) >> 16; // bits 19..16 (4 bits)
        wtf = (15 << 20); MainActivity.speed         =  (options & wtf) >> 20; // bits 23..20 (4 bits)
        wtf = ( 1 << 24); MainActivity.skies         = ((options & wtf) != 0); // bit      24 (1 bit )
        wtf = ( 1 << 25); MainActivity.spring        = ((options & wtf) != 0); // bit      25 (1 bit )
        wtf = ( 1 << 26); MainActivity.swapped       = ((options & wtf) != 0); // bit      26 (1 bit )
        wtf = ( 1 << 27); MainActivity.fill          = ((options & wtf) != 0); // bit      27 (1 bit )
        wtf = ( 1 << 28); MainActivity.dejitter      = ((options & wtf) != 0); // bit      28 (1 bit )
        wtf = ( 3 << 29); MainActivity.colours       =  (options & wtf) >> 29; // bits 30..29 (2 bits)
        wtf = ( 1 << 31); MainActivity.lock          = ((options & wtf) != 0); // bit      31 (1 bit )

        options = cgetoptions2();
        wtf =   7       ; MainActivity.machine       =  (options & wtf)       ; // bits  2.. 0 (3 bits)
        wtf = (15 <<  3); MainActivity.memmap        =  (options & wtf) >>  3 ; // bits  6.. 3 (4 bits)
        wtf = ( 1 <<  7); MainActivity.autocoin      = ((options & wtf) !=  0); // bit       7 (1 bit )
        wtf = ( 1 <<  8); MainActivity.darkenbg      = ((options & wtf) !=  0); // bit       8 (1 bit )
        wtf = ( 1 <<  9); MainActivity.flagline      = ((options & wtf) !=  0); // bit       9 (1 bit )
        wtf = ( 1 << 10); MainActivity.sameplayer    = ((options & wtf) !=  0); // bit      10 (1 bit )
        wtf = ( 7 << 11); MainActivity.sensitivity   =  (options & wtf) >> 11 ; // bits 13..11 (3 bits)
    }

    private void jloadrom(Intent data)
    {   Uri    fileUri      = data.getData();
	    if (fileUri == null)
        {   Toast.makeText(MainActivity.this, "Can't get data!", Toast.LENGTH_LONG).show();
            return;
        }
        String filePath     = fileUri.getPath();
     // String absolutePath = FileSystems.getDefault().getPath(filePath).normalize().toAbsolutePath().toString();
        String ext          = fileUri.toString().substring(fileUri.toString().lastIndexOf('.')).toLowerCase(); // "Unsupported extension .documents/document/msf%3a21!" on Android 11?
        if (ext.equals(".pro") || ext.equals(".pgm"))
        {   Toast.makeText(MainActivity.this, "PRO/PGM files are unsupported (use TVC instead)!", Toast.LENGTH_LONG).show();
            return;
        }

        // Toast.makeText(MainActivity.this, "Loading " + filePath + "...", Toast.LENGTH_SHORT).show();
        try (InputStream inputStream = getContentResolver().openInputStream(fileUri))
        {   if (inputStream == null)
            {   Toast.makeText(MainActivity.this, "Can't open input stream!", Toast.LENGTH_LONG).show();
                return;
            }
            long   fileSize  = inputStream.available();
            byte[] allBytes  = new byte[32768];
            int    rc        = inputStream.read(allBytes);
            inputStream.close();
            if (rc == 0)
            {   Toast.makeText(MainActivity.this, filePath + " is zero-sized!", Toast.LENGTH_LONG).show();
                return;
            } else if (allBytes[0] == 'P' && allBytes[1] == 'K')
            {   // Toast.makeText(MainActivity.this, "Unzipping " + filePath + "...", Toast.LENGTH_LONG).show();
                InputStream inputStream2 = getContentResolver().openInputStream(fileUri);
                if (inputStream2 == null)
                {   Toast.makeText(MainActivity.this, "Unzip error!", Toast.LENGTH_LONG).show();
                    return;
                }
                ZipInputStream zin = new ZipInputStream(inputStream2);
                ZipEntry ze;
                boolean done = false;
                while (!done && ((ze = zin.getNextEntry()) != null))
                {   if (!ze.isDirectory())
                    {   int newSize;
                        fileSize = 0;
                        while ((newSize = zin.read(allBytes, (int) fileSize, (int) (32768 - fileSize))) != -1)
                        {   fileSize += newSize;
                        }
                        done = true;
                        // Toast.makeText(MainActivity.this, "Unzipped size of " + ze.getName() + " is " + fileSize + " bytes.", Toast.LENGTH_LONG).show();
                    }
                    zin.closeEntry();
                }
                zin.close();
                inputStream2.close();
            }
            
            if (loadgame(filePath, (int) fileSize, allBytes) != 0)
            {   Toast.makeText(MainActivity.this, "Can't parse " + filePath + "!", Toast.LENGTH_LONG).show();
                return;
            }

            whichgame = getgame();
            // Toast.makeText(MainActivity.this, "Game #" + whichgame + ".", Toast.LENGTH_SHORT).show();
                
            softkeys[key1] = false;
            jgetoptions();
            rc = getfirebuttons();
            int wtf;
            wtf = 0x000F; key1 = keytable_to_activity[ rc & wtf       ];
            wtf = 0x00F0; key2 = keytable_to_activity[(rc & wtf) >>  4];
            wtf = 0x0F00; key3 = keytable_to_activity[(rc & wtf) >>  8];
            wtf = 0xF000; key4 = keytable_to_activity[(rc & wtf) >> 12];
            topleft     = getdigipos(0);
            centred     = getdigipos(1);
            ox = oy = centred * 256;
            bottomright = getdigipos(2);
            sensitivity = getsensitivity();
            redrawkeypad = true;
            progresswidth =
            trackerwidth  = 0;
            redrawscreen();
        } catch (IOException e)
        {   Toast.makeText(MainActivity.this, getResources().getString(R.string.cantload) + " " + filePath + "!", Toast.LENGTH_LONG).show();
    }   }
    
    private void sizecontrollers()
    {   getsizes();
        LinearLayout.LayoutParams params;

        if (sizemethod == SIZE_OFF)
        {   leftvs.setVisibility(GONE);
            rightvs.setVisibility(GONE);

            params = (LinearLayout.LayoutParams) controller.getLayoutParams();
            params.setMargins(0, 0, 0, 0); // left, top, right, bottom
            controller.setLayoutParams(params);

            params = (LinearLayout.LayoutParams) screenlayout.getLayoutParams();
            params.setMargins(0, 0, 0, 0); // left, top, right, bottom
            screenlayout.setLayoutParams(params);
        } else
        {   if (sizemethod == SIZE_28MM)
            {   scaleby = 28 * mmtopx / 240;
                lefttouch.setLayoutParams(new LinearLayout.LayoutParams((int) (28 * mmtopx), (int) (28 * mmtopx)));
                righttouch.setLayoutParams(new LinearLayout.LayoutParams((int) (28 * mmtopx), (int) (28 * mmtopx)));
            } else if (sizemethod == SIZE_160DP)
            {   scaleby = 1;
                lefttouch.setLayoutParams(new LinearLayout.LayoutParams((int) (160 * dptopx), (int) (160 * dptopx)));
                righttouch.setLayoutParams(new LinearLayout.LayoutParams((int) (160 * dptopx), (int) (160 * dptopx)));
            } else if (sizemethod == SIZE_240PX)
            {   scaleby = 1;
                lefttouch.setLayoutParams(new LinearLayout.LayoutParams(240, 240));
                righttouch.setLayoutParams(new LinearLayout.LayoutParams(240, 240));
            } else if (sizemethod == SIZE_MAX)
            {   scaleby = narrowest / 480.0; // .0 is vital!
                lefttouch.setLayoutParams(new LinearLayout.LayoutParams(narrowest / 2, narrowest / 2));
                righttouch.setLayoutParams(new LinearLayout.LayoutParams(narrowest / 2, narrowest / 2));
            }

            int orientation = getResources().getConfiguration().orientation;

            params = (LinearLayout.LayoutParams) controller.getLayoutParams();
            if (orientation == Configuration.ORIENTATION_PORTRAIT)
            {   params.setMargins(0, (int) (10 * dptopx), 0, (int) (10 * dptopx)); // left, top, right, bottom
            } else
            {   params.setMargins((int) (10 * dptopx), 0, (int) (10 * dptopx), 0); // left, top, right, bottom
            }                
            controller.setLayoutParams(params);

            params = (LinearLayout.LayoutParams) screenlayout.getLayoutParams();
            if (orientation == Configuration.ORIENTATION_PORTRAIT)
            {   params.setMargins(0, 0, 0, 0); // left, top, right, bottom
            } else
            {   params.setMargins((int) (10 * dptopx), 0, (int) (10 * dptopx), 0); // left, top, right, bottom
            }                
            screenlayout.setLayoutParams(params);

            leftvs.setVisibility(VISIBLE);
            rightvs.setVisibility(VISIBLE);
        }
        // Toast.makeText(MainActivity.this, "Scale by " + String.valueOf(scaleby) + " with size method " + String.valueOf(sizemethod) + "!", Toast.LENGTH_LONG).show();
    }

    @SuppressWarnings("deprecation")
    private void getsizes()
    {   if (android.os.Build.VERSION.SDK_INT >= 30) // Android 11+
        {   WindowMetrics wm = getWindowManager().getCurrentWindowMetrics();
            Insets insets = wm.getWindowInsets().getInsetsIgnoringVisibility(WindowInsets.Type.systemBars());
            width  = wm.getBounds().width()  - insets.left - insets.right;
            height = wm.getBounds().height() - insets.top  - insets.bottom;
            dptopx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 1, getResources().getDisplayMetrics());
            mmtopx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM , 1, getResources().getDisplayMetrics());
        } else
        {   DisplayMetrics dm = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(dm); // deprecated
            height = dm.heightPixels;
            width  = dm.widthPixels;
            dptopx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 1, dm);
            mmtopx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM , 1, dm);
        }
        narrowest = Math.min(width, height);
    }

    @SuppressWarnings("deprecation")
    private void ff_init()
    {   if (android.os.Build.VERSION.SDK_INT >= 31) // Android 12+
        {   vm       = (VibratorManager) getSystemService(Context.VIBRATOR_MANAGER_SERVICE);
        } else
        {   vibrator = (Vibrator       ) getSystemService(Context.VIBRATOR_SERVICE        );
    }   }
    
    private void ff_cancel()
    {   if (android.os.Build.VERSION.SDK_INT >= 31) // Android 12+
        {   vm.cancel();
        } else
        {   vibrator.cancel(); // it is safe to cancel other vibrations currently taking place
    }   }
    
    @SuppressWarnings("deprecation")
    private static class DownloadBadgeGfxTask extends android.os.AsyncTask<String, Void, Bitmap>
    {   private final WeakReference<MainActivity> activityWeakRef;

        DownloadBadgeGfxTask(MainActivity activity)
        {   activityWeakRef = new WeakReference<>(activity);
        }

        @Override
        protected Bitmap doInBackground(String... urls)
        {   imageUrl = urls[0];

            try
            {   HttpURLConnection connection = (HttpURLConnection) new URL(imageUrl).openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream inputStream = connection.getInputStream();
                toastbitmap = BitmapFactory.decodeStream(inputStream);
            } catch (Exception e)
            {   ; // e.printStackTrace();
            }

            return toastbitmap;
        }

        @Override
        protected void onPostExecute(Bitmap result)
        {   if (result == null)
            {   return;
            }
            MainActivity activity = activityWeakRef.get();
            if (activity != null)
            {   activity.toastimage.setImageBitmap(result);
            }
            customToast.show();
    }   }
    
    @SuppressWarnings("deprecation")
    private static class DownloadProgressGfxTask extends android.os.AsyncTask<String, Void, Bitmap>
    {   private final WeakReference<MainActivity> activityWeakRef;

        DownloadProgressGfxTask(MainActivity activity)
        {   activityWeakRef = new WeakReference<>(activity);
        }

        @Override
        protected Bitmap doInBackground(String... urls)
        {   imageUrl = urls[0];

            try
            {   HttpURLConnection connection = (HttpURLConnection) new URL(imageUrl).openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream inputStream = connection.getInputStream();
                unscaledprogress = BitmapFactory.decodeStream(inputStream);
            } catch (Exception e)
            {   ; // e.printStackTrace();
            }

            return unscaledprogress;
        }

        @Override
        protected void onPostExecute(Bitmap result)
        {   if (result == null)
            {   return;
            }
            progresswidth  = unscaledprogress.getWidth()  / 2;
            progressheight = unscaledprogress.getHeight() / 2;
            scaledprogress = Bitmap.createScaledBitmap
            (   unscaledprogress,
                progresswidth,
                progressheight,
                true
            );
    }   }

    @SuppressWarnings("deprecation")
    private static class DownloadGameGfxTask extends android.os.AsyncTask<String, Void, Bitmap>
    {   private final WeakReference<MainActivity> activityWeakRef;

        DownloadGameGfxTask(MainActivity activity)
        {   activityWeakRef = new WeakReference<>(activity);
        }

        @Override
        protected Bitmap doInBackground(String... urls)
        {   imageUrl = urls[0];

            try
            {   HttpURLConnection connection = (HttpURLConnection) new URL(imageUrl).openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream inputStream = connection.getInputStream(); // waits here
                toastbitmap = BitmapFactory.decodeStream(inputStream);
            } catch (Exception e)
            {   ; // e.printStackTrace();
            }

            return toastbitmap;
        }

        @Override
        protected void onPostExecute(Bitmap result)
        {   if (result == null)
            {   return;
            }
            MainActivity activity = activityWeakRef.get();
            if (activity != null)
            {   activity.toastimage.setImageBitmap(result);
            }
            customToast.show();

            trackerwidth  = result.getWidth()  / 2;
            trackerheight = result.getHeight() / 2;
            scaledtracker = Bitmap.createScaledBitmap
            (   result,
                trackerwidth,
                trackerheight,
                true
            );
}   }   }
