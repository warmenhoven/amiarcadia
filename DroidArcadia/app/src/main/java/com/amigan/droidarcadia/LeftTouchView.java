package com.amigan.droidarcadia;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.SparseArray;
import android.view.MotionEvent;
import android.view.View;

public class LeftTouchView extends View
{   private SparseArray<PointF> mActivePointers;
    private Paint    mp, // main paint
                     tp; // text paint
    private int      width, nwidth,
                     height,
                     qi;
    private float    widthpart,
                     heightpart,
                     centrex, centrey, keytextx, keytexty;
    private final Rect  r = new Rect();
    private       int   qtrwidth,  qtrheight,
                        nqtrwidth, nqtrheight;
    private final int   verticesSoftColors[] = { Color.WHITE,  Color.WHITE,  Color.WHITE,
                                                 0xFF000000,  0xFF000000,  0xFF000000 };
    private final int   verticesHardColors[] = { Color.YELLOW, Color.YELLOW, Color.YELLOW,
                                                 0xFF000000,  0xFF000000,  0xFF000000 };
    private       int[] verticesColors;
    private final float verts[]          = { 0, 0, 0, 0, 0, 0 };

    // constants
    public  static int MEMMAP_LASERBATTLE = 11,
                       MEMMAP_LAZARIAN    = 12;

    public LeftTouchView(Context context, AttributeSet attrs)
    {   super(context, attrs);
        initView();
    }

    private void initView()
    {   mActivePointers = new SparseArray<>();
        mp              = new Paint(Paint.ANTI_ALIAS_FLAG);
        tp              = new Paint(Paint.ANTI_ALIAS_FLAG);
        tp.setColor(Color.BLACK);
        tp.setTextAlign(Paint.Align.LEFT); // probably not needed?
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {   int pointerIndex = event.getActionIndex();           // get pointer index from the event object
        int pointerId    = event.getPointerId(pointerIndex); // get pointer ID
        int maskedAction = event.getActionMasked();          // get masked (not specific to a pointer) action

        switch (maskedAction)
        {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN:
        {   // add new pointer to list of pointers

            PointF f = new PointF();
            f.x = event.getX(pointerIndex);
            f.y = event.getY(pointerIndex);
            mActivePointers.put(pointerId, f);

            if (!MainActivity.handedness)
            {   checkfingers(1); // left is paddle
            } else
            {   checkfingers(0); // left is keypad
            }
            
            break;
        }
        case MotionEvent.ACTION_MOVE: // a pointer was moved
        {   for (int size = event.getPointerCount(), i = 0; i < size; i++)
            {   PointF point = mActivePointers.get(event.getPointerId(i));
                if (point != null)
                {   point.x = event.getX(i);
                    point.y = event.getY(i);
            }   }
            
            if (!MainActivity.handedness)
            {   checkfingers(1); // left is paddle
            } else
            {   checkfingers(0); // left is keypad
            }
            
            break;
        }
        case MotionEvent.ACTION_UP:
            MainActivity.hardx = MainActivity.softx;
            MainActivity.hardy = MainActivity.softy;
            performClick();
        case MotionEvent.ACTION_POINTER_UP:
        case MotionEvent.ACTION_CANCEL:
        {   mActivePointers.remove(pointerId);
            
            if (!MainActivity.handedness)
            {   checkfingers(1); // left is paddle
            } else
            {   checkfingers(0); // left is keypad
            }
            
            break;
        }
        }
        invalidate();

        return true;
    }

    @Override
    public boolean performClick()
    {   super.performClick();
        return true;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {   width                   = w;
        height                  = h;

        // keypad
        MainActivity.eachwidth  = width  / 3;
        MainActivity.eachheight = height / 4;
        widthpart               = width  / 6.0f;
        heightpart              = height / 8.0f;

        // paddle
        qtrwidth                = width  / 4;
        qtrheight               = height / 4;
        
        super.onSizeChanged(w, h, oldw, oldh);
    }

    @Override
    protected void onDraw(Canvas canvas)
    {   super.onDraw(canvas);

        if (MainActivity.handedness) // left is keypad
        {   for (qi = 0; qi < 12; qi++)
            {   if (MainActivity.onebutton && qi != MainActivity.key1)
                {   continue;
                }

                if (MainActivity.hardkeys[qi] && MainActivity.sameplayer)
                {   mp.setColor(Color.YELLOW);
                } else if (MainActivity.softkeys[qi])
                {   mp.setColor(Color.WHITE);
                } else if
                (   MainActivity.autofire >= 1
                 && (   (qi == MainActivity.key1 &&  MainActivity.memmap != MEMMAP_LASERBATTLE && MainActivity.memmap != MEMMAP_LAZARIAN )
                     || (qi == MainActivity.key2 &&  MainActivity.whichgame == MainActivity._3DATTACKPOS                                 )
                     || (qi == MainActivity.key4 && (MainActivity.memmap == MEMMAP_LASERBATTLE || MainActivity.memmap == MEMMAP_LAZARIAN))
                )   )
                {   mp.setColor(Color.GREEN);
                } else if (!MainActivity.onebutton && qi % 2 == 1)
                {   if (MainActivity.swapped)
                    {   mp.setColor(Color.argb(255, 170 - 34, 170 - 34, 255 - 34));
                    } else
                    {   mp.setColor(Color.argb(255, 255 - 34, 136 - 34, 136 - 34));
                }   }
                else
                {   if (MainActivity.swapped)
                    {   mp.setColor(Color.argb(255, 170     , 170     , 255     ));
                    } else
                    {   mp.setColor(Color.argb(255, 255     , 136     , 136     ));
                }   }

                if (MainActivity.onebutton && qi == MainActivity.key1)
                {   canvas.drawRect
                    (   0,
                        0,
                        width,
                        height,
                        mp
                    );
                } else
                {   canvas.drawRect
                    (   (qi % 3) * MainActivity.eachwidth,
                        (qi / 3) * MainActivity.eachheight,
                        ((qi % 3) + 1) * MainActivity.eachwidth,
                        ((qi / 3) + 1) * MainActivity.eachheight,
                        mp
                    );
            }   }

            if (!MainActivity.onebutton)
            {   mp.setColor(Color.parseColor("#444455"));
                canvas.drawLine(MainActivity.eachwidth     ,                           0, MainActivity.eachwidth    ,                  height    , mp); // 1st vertical   line
                canvas.drawLine(MainActivity.eachwidth  * 2,                           0, MainActivity.eachwidth * 2,                  height    , mp); // 2nd vertical   line
                canvas.drawLine(                          0, MainActivity.eachheight    ,                  width    , MainActivity.eachheight    , mp); // 1st horizontal line
                canvas.drawLine(                          0, MainActivity.eachheight * 2,                  width    , MainActivity.eachheight * 2, mp); // 2nd horizontal line
                canvas.drawLine(                          0, MainActivity.eachheight * 3,                  width    , MainActivity.eachheight * 3, mp); // 3rd horizontal line
            }

            for (qi = 0; qi < 12; qi++)
            {   if (MainActivity.onebutton && qi != MainActivity.key1)
                {   continue;
                }

                if (MainActivity.onebutton)
                {   // assert(qi == MainActivity.key1);
                    tp.setTextSize((float) MainActivity.scaleby * (50 - (MainActivity.keystring[qi].length() * 2))); // 50..40 for lengths 1..6
                    centrex = width  / 2;
                    centrey = height / 2;
                } else
                {   tp.setTextSize((float) MainActivity.scaleby *  25);
                    centrex = widthpart  * (((qi % 3) * 2) + 1);
                    centrey = heightpart * (((qi / 3) * 2) + 1);
                }
                tp.getTextBounds(MainActivity.keystring[qi], 0, MainActivity.keystring[qi].length(), r);
                keytextx = centrex - (r.width()  / 2f) - r.left;
                keytexty = centrey - (r.height() / 2f) - r.top;

                canvas.drawText
                (   MainActivity.keystring[qi],
                    keytextx,
                    keytexty,
                    tp
                );
        }   }
        else // left is paddle
        {   float thepointx, thepointy;
    
            if (MainActivity.layout == MainActivity.WAYS_2H)
            {   mp.setColor(Color.GRAY);
                canvas.drawLine(qtrwidth * 2,             0, qtrwidth * 2,    height    , mp);
            } else if (MainActivity.layout == MainActivity.WAYS_2V)
            {   mp.setColor(Color.GRAY);
                canvas.drawLine(           0, qtrheight * 2,    width    , qtrheight * 2, mp);
            } else if (MainActivity.layout == MainActivity.WAYS_4O)
            {   mp.setColor(Color.GRAY);
                canvas.drawLine(           0,             0,    width    ,    height    , mp); // backslash
                canvas.drawLine(   width    ,             0,            0,    height    , mp); // forward slash
            } else if (MainActivity.layout == MainActivity.WAYS_4D)
            {   mp.setColor(Color.GRAY);
                canvas.drawLine(qtrwidth * 2,             0, qtrwidth * 2,    height    , mp);
                canvas.drawLine(           0, qtrheight * 2,    width    , qtrheight * 2, mp);
            } else if (MainActivity.layout == MainActivity.WAYS_8)
            {   mp.setColor(Color.GRAY);
                canvas.drawLine(qtrwidth    ,             0, qtrwidth * 3,    height    , mp);
                canvas.drawLine(qtrwidth * 3,             0, qtrwidth    ,    height    , mp);
                canvas.drawLine(           0, qtrheight    ,    width    , qtrheight * 3, mp);
                canvas.drawLine(           0, qtrheight * 3,    width    , qtrheight    , mp);
            }

            if (MainActivity.layout == MainActivity.WAYS_ANALOG)
            {   if (MainActivity.sameplayer && (MainActivity.hardxmoved || MainActivity.hardymoved))
                {   mp.setColor(Color.YELLOW);
                    thepointx = (float) MainActivity.hardx;
                    thepointy = (float) MainActivity.hardy;
                } else if (MainActivity.softmoved)
                {   mp.setColor(Color.WHITE);
                    thepointx = (float) MainActivity.softx;
                    thepointy = (float) MainActivity.softy;
                } else
                {   if (MainActivity.swapped)
                    {   mp.setColor(Color.argb(255, 170, 170, 255)); // blue
                    } else
                    {   mp.setColor(Color.argb(255, 255, 136, 136)); // red
                    }
                    if (MainActivity.spring)
                    {   thepointx = (float) MainActivity.centred * 256;
                        thepointy = (float) MainActivity.centred * 256;
                    } else
                    {   thepointx = (float) MainActivity.ox;
                        thepointy = (float) MainActivity.oy;
                }   }
                thepointx /= (float) (65535.0 / width ); // scale 0..65535 to 0..width
                thepointy /= (float) (65535.0 / height); // scale 0..65535 to 0..height
                canvas.drawCircle(thepointx, thepointy, width / 10, mp);
                canvas.drawLine(thepointx, thepointy, width / 2, height / 2, mp);
            } else if
            (   MainActivity.softmoved
             || (MainActivity.sameplayer && (MainActivity.hardxmoved || MainActivity.hardymoved))
            )
            {   if (MainActivity.sameplayer && (MainActivity.hardxmoved || MainActivity.hardymoved))
                {   if
                    (   (MainActivity.layout == MainActivity.WAYS_2H && MainActivity.hardx == MainActivity.centred * 256)
                     || (MainActivity.layout == MainActivity.WAYS_2V && MainActivity.hardy == MainActivity.centred * 256)
                    )
                    {   return;
                    } // implied else
                    verticesColors = verticesHardColors;
                    nwidth = 65535;
                    nqtrwidth = nqtrheight = 16384;
                    digitizecoords(MainActivity.hardx, MainActivity.hardy);
                } else
                {   // assert(MainActivity.softmoved);
                    verticesColors = verticesSoftColors;
                }
                setLayerType(View.LAYER_TYPE_SOFTWARE, null);
                if (MainActivity.layout == MainActivity.WAYS_8)
                {   if (MainActivity.hx == MainActivity.topleft)
                    {   if (MainActivity.hy == MainActivity.topleft) // top left
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] = qtrwidth    ; verts[3] =             0;
                            verts[4] =            0; verts[5] = qtrheight;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] =            0; verts[1] =             0;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.centred) // left
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] =            0; verts[3] = qtrheight;
                            verts[4] =            0; verts[5] = qtrheight * 3;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.bottomright) // bottom left
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] =            0; verts[3] = qtrheight * 3;
                            verts[4] = qtrwidth    ; verts[5] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] =            0; verts[1] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    }   }
                    else if (MainActivity.hx == MainActivity.centred)
                    {   if (MainActivity.hy == MainActivity.topleft) // top
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] = qtrwidth    ; verts[3] =             0;
                            verts[4] = qtrwidth * 3; verts[5] =             0;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.bottomright) // bottom
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] = qtrwidth    ; verts[3] =    height    ;
                            verts[4] = qtrwidth * 3; verts[5] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    }   }
                    else if (MainActivity.hx == MainActivity.bottomright)
                    {   if (MainActivity.hy == MainActivity.topleft) // top right
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] = qtrwidth * 3; verts[3] =             0;
                            verts[4] =    width    ; verts[5] = qtrheight;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] = width    ; verts[1] =   0;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.centred) // right
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] =    width    ; verts[3] = qtrheight;
                            verts[4] =    width    ; verts[5] = qtrheight * 3;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.bottomright) // bottom right
                        {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                            verts[2] =    width    ; verts[3] = qtrheight * 3;
                            verts[4] = qtrwidth * 3; verts[5] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] =    width    ; verts[1] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                }   }   }
                else if (MainActivity.layout == MainActivity.WAYS_4O)
                {   if (MainActivity.hy == MainActivity.centred)
                    {   if (MainActivity.hx == MainActivity.topleft) // left
                        {   verts[0] =            0; verts[1] =             0;
                            verts[2] =            0; verts[3] =    height    ;
                            verts[4] = qtrwidth * 2; verts[5] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hx == MainActivity.bottomright) // right
                        {   verts[0] =    width    ; verts[1] =             0;
                            verts[2] =    width    ; verts[3] =    height    ;
                            verts[4] = qtrwidth * 2; verts[5] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    }   }
                    else if (MainActivity.hx == MainActivity.centred)
                    {   if (MainActivity.hy == MainActivity.topleft) // top
                        {   verts[0] =            0; verts[1] =             0;
                            verts[2] =    width    ; verts[3] =             0;
                            verts[4] = qtrwidth * 2; verts[5] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.bottomright) // bottom
                        {   verts[0] =            0; verts[1] =    height    ;
                            verts[2] =    width    ; verts[3] =    height    ;
                            verts[4] = qtrwidth * 2; verts[5] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                }   }   }
                else if (MainActivity.layout == MainActivity.WAYS_4D)
                {   if (MainActivity.hx == MainActivity.topleft && MainActivity.hy == MainActivity.topleft) // top left
                    {   verts[0] =            0; verts[1] =             0;
                        verts[2] =            0; verts[3] = qtrheight * 2;
                        verts[4] = qtrwidth * 2; verts[5] = qtrheight * 2;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        verts[2] = qtrwidth * 2; verts[3] =             0;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    } else if (MainActivity.hx == MainActivity.topleft && MainActivity.hy == MainActivity.bottomright) // bottom left
                    {   verts[0] =            0; verts[1] = qtrheight * 2;
                        verts[2] =            0; verts[3] =    height    ;
                        verts[4] = qtrwidth * 2; verts[5] =    height    ;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        verts[2] = qtrwidth * 2; verts[3] = qtrheight * 2;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    } else if (MainActivity.hx == MainActivity.bottomright && MainActivity.hy == MainActivity.topleft) // top right
                    {   verts[0] = qtrwidth * 2; verts[1] =             0;
                        verts[2] = qtrwidth * 2; verts[3] = qtrheight * 2;
                        verts[4] =    width    ; verts[5] = qtrheight * 2;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        verts[2] =    width    ; verts[3] =             0;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                    } else if (MainActivity.hx == MainActivity.bottomright && MainActivity.hy == MainActivity.bottomright) // bottom right
                    {   verts[0] = qtrwidth * 2; verts[1] = qtrheight * 2;
                        verts[2] = qtrwidth * 2; verts[3] =    height    ;
                        verts[4] =    width    ; verts[5] =    height    ;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        verts[2] =    width    ; verts[3] = qtrheight * 2;
                        canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                }   }
                else if (MainActivity.layout == MainActivity.WAYS_2H)
                {   if (MainActivity.hy == MainActivity.centred)
                    {   if (MainActivity.hx == MainActivity.topleft) // left
                        {   verts[0] =            0; verts[1] =          0;
                            verts[2] = qtrwidth * 2; verts[3] =          0;
                            verts[4] =            0; verts[5] = height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] = qtrwidth * 2; verts[1] = height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hx == MainActivity.bottomright) // right
                        {   verts[0] = qtrwidth * 2; verts[1] =          0;
                            verts[2] =    width    ; verts[3] =          0;
                            verts[4] = qtrwidth * 2; verts[5] = height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] =    width    ; verts[1] = height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                }   }   }
                else if (MainActivity.layout == MainActivity.WAYS_2V)
                {   if (MainActivity.hx == MainActivity.centred)
                    {   if (MainActivity.hy == MainActivity.topleft) // top
                        {   verts[0] =         0; verts[1] =             0;
                            verts[2] = width    ; verts[3] =             0;
                            verts[4] =         0; verts[5] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] = width    ; verts[1] = qtrheight * 2;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                        } else if (MainActivity.hy == MainActivity.bottomright) // bottom
                        {   verts[0] =         0; verts[1] = qtrheight * 2;
                            verts[2] = width    ; verts[3] = qtrheight * 2;
                            verts[4] =         0; verts[5] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                            verts[0] = width    ; verts[1] =    height    ;
                            canvas.drawVertices(Canvas.VertexMode.TRIANGLES, 6, verts, 0, null, 0, verticesColors, 0, null, 0, 0, mp);
                }   }   }

                // canvas.drawText(String.valueOf(MainActivity.hx) + "," + String.valueOf(MainActivity.hy), 10, 30, tp);
    }   }   }

    private void checkfingers(int mode)
    {   int    i, whichkey, x, y;
        int    size = mActivePointers.size();
        PointF point;

        if (mode == 0) // keypad
        {   for (i = 0; i < 12; i++)
            {   MainActivity.softkeys[i] = false;
            }

            for (i = 0; i < size; i++)
            {   point = mActivePointers.valueAt(i);
                if (point != null)
                {   x = (int) point.x;
                    y = (int) point.y;
                    if (x >= 0 && y >= 0 && x / MainActivity.eachwidth < 3 && y / MainActivity.eachheight < 4) // important!
                    {   whichkey = (x / MainActivity.eachwidth) + ((y / MainActivity.eachheight) * 3);
                        if (MainActivity.onebutton)
                        {   MainActivity.softkeys[MainActivity.key1] = true;
                        } else
                        {   MainActivity.softkeys[whichkey] = true;
        }   }   }   }   }
        else
        {   MainActivity.softmoved = false;
            MainActivity.hx = MainActivity.hy = -1;
        
            for (i = 0; i < size; i++)
            {   point = mActivePointers.valueAt(i);
                if (point != null)
                {   x = (int) point.x;
                    y = (int) point.y;
                    MainActivity.softmoved = true;
                    if (MainActivity.layout == MainActivity.WAYS_ANALOG)
                    {   MainActivity.softx = (65535 * x) / width;
                        MainActivity.softy = (65535 * y) / height;
                    } else
                    {   nwidth     = width;
                        nqtrwidth  = qtrwidth;
                        nqtrheight = qtrheight;
                        digitizecoords(x, y);
                        MainActivity.softx = MainActivity.hx * 256;
                        MainActivity.softy = MainActivity.hy * 256;
                    }
                    if      (MainActivity.softx <     0) MainActivity.softx =     0;
                    else if (MainActivity.softx > 65535) MainActivity.softx = 65535;
                    if      (MainActivity.softy <     0) MainActivity.softy =     0;
                    else if (MainActivity.softy > 65535) MainActivity.softy = 65535;
    }   }   }   }

    private void digitizecoords(int x, int y)
    {   if (MainActivity.layout == MainActivity.WAYS_8)
        {   if (x >= nqtrwidth + (y / 2)) // right of line 1
            {   if (y >= nqtrwidth * 3 - (x / 2)) // bottom of line 3
                {   MainActivity.hx = MainActivity.bottomright;
                    if (y >= nqtrwidth + (x / 2)) // bottom of line 4
                    {   MainActivity.hy = MainActivity.bottomright; // bottom right
                    } else // top of line 4
                    {   MainActivity.hy = MainActivity.centred;     // right
                }   }
                else // top of line 4
                {   MainActivity.hy = MainActivity.topleft;
                    if (x >= nqtrwidth * 3 - (y / 2)) // right of line 2
                    {   MainActivity.hx = MainActivity.bottomright; // top right
                    } else // left of line 2
                    {   MainActivity.hx = MainActivity.centred;     // top
            }   }   }
            else // left of line 1
            {   if (y >= nqtrwidth * 3 - (x / 2)) // bottom of line 3
                {   MainActivity.hy = MainActivity.bottomright;
                    if (x >= nqtrwidth * 3 - (y / 2)) // right of line 2
                    {   MainActivity.hx = MainActivity.centred; // bottom
                    } else // left of line 2
                    {   MainActivity.hx = MainActivity.topleft; // bottom left
                }   }
                else // top of line 3
                {   MainActivity.hx = MainActivity.topleft;
                    if (y >= nqtrwidth + (x / 2)) // bottom of line 4
                    {   MainActivity.hy = MainActivity.centred; // left
                    } else // top of line 4
                    {   MainActivity.hy = MainActivity.topleft; // top left
        }   }   }   }
        else if (MainActivity.layout == MainActivity.WAYS_4O)
        {   if (x > y) // top right side
            {   if (x > nwidth     - y) // right side
                {   MainActivity.hx = MainActivity.bottomright; MainActivity.hy = MainActivity.centred;
                } else // top side
                {   MainActivity.hx = MainActivity.centred;     MainActivity.hy = MainActivity.topleft;
            }   }
            else // bottom left side
            {   if (x > nwidth     - y) // bottom side
                {   MainActivity.hx = MainActivity.centred;     MainActivity.hy = MainActivity.bottomright;
                } else // left side
                {   MainActivity.hx = MainActivity.topleft;     MainActivity.hy = MainActivity.centred;
        }   }   }
        else if (MainActivity.layout == MainActivity.WAYS_4D)
        {   if (x < nqtrwidth * 2)
            {   if (y < nqtrheight * 2) // top left
                {   MainActivity.hx = MainActivity.hy = MainActivity.topleft;
                } else // bottom left
                {   MainActivity.hx = MainActivity.topleft;
                    MainActivity.hy = MainActivity.bottomright;
            }   }
            else
            {   if (y < nqtrheight * 2) // top right
                {   MainActivity.hx = MainActivity.bottomright;
                    MainActivity.hy = MainActivity.topleft;
                } else // bottom right
                {   MainActivity.hx = MainActivity.hy = MainActivity.bottomright;
        }   }   }
        else if (MainActivity.layout == MainActivity.WAYS_2H)
        {   if (x < nqtrwidth * 2)
            {   MainActivity.hx = MainActivity.topleft;
            } else
            {   MainActivity.hx = MainActivity.bottomright;
            }
            MainActivity.hy    = MainActivity.centred;
        } else if (MainActivity.layout == MainActivity.WAYS_2V)
        {   MainActivity.hx    = MainActivity.centred;
            if (y < nqtrheight * 2)
            {   MainActivity.hy = MainActivity.topleft;
            } else
            {   MainActivity.hy = MainActivity.bottomright;
}   }   }   }
