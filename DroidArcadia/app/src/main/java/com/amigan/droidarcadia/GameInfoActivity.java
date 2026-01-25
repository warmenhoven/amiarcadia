package com.amigan.droidarcadia;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.net.Uri;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.core.content.ContextCompat;
import androidx.core.content.res.ResourcesCompat;

import java.nio.charset.StandardCharsets;

public class GameInfoActivity extends Activity
{   public native byte[] getautotext(int whichfield);
    public native int    getbox();
    public native int    getglyph();
    public native byte[] getgglink();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gameinfo);

        if (MainActivity.lock)
        {   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        }

        TextView  gameinfo_name      = findViewById(R.id.gameinfo_name);
        ImageView gameinfo_box       = findViewById(R.id.gameinfo_box);
        TextView  gameinfo_credits   = findViewById(R.id.gameinfo_credits);
        TextView  gameinfo_reference = findViewById(R.id.gameinfo_reference);
        TextView  gameinfo_text      = findViewById(R.id.gameinfo_text);
        TextView  button_gameguide   = findViewById(R.id.button_gameguide);

        String   thestring1         = new String(getautotext(0), StandardCharsets.ISO_8859_1);
        gameinfo_name.setText(thestring1);

        String   thestring2         = new String(getautotext(1), StandardCharsets.ISO_8859_1);
        gameinfo_credits.setText(getResources().getString(R.string.creditsyear) + ": " + thestring2);
        
        String   thestring3         = new String(getautotext(2), StandardCharsets.ISO_8859_1);
        gameinfo_reference.setText(getResources().getString(R.string.reference) + ": " + thestring3);

        String   thestring4         = new String(getautotext(3), StandardCharsets.ISO_8859_1);
        gameinfo_text.setText(thestring4);

        Drawable img;
        int      glyph = getglyph();
        switch (glyph)
        {
        case  0: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.tdattack); break;
        case  1: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.tdsoccer); break;
        case  2: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.homerun ); break;
        case  3: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.alieninv); break;
        case  4: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.astroinv); break;
        case  5: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.autorace); break;
        case  6: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.baseball); break;
        case  7: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.basketba); break;
        case  8: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.battle  ); break;
        case  9: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.blackjac); break;
        case 10: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.bowling ); break;
        case 11: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.capture ); break;
        case 12: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.cattrax ); break;
        case 13: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.circus  ); break;
        case 14: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.combat  ); break;
        case 15: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.crazycli); break;
        case 16: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.crazygob); break;
        case 17: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.dictiona); break;
        case 18: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.doraemon); break;
        case 19: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.drslump ); break;
        case 20: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.escape  ); break;
        case 21: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.football); break;
        case 22: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.frogger ); break;
        case 23: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.funkfish); break;
        case 24: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.golf    ); break;
        case 25: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.gundam  ); break;
        case 26: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.hobo    ); break;
        case 27: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.horserac); break;
        case 28: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.journey ); break;
        case 29: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.jumpbug ); break;
        case 30: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.jungler ); break;
        case 31: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.macross ); break;
        case 32: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.missilew); break;
        case 33: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.monaco  ); break;
        case 34: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.nibble  ); break;
        case 35: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.oceanbat); break;
        case 36: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.parashoo); break;
        case 37: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.pleiades); break;
        case 38: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.r2dtank ); break;
        case 39: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.redclash); break;
        case 40: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.robotkil); break;
        case 41: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.route16 ); break;
        case 42: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.soccer  ); break;
        case 43: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.sattack ); break;
        case 44: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.sbuster ); break;
        case 45: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.smission); break;
        case 46: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.sraiders); break;
        case 47: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.ssquadro); break;
        case 48: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.svulture); break;
        case 49: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.spiders ); break;
        case 50: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.superbug); break;
        case 51: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.tanksalo); break;
        case 52: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.tennis  ); break;
        case 53: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.theend  ); break;
        case 54: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.turtles ); break;
        case 55: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.boxing     ); break;
        case 56: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.brainquiz  ); break;
        case 57: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.breakaway  ); break;
        case 58: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.starchess  ); break;
        case 59: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.arcadia    ); break;
        case 60: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.interton   ); break;
        case 61: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.elektor    ); break;
        case 62: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.astrowars  ); break;
        case 63: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.galaxia    ); break;
        case 64: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.laserbattle); break;
        case 65: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.malzak     ); break;
        default: img = ContextCompat.getDrawable(GameInfoActivity.this, R.drawable.generic    ); // should never happen
        }
        if (img != null)
        {   img.setBounds(0, 0, 32, 32);
            gameinfo_name.setCompoundDrawables(img, null, null, null);
        }

        glyph = getbox();
        switch (glyph)
        {
        // Arcadia
        case  0     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_3dattack        , null); break;
        case  1     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_3dsoccer        , null); break;
        case  2     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_alieninvaders   , null); break;
        case  3     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_americanfootball, null); break;
        case  4     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_astroinvader    , null); break;
        case  5     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_autorace        , null); break;
        case  6     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_baseball        , null); break;
        case  7     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_basketball      , null); break;
        case  8     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_blackjackpoker  , null); break;
        case  9     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_3dbowling       , null); break;
        case 10     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_boxing          , null); break;
        case 11     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_brainquiz       , null); break;
        case 12     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_breakaway       , null); break;
        case 13     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_capture         , null); break;
        case 14     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_cattrax         , null); break;
        case 15     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_circus          , null); break;
        case 16     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_combat          , null); break;
        case 17     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_crazyclimber    , null); break;
        case 18     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_crazygobbler    , null); break;
        case 19     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_doraemon        , null); break;
        case 20     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_drslump         , null); break;
        case 21     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_escape          , null); break;
        case 22     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_frogger         , null); break;
        case 23     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_funkyfish       , null); break;
        case 24     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_golf            , null); break;
        case 25     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_grandslamtennis , null); break;
        case 26     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_gundam          , null); break;
        case 27     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_hobo            , null); break;
        case 28     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_horseracing     , null); break;
        case 29     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_jtron           , null); break;
        case 30     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_jumpbug         , null); break;
        case 31     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_jungler         , null); break;
        case 32     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_macross         , null); break;
        case 33     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_missilewar      , null); break;
        case 34     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_monacograndprix , null); break;
        case 35     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_nibblemen       , null); break;
        case 36     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_oceanbattle     , null); break;
        case 37     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_parashooter     , null); break;
        case 38     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_pleiades        , null); break;
        case 39     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_r2dtank         , null); break;
        case 40     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_redclash        , null); break;
        case 41     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_robotkiller     , null); break;
        case 42     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_route16         , null); break;
        case 43     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_soccer          , null); break;
        case 44     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spaceattack     , null); break;
        case 45     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spacemission    , null); break;
        case 46     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spaceraiders    , null); break;
        case 47     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spacesquadron   , null); break;
        case 48     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spacevultures   , null); break;
        case 49     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_spiders         , null); break;
        case 50     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_starchess       , null); break;
        case 51     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_superbug        , null); break;
        case 52     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_supergobbler    , null); break;
        case 53     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_tanksalot       , null); break;
        case 54     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_tetris          , null); break;
        case 55     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_theend          , null); break;
        case 56     : img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_turtlesturpin   , null); break;
        // Interton
        case 56 +  1: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_1               , null); break;
        case 56 +  2: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_2               , null); break;
        case 56 +  3: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_3               , null); break;
        case 56 +  4: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_4               , null); break;
        case 56 +  5: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_5               , null); break;
        case 56 +  6: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_6               , null); break;
        case 56 +  7: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_7               , null); break;
        case 56 +  8: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_8               , null); break;
        case 56 +  9: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_9               , null); break;
        case 56 + 10: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_10              , null); break;
        case 56 + 11: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_11              , null); break;
        case 56 + 12: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_12              , null); break;
        case 56 + 13: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_13              , null); break;
        case 56 + 14: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_14              , null); break;
        case 56 + 15: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_15              , null); break;
        case 56 + 16: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_16              , null); break;
        case 56 + 17: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_17              , null); break;
        case 56 + 18: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_18              , null); break;
        case 56 + 19: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_19              , null); break;
        case 56 + 20: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_20              , null); break;
        case 56 + 21: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_21              , null); break;
        case 56 + 22: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_22              , null); break;
        case 56 + 23: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_23              , null); break;
        case 56 + 24: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_24              , null); break;
        case 56 + 25: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_25              , null); break;
        case 56 + 26: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_26              , null); break;
        case 56 + 27: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_27              , null); break;
        case 56 + 28: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_28              , null); break;
        case 56 + 29: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_29              , null); break;
        case 56 + 30: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_30              , null); break;
        case 56 + 31: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_31              , null); break;
        case 56 + 32: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_32              , null); break;
        case 56 + 33: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_33              , null); break;
        case 56 + 34: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_36              , null); break;
        case 56 + 35: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_37              , null); break;
        case 56 + 36: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_38              , null); break;
        case 56 + 37: img = ResourcesCompat.getDrawable(getResources(), R.drawable.box_40              , null); break;
        // all
        default:      img = null;
        }
        if (img != null)
        {   gameinfo_box.setImageDrawable(img);
        }

        button_gameguide.setOnClickListener((view) ->
        {   String linkpath      = new String(getgglink(), StandardCharsets.ISO_8859_1);
            Intent gglink_intent = new Intent(Intent.ACTION_VIEW, Uri.parse(linkpath));
            startActivity(gglink_intent);
        });
}   }
