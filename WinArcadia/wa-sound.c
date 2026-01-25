// 1. INCLUDES -----------------------------------------------------------

#include "ibm.h"
#include "sapi.h"

#include "resource.h"

#include <stdio.h>

#define EXEC_TYPES_H
#include "aa.h"

// 2. DEFINES ------------------------------------------------------------

// #define REGDB_E_CLASSNOTREG   0x80040154 (already #defined in winerror.h)
#define SPERR_UNSUPPORTED_FORMAT 0x80045003
#define SPERR_NOT_FOUND          0x8004503A

// 3. EXPORTED VARIABLES -------------------------------------------------

EXPORT       FLAG                    ChannelOpened[TOTALCHANNELS];
EXPORT       UBYTE                   SilenceBuffer8[SILENCEBUFSIZE],
                                     SilenceBuffer16[SILENCEBUFSIZE * 2];
EXPORT       int                     speakable = FALSE;

// 4. IMPORTED VARIABLES -------------------------------------------------

IMPORT       FLAG                    guestplaying[TOTALCHANNELS],
                                     timeout;
IMPORT       UBYTE                   SoundBuffer[GUESTCHANNELS][SOUNDLENGTH * 2];
IMPORT       ULONG                   cycles_2650,
                                     paused,
                                     region,
                                     sound;
IMPORT       int                     ambient,
                                     bitrate,
                                     divideby,
                                     esvxfile,
                                     hostvolume,
                                     language,
                                     machine,
                                     memmap,
                                     recmode,
                                     samplerate,
                                     SoundLength[GUESTCHANNELS],
                                     usespeech;
IMPORT       WCHAR                   voicename[80 + 1];
IMPORT       FILE*                   WAVHandle[TOTALCHANNELS];
IMPORT       struct DriveStruct      drive[DRIVES_MAX];
IMPORT       struct LangStruct       langs[LANGUAGES];
IMPORT       struct MachineStruct    machines[MACHINES];
IMPORT const FLAG                    loopable[TOTALCHANNELS];

// 5. MODULE VARIABLES ---------------------------------------------------

MODULE       HWAVEOUT                hChannel[TOTALCHANNELS];
MODULE       WAVEHDR                 ChannelHdr[TOTALCHANNELS];
MODULE       ISpVoice*               gpIVTxt   = NULL;

MODULE const DWORD convvolume[8 + 1] = { 0x00000000,
                                         0x20002000,
                                         0x40004000,
                                         0x60006000,
                                         0x80008000,
                                         0xA000A000,
                                         0xC000C000,
                                         0xE000E000,
                                         0xFFFFFFFF };

// 6. STRUCTURES ---------------------------------------------------------

EXPORT struct SampleStruct samp[SAMPLES] =
{ {"sounds\\explode.wav" },
  {"sounds\\shoot.wav"   },
  {"sounds\\bang.wav"    },
  {"sounds\\coin.wav"    },
  {"sounds\\printer.wav" },
  {"sounds\\click.wav"   },
  {"sounds\\spin.wav"    },
  {"sounds\\step.wav"    },
  {"sounds\\grind.wav"   },
  {"sounds\\punch.wav"   },
};

/* 7. MODULE FUNCTIONS ---------------------------------------------------

(None)

8. CODE --------------------------------------------------------------- */

EXPORT void open_channels(int howmany)
{   WAVEFORMATEX l_WaveFormatEx;
    int          i;

    for (i = 0; i < howmany; i++)
    {   if (i >= GUESTCHANNELS)
        {   if (!samp[i - GUESTCHANNELS].filebase)
            {   continue;
            }

            // If samples aren't at 8-bit 11025Hz, they will play fine but won't record correctly.
            l_WaveFormatEx.nSamplesPerSec  =  samp[i - GUESTCHANNELS].filebase[24]
                                           + (samp[i - GUESTCHANNELS].filebase[25] *      256)
                                           + (samp[i - GUESTCHANNELS].filebase[26] *    65536)
                                           + (samp[i - GUESTCHANNELS].filebase[27] * 16777216);
            l_WaveFormatEx.wBitsPerSample  =  samp[i - GUESTCHANNELS].filebase[34]
                                           + (samp[i - GUESTCHANNELS].filebase[35] *      256);
            l_WaveFormatEx.nBlockAlign     = 1;
        } else
        {   l_WaveFormatEx.nSamplesPerSec  = (DWORD) samplerate;
            l_WaveFormatEx.wBitsPerSample  = bitrate;
            // assert(bitrate % 8 == 0);
            l_WaveFormatEx.nBlockAlign     = bitrate / 8;
        }            
        l_WaveFormatEx.wFormatTag          = WAVE_FORMAT_PCM;
        l_WaveFormatEx.nChannels           = 1;
        l_WaveFormatEx.cbSize              = sizeof(WAVEFORMATEX); // needed for Windows 7!
        l_WaveFormatEx.nAvgBytesPerSec     = l_WaveFormatEx.nSamplesPerSec * l_WaveFormatEx.nBlockAlign;

        memset(&hChannel[i], 0, sizeof(HWAVEOUT));
        DISCARD waveOutOpen
        (   &hChannel[i],
            WAVE_MAPPER,
            &l_WaveFormatEx,
            0,
            0,
            CALLBACK_NULL
        );
        ChannelOpened[i] = TRUE;
}   }

EXPORT void sound_stop(int guestchan)
{   write_wav(guestchan, TRUE);

    if (guestplaying[guestchan])
    {   guestplaying[guestchan] = FALSE;

#ifdef DEBUGSOUND
        zprintf(TEXTPEN_VERBOSE, "sound_stop(%d) at cycle %u.\n", guestchan, cycles_2650);
#endif

        write_wav(guestchan, FALSE);
        DISCARD waveOutReset(hChannel[guestchan]);
        DISCARD waveOutUnprepareHeader(hChannel[guestchan], &ChannelHdr[guestchan], sizeof(WAVEHDR));
    }
#ifdef DEBUGSOUND
    else zprintf(TEXTPEN_VERBOSE, "sound_stop(%d) at cycle %u, but was already stopped!\n", guestchan, cycles_2650);
#endif
}

EXPORT void sound_close(int guestchan)
{   if (ChannelOpened[guestchan])
    {   sound_stop(guestchan);
        DISCARD waveOutClose(hChannel[guestchan]);
        ChannelOpened[guestchan] = FALSE;
}   }

EXPORT void load_samples(void)
{   FILE* TheLocalHandle; // LocalHandle is a variable of winbase.h
    int   i;

    for (i = 0; i < SAMPLES; i++)
    {   if
        (   ((samp[i].filesize = getsize(samp[i].filename)))
         && ((TheLocalHandle = fopen(samp[i].filename, "rb")))
        )
        {   samp[i].filebase = malloc(samp[i].filesize); // should really check return code
#ifdef SCALEVOLUME
            samp[i].scaledbase = malloc(samp[i].filesize); // should really check return code
#endif
            DISCARD fread(samp[i].filebase, samp[i].filesize, 1, TheLocalHandle); // should really check return code
            DISCARD fclose(TheLocalHandle);
            // TheLocalHandle = NULL;
            samp[i].bodysize = samp[i].filesize - 44;
            samp[i].bodybase = samp[i].filebase + 44;

#ifdef DEBUGSOUND
            zprintf
            (   TEXTPEN_VERBOSE,
                "Loaded sample #%d (\"%s\").\n",
                i,
                samp[i].filename
            );
#endif
}   }   }

EXPORT void play_sample(int sample)
{   play_any(GUESTCHANNELS + sample, (float) 0.0, 0);
}
EXPORT void play_any(int guestchan, float hertz, int volume)
{   FAST DWORD change;
#ifdef SCALEVOLUME
    FAST ULONG i;
#endif

    if
    (   !sound
     || !ChannelOpened[guestchan]
     || (guestchan >= GUESTCHANNELS && !samp[guestchan - GUESTCHANNELS].filebase)
    )
    {
#ifdef DEBUGSOUND
        if (!ChannelOpened[guestchan])
        {   zprintf(TEXTPEN_ERROR, "Channel %d is not open!\n", guestchan);
        }
#endif
        return;
    }

    sound_stop(guestchan);
    if (guestchan >= GUESTCHANNELS)
    {
#ifdef SCALEVOLUME
        for (i = 0; i < samp[guestchan - GUESTCHANNELS].bodysize; i++)
        {   samp[guestchan - GUESTCHANNELS].scaledbase[i] = (samp[guestchan - GUESTCHANNELS].bodybase[i] * hostvolume) / 8;
            ChannelHdr[guestchan].lpData     = samp[guestchan - GUESTCHANNELS].scaledbase;
        }
#else
        ChannelHdr[guestchan].lpData         = samp[guestchan - GUESTCHANNELS].bodybase;
#endif
        ChannelHdr[guestchan].dwBufferLength = samp[guestchan - GUESTCHANNELS].bodysize;
        if (loopable[guestchan])
        {   ChannelHdr[guestchan].dwFlags    = WHDR_BEGINLOOP | WHDR_ENDLOOP;
            ChannelHdr[guestchan].dwLoops    = ~0; // loop almost indefinitely
        } else
        {   ChannelHdr[guestchan].dwFlags    = 0;
            ChannelHdr[guestchan].dwLoops    = 1;
    }   }
    else
    {   // assert(volume >= 1 && volume <= 15);
        if (guestchan >= FIRSTGUESTTONE && guestchan <= LASTGUESTTONE)
        {   generate_tone( guestchan, (double) hertz, volume);
        } else
        {   // assert(guestchan >= FIRSTGUESTNOISE && guestchan <= LASTGUESTNOISE);
            generate_noise(guestchan, (double) hertz, volume);
        }
        ChannelHdr[guestchan].lpData         = SoundBuffer[guestchan];
        // assert(bitrate % 8 == 0);
        ChannelHdr[guestchan].dwBufferLength = (DWORD) SoundLength[guestchan] * bitrate / 8;
        ChannelHdr[guestchan].dwFlags        = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        ChannelHdr[guestchan].dwLoops        = ~0; // loop almost indefinitely
    }
    ChannelHdr[guestchan].dwBytesRecorded = 0;
    ChannelHdr[guestchan].dwUser          = 0;
    ChannelHdr[guestchan].lpNext          = NULL;
    ChannelHdr[guestchan].reserved        = (unsigned long) NULL;
    DISCARD waveOutPrepareHeader(hChannel[guestchan], &ChannelHdr[guestchan], sizeof(WAVEHDR));

#ifndef SCALEVOLUME
    DISCARD waveOutSetVolume(hChannel[guestchan], convvolume[hostvolume]); // 16 bits for left channel, then 16 bits for right channel
#endif

    DISCARD waveOutWrite(hChannel[guestchan], &ChannelHdr[guestchan], sizeof(WAVEHDR));

    guestplaying[guestchan] = TRUE;
}

EXPORT void speech_init(void)
{   if
    (   FAILED(CoInitialize(NULL))
     || FAILED(CoCreateInstance
        (   (REFCLSID) &CLSID_SpVoice,
            NULL,
            CLSCTX_ALL,
            (REFIID) &IID_ISpVoice,
            (LPVOID*) &gpIVTxt
    )   ))
    {   usespeech = FALSE;
        // no window open at this point, so don't call updatemenu()
        return;
    }

    speakable = TRUE;
}

EXPORT FLAG speak(STRPTR thespeech)
{   PERSIST       unsigned short UnicodeSpeakBuffer[80 + 1];
    PERSIST const USHORT         speechvol[8 + 1] = { 0, 13, 25, 38, 50, 63, 75, 88, 100 }; // 0..8

    if (!speakable)
    {   return FALSE;
    }

    gpIVTxt->lpVtbl->SetVolume(gpIVTxt, speechvol[hostvolume]);
    DISCARD change_voice(voicename);

    DISCARD MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, thespeech, -1, UnicodeSpeakBuffer, 80); // or should we use 81?
    gpIVTxt->lpVtbl->Speak(gpIVTxt, UnicodeSpeakBuffer, SVSFlagsAsync, NULL);

    return TRUE;
}

EXPORT void speech_setrate(int rate)
{   gpIVTxt->lpVtbl->SetRate(gpIVTxt, rate);
}

EXPORT void speech_die(void)
{   if (gpIVTxt)
    {   gpIVTxt->lpVtbl->Release(gpIVTxt);
	    gpIVTxt = NULL;
    }
    CoUninitialize(); // is this safe even if CoInitialize() failed?
}

EXPORT void sound_preinit(void)
{   int i;

    for (i = 0; i < SAMPLES; i++)
    {   samp[i].filesize   =
        samp[i].bodysize   = 0;
        samp[i].filebase   =
        samp[i].bodybase   = NULL;
    }
    for (i = 0; i < TOTALCHANNELS; i++)
    {   hChannel[i]        = NULL;
        WAVHandle[i]       = NULL;
        ChannelOpened[i]   = FALSE;
    }
    memset(SilenceBuffer16,    0, SILENCEBUFSIZE * 2);
    memset(SilenceBuffer8,  0x80, SILENCEBUFSIZE);
}

EXPORT void sound_die(void)
{   int i;

    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (ChannelOpened[i])
        {   DISCARD waveOutClose(hChannel[i]);
            // ChannelOpened[i] = FALSE;
    }   }
    for (i = 0; i < SAMPLES; i++)
    {   if (samp[i].filebase)
        {   free(samp[i].filebase);
            // samp[i].filebase = samp[i].bodybase = NULL;
        }
#ifdef SCALEVOLUME
        if (samp[i].scaledbase)
        {   free(samp[i].scaledbase);
            // samp[i].scaledbase = NULL;
        }
#endif
}   }

EXPORT void sound_off(FLAG force)
{   int i;

    if (!force)
    {   ff_off();
        if (!sound)
        {   return; // important!
    }   }

    for (i = 0; i < TOTALCHANNELS; i++)
    {   sound_stop(i);
}   }

EXPORT void sound_on(FLAG hadpaused)
{   int i;

    if (hadpaused)
    {   reset_fps();
    }

    if (!sound)
    {   return; // important!
    }

    if (!paused)
    {   if (ambient)
        {   if (machine == TWIN)
            {   play_sample(SAMPLE_SPIN);
            } elif (machine == BINBUG || machine == CD2650)
            {   for (i = 0; i < machines[machine].drives; i++)
                {   if (drive[i].spinning)
                    {   play_sample(SAMPLE_SPIN);
                        break;
        }   }   }   }

        playsound(TRUE);
}   }

EXPORT int list_voices(HWND winptr)
{   HRESULT                 hr;
    ISpDataKey*             pAttribs  = NULL;
    ISpVoice*               pVoice    = NULL;
    ISpObjectTokenCategory* pCategory = NULL;
    IEnumSpObjectTokens*    pEnum     = NULL;
    ISpObjectToken*         pToken    = NULL;
    ULONG                   count     = 0,
                            fetched   = 0,
                            i,
                            j         = 0;
    WCHAR                   szName[80 + 1] = L"";
    WCHAR*                  pszDescription = NULL;
    WCHAR*                  name;
 // WCHAR*                  sex;
    int                     rc        = -1;

    if (!speakable)
    {   return -1;
    }

    hr = CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void**) &pVoice);
    if (FAILED(hr))
    {   zprintf(TEXTPEN_ERROR, "Failed to create voice instance: error $%08X!\n", hr);
        // return -1;
    }
    hr = CoCreateInstance(&CLSID_SpObjectTokenCategory, NULL, CLSCTX_ALL, &IID_ISpObjectTokenCategory, (void**) &pCategory);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "Failed to create token category: error $%08X!\n", hr);
        return -1;
    }
    hr = pCategory->lpVtbl->SetId(pCategory, SPCAT_VOICES, FALSE);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "SetId() failed: error $%08X!\n", hr);
        return -1;
    }
    hr = pCategory->lpVtbl->EnumTokens(pCategory, NULL, NULL, &pEnum);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "EnumTokens() failed: error $%08X!\n", hr);
        return -1;
    }
    hr = pEnum->lpVtbl->GetCount(pEnum, &count);
    if (FAILED(hr) || count == 0)
    {   // zprintf(TEXTPEN_ERROR, "No voices found!\n");
        return -1;
    }

/*
"Microsoft Hazel Desktop" works.
"Microsoft Mike" doesn't work (because it is 32-bit and our OS is 64-bit).
"SampleTTSVoice" works but mostly only says "blah".
"Microsoft Mary" doesn't work (because it is 32-bit and our OS is 64-bit).
"Microsoft Zira Desktop" works.
*/

    // zprintf(TEXTPEN_VERBOSE, "Found %d voices:\n", count);
    for (i = 0; i < count; i++)
    {   hr = pEnum->lpVtbl->Next(pEnum, 1, &pToken, &fetched);
        if (hr == S_OK && fetched == 1)
        {   hr = pToken->lpVtbl->OpenKey(pToken, L"Attributes", &pAttribs);
            if (SUCCEEDED(hr))
            {   name = NULL;
                hr = pAttribs->lpVtbl->GetStringValue(pAttribs, L"Name", &name);
                if (SUCCEEDED(hr) && name)
                {   /* sex = NULL;
                    hr = pAttribs->lpVtbl->GetStringValue(pAttribs, L"Gender", &sex);
                    if (SUCCEEDED(hr) && sex)
                    {   // zprintf(TEXTPEN_VERBOSE, "Name: %ls. Sex: %ls\n", name, sex);
                        */
                        if (wcscmp(name, L"SampleTTSVoice")) // because it mostly only says "blah"
                        {   hr = pVoice->lpVtbl->SetVoice(pVoice, pToken);
                            if (SUCCEEDED(hr))
                            {   hr = pVoice->lpVtbl->Speak(pVoice, L"", 0, NULL); // because the 32-bit only ones fail here
                                if (SUCCEEDED(hr))
                                {   SendMessageW(GetDlgItem(winptr, IDC_VOICE), LB_ADDSTRING, 0, (LPARAM) name);
                                    if (!wcscmp(name, voicename))
                                    {   rc = j;
                                    }
                                    j++;
                        }   }   }
                 // }
                }
                CoTaskMemFree(name);
                // CoTaskMemFree(sex);
                pAttribs->lpVtbl->Release(pAttribs);
            }
            pToken->lpVtbl->Release(pToken);
    }   }

        pEnum->lpVtbl->Release(pEnum);
    pCategory->lpVtbl->Release(pCategory);
       pVoice->lpVtbl->Release(pVoice);

    return rc;
}

EXPORT FLAG change_voice(WCHAR* desired)
{   HRESULT                 hr;
    ISpDataKey*             pAttribs  = NULL;
    ISpObjectTokenCategory* pCategory = NULL;
    IEnumSpObjectTokens*    pEnum     = NULL;
    ISpObjectToken*         pToken    = NULL;
    ULONG                   count     = 0,
                            fetched   = 0,
                            i;
    WCHAR                  *name      = NULL,
                           *sex       = NULL;
    FLAG                    ok        = FALSE;

    if (!speakable)
    {   return FALSE;
    }

    hr = CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void**) &gpIVTxt);
    if (FAILED(hr))
    {   zprintf(TEXTPEN_ERROR, "Failed to create voice instance: error $%08X!\n", hr);
        // return FALSE;
    }
    hr = CoCreateInstance(&CLSID_SpObjectTokenCategory, NULL, CLSCTX_ALL, &IID_ISpObjectTokenCategory, (void**) &pCategory);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "Failed to create token category: error $%08X!\n", hr);
        return FALSE;
    }
    hr = pCategory->lpVtbl->SetId(pCategory, SPCAT_VOICES, FALSE);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "SetId() failed: error $%08X!\n", hr);
        return FALSE;
    }
    hr = pCategory->lpVtbl->EnumTokens(pCategory, NULL, NULL, &pEnum);
    if (FAILED(hr))
    {   // zprintf(TEXTPEN_ERROR, "EnumTokens() failed: error $%08X!\n", hr);
        return FALSE;
    }
    hr = pEnum->lpVtbl->GetCount(pEnum, &count);
    if (FAILED(hr) || count == 0)
    {   // zprintf(TEXTPEN_ERROR, "No voices found!\n");
        return FALSE;
    }

    // zprintf(TEXTPEN_VERBOSE, "Found %d voices:\n", count);
    ok = FALSE;
    for (i = 0; i < count; i++)
    {   hr = pEnum->lpVtbl->Next(pEnum, 1, &pToken, &fetched);
        if (hr == S_OK && fetched == 1)
        {   hr = pToken->lpVtbl->OpenKey(pToken, L"Attributes", &pAttribs);
            if (SUCCEEDED(hr))
            {   name = NULL;
                hr = pAttribs->lpVtbl->GetStringValue(pAttribs, L"Name", &name);
                if (SUCCEEDED(hr) && name)
                {   if (!wcscmp(name, desired))
                    {   hr = gpIVTxt->lpVtbl->SetVoice(gpIVTxt, pToken);
                        if (SUCCEEDED(hr))
                        {   hr = gpIVTxt->lpVtbl->Speak(gpIVTxt, L"", 0, NULL); // because the 32-bit only ones fail here
                            if (SUCCEEDED(hr))
                            {   ok = TRUE;
                                // zprintf(TEXTPEN_VERBOSE ,"Successfully changed voice to %ls.\n", desired);
                }   }   }   }
                CoTaskMemFree(name);
                pAttribs->lpVtbl->Release(pAttribs);
            }
            pToken->lpVtbl->Release(pToken);
    }   }

        pEnum->lpVtbl->Release(pEnum);
    pCategory->lpVtbl->Release(pCategory);

    return ok;
}
