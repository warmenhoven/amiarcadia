// INCLUDES---------------------------------------------------------------

#include <windows.h>
#include "dinput.h"

#include "ibm.h"

#include <stdio.h>
#include <math.h>

#include "aa.h"

// DEFINES----------------------------------------------------------------

// #define FFVERBOSE
// whether you want diagnostic messages re. the force feedback subsystem

#define AUTOACTIVATEPADS
// whether manipulation of unassigned host gamepads by user should
// automagically assign the pad to a suitable guest player

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT struct IDirectInput*        lpdi         = NULL;
EXPORT struct IDirectInputDevice2* ffjoy2[2]    = { NULL,  NULL };
EXPORT TEXT                        joyname[2][MAX_PATH];
EXPORT int                         rumbling[2]  = {    0,     0 };
EXPORT ULONG                       softpad[2]   = {    0,     0 };
                                   
// IMPORTED VARIABLES-----------------------------------------------------

IMPORT FLAG                        unit[2];
IMPORT UBYTE                       button[2][8],
                                   jx[2], jy[2];
IMPORT ULONG                       jf[2],
                                   swapped;
IMPORT int                         hostcontroller[2],
                                   joys,
                                   machine,
                                   p1rumble,
                                   p2rumble,
                                   useff[2],
                                   whichgame;
IMPORT HINSTANCE                   InstancePtr;
IMPORT HWND                        MainWindowPtr;
IMPORT const DWORD                 joyfires[8];
IMPORT struct KnownStruct          known[KNOWNGAMES + 1];
IMPORT struct MachineStruct        machines[MACHINES];
IMPORT struct SubWindowStruct      subwin[SUBWINDOWS];

// MODULE VARIABLES-------------------------------------------------------

MODULE FLAG                        dapter[2],
                                   rumblable[2] = { FALSE, FALSE };
MODULE struct IDirectInputEffect*  ffeffect[2]  = { NULL,  NULL  };

// MODULE FUNCTIONS-------------------------------------------------------

BOOL CALLBACK DIEnumDevicesProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
MODULE void joySetForcesXY(int joy, int x, int y);
MODULE void acquire(int joy);

// CODE-------------------------------------------------------------------

EXPORT void ff_init(void)
{   HRESULT hr;

    hr = DirectInputCreate(InstancePtr, DIRECTINPUT_VERSION, &lpdi, NULL);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                        zprintf(TEXTPEN_VERBOSE, "Opened DirectInput.\n");
    acase DIERR_BETADIRECTINPUTVERSION: zprintf(TEXTPEN_ERROR,   "DirectInputCreate(): Beta version!\n");
    acase DIERR_INVALIDPARAM:           zprintf(TEXTPEN_ERROR,   "DirectInputCreate(): Invalid parameter!\n");
    acase DIERR_OLDDIRECTINPUTVERSION:  zprintf(TEXTPEN_ERROR,   "DirectInputCreate(): Old version!\n");
    acase DIERR_OUTOFMEMORY:            zprintf(TEXTPEN_ERROR,   "DirectInputCreate(): Out of memory!\n");
    adefault:                           zprintf(TEXTPEN_ERROR,   "DirectInputCreate(): Unknown error!\n");
    }
#endif
    if (hr != DI_OK)
    {   rq("Can't open DirectInput!");
}   }

EXPORT void ff_initjoys(void)
{   int i;

    joys    = 0;
    unit[0] =
    unit[1] = FALSE;
    lpdi->lpVtbl->EnumDevices
    (   lpdi,
        DIDEVTYPE_JOYSTICK,
        DIEnumDevicesProc,
        NULL,
        DIEDFL_ATTACHEDONLY
    );
    // assert(joys <= 2);

    if (joys == 0)
    {
#ifdef FFVERBOSE
        zprintf(TEXTPEN_ERROR, "No devices found!\n");
#endif
        return;
    } // implied else
#ifdef FFVERBOSE
    zprintf(TEXTPEN_VERBOSE, "%d device(s) found.\n", joys);
#endif

    for (i = 0; i < joys; i++)
    {   ff_initjoy(i);
}   }

BOOL CALLBACK DIEnumDevicesProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{   HRESULT             hr;
    LPDIRECTINPUTDEVICE ffjoy1;
    int                 i,
                        length;

#ifdef FFVERBOSE
    zprintf(TEXTPEN_VERBOSE, "Found a device.\n");
#endif

    // create game device (ie. create V1 interface)
    hr = lpdi->lpVtbl->CreateDevice(lpdi, &lpddi->guidInstance, &ffjoy1, NULL);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Created device.\n");
    acase DIERR_DEVICENOTREG:   zprintf(TEXTPEN_ERROR,   "CreateDevice(): Device not registered!\n");
    acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "CreateDevice(): Invalid parameter!\n");
    acase DIERR_NOINTERFACE:    zprintf(TEXTPEN_ERROR,   "CreateDevice(): No interface!\n");
    acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "CreateDevice(): Not initialized!\n");
    acase DIERR_OUTOFMEMORY:    zprintf(TEXTPEN_ERROR,   "CreateDevice(): Out of memory!\n");
    adefault:                   zprintf(TEXTPEN_ERROR,   "CreateDevice(): Unknown error!\n");
    }
#endif
    if (hr != DI_OK)
    {   ffjoy1->lpVtbl->Release(ffjoy1);
        return DIENUM_CONTINUE;
    }

    // get V2 interface
    hr = ffjoy1->lpVtbl->QueryInterface(ffjoy1, &IID_IDirectInputDevice2, &ffjoy2[joys]);
    // release V1 interface
    ffjoy1->lpVtbl->Release(ffjoy1);
    if (FAILED(hr))
    {
#ifdef FFVERBOSE
        zprintf(TEXTPEN_ERROR, "Failed to obtain interface!\n");
#endif
        return DIENUM_CONTINUE;
    }

    strcpy(joyname[joys], lpddi->tszProductName);
    // now remove trailing spaces
    length = strlen(joyname[joys]);
    if (length >= 1)
    {   for (i = length - 1; i >= 0; i--)
        {   if (joyname[joys][i] == ' ')
            {   joyname[joys][i] = EOS;
            } else
            {   break;
    }   }   }
    dapter[joys] = (stricmp(joyname[joys], "2600-daptor D9")) ? FALSE : TRUE;
#ifdef FFVERBOSE
    zprintf(TEXTPEN_VERBOSE, "Device name is %s.\n", joyname[joys]);
    if (dapter[joys]) zprintf(TEXTPEN_VERBOSE, "Is a dapter.\n");
    else              zprintf(TEXTPEN_VERBOSE, "Is not a dapter.\n");
#endif

    unit[joys] = TRUE;
    joys++;
    if (joys == 2)
    {   return DIENUM_STOP; // two is enough
    } else
    {   // assert(joys == 1);
        return DIENUM_CONTINUE;
}   }

EXPORT void ff_initjoy(int joy)
{   HRESULT         hr;
    DWORD           rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG            rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf = { 0 };
    DIDEVCAPS       devcaps;
    DIEFFECT        eff;
    DIPROPDWORD     DIPropAutoCenter;

    // assert(MainWindowPtr);
#ifdef FFVERBOSE
    zprintf(TEXTPEN_VERBOSE, "No window handle!\n");
#endif

    // set game data format
    hr = ffjoy2[joy]->lpVtbl->SetDataFormat(ffjoy2[joy], &c_dfDIJoystick);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Set data format.\n");
    acase DIERR_ACQUIRED:       zprintf(TEXTPEN_ERROR,   "SetDataFormat(): Already acquired!\n");
    acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "SetDataFormat(): Invalid parameter!\n");
    acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "SetDataFormat(): Not initialized!\n");
    adefault:                   zprintf(TEXTPEN_ERROR,   "SetDataFormat(): Unknown error!\n");
    }
#endif
    if (hr != DI_OK)
    {   return;
    }

    devcaps.dwSize = sizeof(DIDEVCAPS);
    hr = ffjoy2[joy]->lpVtbl->GetCapabilities(ffjoy2[joy], &devcaps);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Ascertained device capabilities.\n");
    acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "GetCapabilities(): Invalid parameter!\n");
    acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "GetCapabilities(): Not initialized!\n");
    adefault:                   zprintf(TEXTPEN_ERROR,   "GetCapabilities(): Unknown error!\n");
    }
#endif
    if (hr != DI_OK)
    {   return;
    }

    if (devcaps.dwFlags & DIDC_FORCEFEEDBACK)
    {   rumblable[joy] = TRUE;
#ifdef FFVERBOSE
        zprintf(TEXTPEN_VERBOSE, "Unit %d supports force feedback.\n", joy);
#endif
    } else
    {   rumblable[joy] = FALSE;
#ifdef FFVERBOSE
        zprintf(TEXTPEN_ERROR, "Unit %d doesn't support force feedback.\n", joy);
#endif
    }

    if (rumblable[joy])
    {   // set cooperative level
        hr = ffjoy2[joy]->lpVtbl->SetCooperativeLevel(ffjoy2[joy],
            MainWindowPtr,
            DISCL_BACKGROUND | DISCL_EXCLUSIVE // DISCL_FOREGROUND causes input loss issues
        );
#ifdef FFVERBOSE
        switch (hr)
        {
        case  DI_OK:                                    zprintf(TEXTPEN_VERBOSE, "Set cooperative level to exclusive.\n");
        acase DIERR_INVALIDPARAM:                       zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to exclusive: Invalid parameter!\n");
        acase DIERR_NOTINITIALIZED:                     zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to exclusive: Not initialized!\n");
        acase HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE): zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to exclusive: No window handle!\n");
        adefault:                                       zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to exclusive: Error $%08X!\n", hr);
        }
#endif
        if (hr != DI_OK)
        {   rumblable[joy] = FALSE;
    }   }

    if (!rumblable[joy])
    {   // set cooperative level
        hr = ffjoy2[joy]->lpVtbl->SetCooperativeLevel(ffjoy2[joy],
            MainWindowPtr,
            DISCL_BACKGROUND | DISCL_NONEXCLUSIVE // DISCL_FOREGROUND causes input loss issues
        );
#ifdef FFVERBOSE
        switch (hr)
        {
        case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Set cooperative level to shared.\n");
        acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to shared: Invalid parameter!\n");
        acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to shared: Not initialized!\n");
        adefault:                   zprintf(TEXTPEN_ERROR,   "SetCooperativeLevel() to shared: Unknown error!\n");
        }
#endif
        if (hr != DI_OK)
        {   return;
    }   }

    if (rumblable[joy])
    {   DIPropAutoCenter.diph.dwSize       = sizeof(DIPropAutoCenter);
        DIPropAutoCenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        DIPropAutoCenter.diph.dwObj        = 0;
        DIPropAutoCenter.diph.dwHow        = DIPH_DEVICE;
        DIPropAutoCenter.dwData            = 0;
        hr = ffjoy2[joy]->lpVtbl->SetProperty(ffjoy2[joy], DIPROP_AUTOCENTER, &DIPropAutoCenter.diph);
#ifdef FFVERBOSE
        switch (hr)
        {
        case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Changed device property.\n");
        acase DI_PROPNOEFFECT:      zprintf(TEXTPEN_ERROR,   "SetProperty(): No effect.\n");
        acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "SetProperty(): Invalid parameter!\n");
        acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "SetProperty(): Not initialized!\n");
        acase DIERR_OBJECTNOTFOUND: zprintf(TEXTPEN_ERROR,   "SetProperty(): Object not found!\n");
        acase DIERR_UNSUPPORTED:    zprintf(TEXTPEN_ERROR,   "SetProperty(): Unsupported property!\n");
        adefault:                   zprintf(TEXTPEN_ERROR,   "SetProperty(): Unknown error!\n");
        }
#endif
        if (hr != DI_OK)
        {   rumblable[joy] = FALSE;
    }   }

    if (rumblable[joy])
    {   eff.dwSize                  = sizeof(DIEFFECT);
        eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        eff.dwDuration              = INFINITE;
        eff.dwSamplePeriod          = 0;
        eff.dwGain                  = DI_FFNOMINALMAX;
        eff.dwTriggerButton         = DIEB_NOTRIGGER;
        eff.dwTriggerRepeatInterval = 0;
        eff.cAxes                   = 2;
        eff.rgdwAxes                = rgdwAxes;
        eff.rglDirection            = rglDirection;
        eff.lpEnvelope              = 0;
        eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
        eff.lpvTypeSpecificParams   = &cf;
        hr = ffjoy2[joy]->lpVtbl->CreateEffect(ffjoy2[joy], &GUID_ConstantForce, &eff, &ffeffect[joy], NULL);
#ifdef FFVERBOSE
        switch (hr)
        {
        case  DI_OK:                zprintf(TEXTPEN_VERBOSE, "Created effect.\n");
        acase S_FALSE:              zprintf(TEXTPEN_ERROR,   "CreateEffect(): Already created effect.\n");
        acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "CreateEffect(): Invalid parameter!\n");
        acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "CreateEffect(): Not initialized!\n");
        acase DIERR_DEVICENOTREG:   zprintf(TEXTPEN_ERROR,   "CreateEffect(): Device not registered!\n");
        acase DIERR_DEVICEFULL:     zprintf(TEXTPEN_ERROR,   "CreateEffect(): Device is full!\n");
        adefault:                   zprintf(TEXTPEN_ERROR,   "CreateEffect(): Unknown error!\n");
        }
#endif
        if (hr != DI_OK)
        {   rumblable[joy] = FALSE;
    }   }

    acquire(joy);

    if (rumblable[joy])
    {   joySetForcesXY(joy, 5000, 5000); // half power
    }
#ifdef FFVERBOSE
    else
    {   zprintf(TEXTPEN_VERBOSE, "Joystick %d is not rumblable!\n", joy);
    }
#endif
}

EXPORT void ff_ReadJoystick(UWORD joynum)
{   HRESULT    hr;
    DIJOYSTATE js;
    FLAG       ok = FALSE;
    UBYTE      oldjx, oldjy;
    ULONG      oldjf;
    int        i;

    // joynum will be '0' if they want the PRIMARY port (the FIRST detected joystick)
    // joynum will be '1' if they want the SECONDARY port (the SECOND detected joystick)

    if (joynum < joys && ffjoy2[joynum])
    {   hr = ffjoy2[joynum]->lpVtbl->Poll(ffjoy2[joynum]);
        switch (hr)
        {
        case DI_OK:
/* #ifdef FFVERBOSE
            zprintf(TEXTPEN_VERBOSE, "Polled data for unit %d.\n", joynum);
#endif */
            hr = ffjoy2[joynum]->lpVtbl->GetDeviceState(ffjoy2[joynum], sizeof(DIJOYSTATE), &js);
            switch (hr)
            {
            case  DI_OK:
                ok = TRUE;
#ifdef FFVERBOSE
                // zprintf(TEXTPEN_VERBOSE, "Read data for unit %d.\n", joynum);
            acase DIERR_INPUTLOST:      zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Input lost!\n"               , joynum);
            acase E_PENDING:            zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Data is not yet available.\n", joynum);
            acase DIERR_INVALIDPARAM:   zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Invalid parameter!\n"        , joynum);
            acase DIERR_NOTACQUIRED:    zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Not acquired!\n"             , joynum);
            acase DIERR_NOTINITIALIZED: zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Not initialized!\n"          , joynum);
            adefault:                   zprintf(TEXTPEN_ERROR,   "GetDeviceState(%d): Unknown error!\n"            , joynum);
#endif
            }
#ifdef FFVERBOSE
        acase DIERR_INPUTLOST:          zprintf(TEXTPEN_ERROR,   "Poll(%d): Input lost!\n"     , joynum);
        acase DIERR_NOTACQUIRED:        zprintf(TEXTPEN_ERROR,   "Poll(%d): Not acquired!\n"   , joynum);
        acase DIERR_NOTINITIALIZED:     zprintf(TEXTPEN_ERROR,   "Poll(%d): Not initialized!\n", joynum);
        adefault:                       zprintf(TEXTPEN_ERROR,   "Poll(%d): Unknown error!\n"  , joynum);
#endif
        }

        if (!ok)
        {   ff_off();
            ff_uninitjoys();
            ff_initjoys();
            update_joynames();
    }   }

    if (!ok)
    {   jx[joynum] =
        jy[joynum] = 128;
        jf[joynum] = softpad[joynum];
        return;
    } // implied else

    oldjx = jx[joynum];
    oldjy = jy[joynum];
    jx[joynum] = (UBYTE) (js.lX >> 8);
    jy[joynum] = (UBYTE) (js.lY >> 8);
    if (machine == ARCADIA && jx[joynum] >= 0x7E && jx[joynum] <= 0x81) // && game == ALIENINV
    {   jx[joynum] = 0x7D;
    }
    if (softpad[joynum] & JOYLEFT)
    {   if (!(softpad[joynum] & JOYRIGHT))
        {   jx[joynum] = machines[machine].digipos[0];
    }   }
    elif (softpad[joynum] & JOYRIGHT)
    {   jx[joynum] = machines[machine].digipos[2];
    }
    if (softpad[joynum] & JOYUP)
    {   if (!(softpad[joynum] & JOYDOWN))
        {   jy[joynum] = machines[machine].digipos[0];
    }   }
    elif (softpad[joynum] & JOYDOWN)
    {   jy[joynum] = machines[machine].digipos[2];
    }

    oldjf = jf[joynum];
    jf[joynum] = softpad[joynum];
    if (dapter[joynum])
    {   if ((js.rgbButtons[0] | js.rgbButtons[1] | js.rgbButtons[2]) & 0x80) // [0] is top left and top right. [1] is bottom left. [2] is bottom right.
        {   jf[joynum] |= JOYFIRE1;
        }
        if (js.rgbButtons[3] & 0x80)
        {   jf[joynum] |= JOYPAUSE;
        }
        if (js.rgbButtons[4] & 0x80)
        {   jf[joynum] |= JOYRESET;
        }
        if (js.rgbButtons[5] & 0x80) // [5] is "quit" button
        {   jf[joynum] |= JOYSTART;
        }
        for (i = 0; i <= 11; i++)
        {   if (js.rgbButtons[8 + i] & 0x80) // 8..19
            {   jf[joynum] |= 1 << i; // bits 0..11
    }   }   }
    else
    {   if (js.rgbButtons[ 0] & 0x80) jf[joynum] |= joyfires[button[joynum][0] - 1];
        if (js.rgbButtons[ 1] & 0x80) jf[joynum] |= joyfires[button[joynum][1] - 1];
        if (js.rgbButtons[ 2] & 0x80) jf[joynum] |= joyfires[button[joynum][2] - 1];
        if (js.rgbButtons[ 3] & 0x80) jf[joynum] |= joyfires[button[joynum][3] - 1];
        if (js.rgbButtons[ 4] & 0x80) jf[joynum] |= joyfires[button[joynum][4] - 1];
        if (js.rgbButtons[ 5] & 0x80) jf[joynum] |= joyfires[button[joynum][5] - 1];
        if (js.rgbButtons[ 6] & 0x80) jf[joynum] |= joyfires[button[joynum][6] - 1];
        if (js.rgbButtons[ 7] & 0x80) jf[joynum] |= joyfires[button[joynum][7] - 1];
        if (js.rgbButtons[ 8] & 0x80) jf[joynum] |= JOYRESET;
        if (js.rgbButtons[ 9] & 0x80) jf[joynum] |= JOYSTART;
        if
        (   (js.rgbButtons[10] & 0x80)
         || (js.rgbButtons[11] & 0x80)
        )
        {   jf[joynum] |= JOYFIRE1;
    }   }

#ifdef AUTOACTIVATEPADS
    if (jf[joynum])
    {   if (joynum == 0)
        {   if
            (   hostcontroller[0] != CONTROLLER_1STDJOY
             && hostcontroller[0] != CONTROLLER_1STDPAD
             && hostcontroller[0] != CONTROLLER_1STAPAD
             && hostcontroller[1] != CONTROLLER_1STDJOY
             && hostcontroller[1] != CONTROLLER_1STDPAD
             && hostcontroller[1] != CONTROLLER_1STAPAD
            )
            {   if (whichgame != -1 && known[whichgame].swapped)
                {   hostcontroller[1] = CONTROLLER_1STDJOY;
                    docommand(MENUFAKE_RIGHT);
                } else
                {   hostcontroller[0] = CONTROLLER_1STDJOY;
                    docommand(MENUFAKE_LEFT);
        }   }   }
        else
        {   // assert(joynum == 1);
            if
            (   hostcontroller[0] != CONTROLLER_2NDDJOY
             && hostcontroller[0] != CONTROLLER_2NDDPAD
             && hostcontroller[0] != CONTROLLER_2NDAPAD
             && hostcontroller[1] != CONTROLLER_2NDDJOY
             && hostcontroller[1] != CONTROLLER_2NDDPAD
             && hostcontroller[1] != CONTROLLER_2NDAPAD
            )
            {   if (whichgame != -1 && known[whichgame].swapped)
                {   hostcontroller[1] = CONTROLLER_2NDDJOY;
                    docommand(MENUFAKE_RIGHT);
                } else
                {   hostcontroller[0] = CONTROLLER_2NDDJOY;
                    docommand(MENUFAKE_LEFT);
    }   }   }   }
#endif

    if (subwin[SUBWINDOW_HOSTPADS].hwnd)
    {   if (oldjf != jf[joynum] || oldjx != jx[joynum] || oldjy != jy[joynum])
        {   DISCARD RedrawWindow(subwin[SUBWINDOW_HOSTPADS].hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}   }   }

EXPORT void ff_uninitjoys(void)
{   int i;

    for (i = 0; i < joys; i++)
    {   if (ffeffect[i])
        {   ffeffect[i]->lpVtbl->Release(ffeffect[i]);
            ffeffect[i] = NULL;
        }
        if (ffjoy2[i])
        {   if (ffjoy2[i]->lpVtbl->Unacquire(ffjoy2[i]) == DI_OK)
            {   ;
#ifdef FFVERBOSE
                zprintf(TEXTPEN_VERBOSE, "Unacquired device.\n");
#endif
            } else
            {   ;
#ifdef FFVERBOSE
                zprintf(TEXTPEN_ERROR,   "Can't unacquire device!\n");
#endif
            }
            ffjoy2[i]->lpVtbl->Release(ffjoy2[i]);
            ffjoy2[i] = NULL;
}   }   }

EXPORT void ff_die(void)
{   if (!lpdi)
    {   return;
    }

    lpdi->lpVtbl->Release(lpdi);
    lpdi = NULL;
}

EXPORT void ff_on(int joy, int amount)
{   HRESULT hr;

    if (!ffeffect[joy])
    {
#ifdef FFVERBOSE
        zprintf(TEXTPEN_VERBOSE, "No IDirectInputEffect pointer for joystick %d!\n", joy);
#endif
        return;
    }
    if (rumbling[joy])
    {
#ifdef FFVERBOSE
        zprintf(TEXTPEN_VERBOSE, "Joystick %d is not rumbling!\n", joy);
#endif
        return;
    }

    hr = ffeffect[joy]->lpVtbl->Start(ffeffect[joy], 1, 0);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                      zprintf(TEXTPEN_VERBOSE, "Started effect.\n");
    acase DIERR_INVALIDPARAM:         zprintf(TEXTPEN_ERROR,   "Start(): Invalid parameter!\n");
    acase DIERR_INCOMPLETEEFFECT:     zprintf(TEXTPEN_ERROR,   "Start(): Incomplete effect!\n");
    acase DIERR_NOTEXCLUSIVEACQUIRED: zprintf(TEXTPEN_ERROR,   "Start(): Not exclusively acquired!\n");
    acase DIERR_NOTINITIALIZED:       zprintf(TEXTPEN_ERROR,   "Start(): Not initialized!\n");
    acase DIERR_UNSUPPORTED:          zprintf(TEXTPEN_ERROR,   "Start(): Unsupported!\n");
    adefault:                         zprintf(TEXTPEN_ERROR,   "Start(): Unknown error!\n");
    }
#endif
    if (hr == DI_OK)
    {   rumbling[joy] = amount;
}   }

EXPORT void ff_off(void)
{   int i;

    for (i = 0; i < 2; i++)
    {   if (rumbling[i])
        {   rumbling[i] = 0;
            DISCARD ffeffect[i]->lpVtbl->Stop(ffeffect[i]);
}   }   }

EXPORT void ff_decay(int joy)
{   HRESULT hr;

    if (!ffeffect[joy] || !rumblable[joy] || !rumbling[joy])
    {   return;
    }

    rumbling[joy]--;
    if (rumbling[joy])
    {   return;
    }

    hr = ffeffect[joy]->lpVtbl->Stop(ffeffect[joy]);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                      zprintf(TEXTPEN_VERBOSE, "Stopped effect.\n");
    acase DIERR_NOTEXCLUSIVEACQUIRED: zprintf(TEXTPEN_ERROR,   "Stop(): Not exclusively acquired!\n");
    acase DIERR_NOTINITIALIZED:       zprintf(TEXTPEN_ERROR,   "Stop(): Not initialized!\n");
    adefault:                         zprintf(TEXTPEN_ERROR,   "Stop(): Unknown error!\n");
    }
#endif
}

MODULE void joySetForcesXY(int joy, int x, int y)
{   HRESULT         hr;
    LONG            rglDirection[2] = { x, y };
    DICONSTANTFORCE cf;
    DIEFFECT        eff;

    cf.lMagnitude = (DWORD) sqrt(  ((double) x * (double) x)
                                 + ((double) y * (double) y)
                                );
    eff.dwSize                = sizeof(DIEFFECT);
    eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.cAxes                 = 2;
    eff.rglDirection          = rglDirection;
    eff.lpEnvelope            = 0;
    eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;
    hr = ffeffect[joy]->lpVtbl->SetParameters(ffeffect[joy],
        &eff,
        DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS
    );
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                    zprintf(TEXTPEN_VERBOSE, "Set forces.\n");
    acase DI_EFFECTRESTARTED:       zprintf(TEXTPEN_ERROR,   "SetParameters(): Effect restarted.\n");
    acase DI_DOWNLOADSKIPPED:       zprintf(TEXTPEN_ERROR,   "SetParameters(): Download skipped.\n");
    acase DI_TRUNCATED:             zprintf(TEXTPEN_ERROR,   "SetParameters(): Truncated.\n");
    acase DI_TRUNCATEDANDRESTARTED: zprintf(TEXTPEN_ERROR,   "SetParameters(): Truncated and restarted.\n");
    acase DIERR_INCOMPLETEEFFECT:   zprintf(TEXTPEN_ERROR,   "SetParameters(): Incomplete effect!\n");
    acase DIERR_NOTINITIALIZED:     zprintf(TEXTPEN_ERROR,   "SetParameters(): Not initialized!\n");
    acase DIERR_INVALIDPARAM:       zprintf(TEXTPEN_ERROR,   "SetParameters(): Invalid parameter!\n");
    acase DIERR_EFFECTPLAYING:      zprintf(TEXTPEN_ERROR,   "SetParameters(): Effect is already playing!\n");
    acase DIERR_INPUTLOST:          zprintf(TEXTPEN_ERROR,   "SetParameters(): Input lost!\n"); // acquire(joy);
    adefault:                       zprintf(TEXTPEN_ERROR,   "SetParameters(): Unknown error!\n");
    }
#endif
}

MODULE void acquire(int joy)
{   HRESULT hr;

    hr = ffjoy2[joy]->lpVtbl->Acquire(ffjoy2[joy]);
#ifdef FFVERBOSE
    switch (hr)
    {
    case  DI_OK:                 zprintf(TEXTPEN_VERBOSE, "Acquired device.\n");
    acase S_FALSE:               zprintf(TEXTPEN_ERROR,   "Acquire(): Already acquired device!\n");
    acase DIERR_INVALIDPARAM:    zprintf(TEXTPEN_ERROR,   "Acquire(): Invalid parameter!\n");
    acase DIERR_NOTINITIALIZED:  zprintf(TEXTPEN_ERROR,   "Acquire(): Not initialized!\n");
    acase DIERR_OTHERAPPHASPRIO: zprintf(TEXTPEN_ERROR,   "Acquire(): Another application has priority!\n");
    adefault:                    zprintf(TEXTPEN_ERROR,   "Acquire(): Unknown error!\n");
    }
#endif
}
