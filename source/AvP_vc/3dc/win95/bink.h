#ifndef BINKH
#define BINKH

#define BINKVERSION "0.8e"

#ifndef __RADRES__

#include "bink_rad.h"

RADDEFSTART

typedef struct BINK PTR4* HBINK;

typedef s32  (RADLINK PTR4* BINKIOOPEN)         (struct BINKIO PTR4* Bnkio, const char PTR4 *name, u32 flags);
typedef u32  (RADLINK PTR4* BINKIOREADHEADER)   (struct BINKIO PTR4* Bnkio, s32 Offset, void PTR4* Dest,u32 Size);
typedef u32  (RADLINK PTR4* BINKIOREADFRAME)    (struct BINKIO PTR4* Bnkio, u32 Framenum,s32 origofs,void PTR4* dest,u32 size);
typedef u32  (RADLINK PTR4* BINKIOGETBUFFERSIZE)(struct BINKIO PTR4* Bnkio, u32 Size);
typedef void (RADLINK PTR4* BINKIOSETINFO)      (struct BINKIO PTR4* Bnkio, void PTR4* Buf,u32 Size,u32 FileSize,u32 simulate);
typedef u32  (RADLINK PTR4* BINKIOIDLE)         (struct BINKIO PTR4* Bnkio);
typedef void (RADLINK PTR4* BINKIOCLOSE)        (struct BINKIO PTR4* Bnkio);

typedef struct BINKIO {
  BINKIOREADHEADER ReadHeader;
  BINKIOREADFRAME  ReadFrame;
  BINKIOGETBUFFERSIZE GetBufferSize;
  BINKIOSETINFO SetInfo;
  BINKIOIDLE Idle;
  BINKIOCLOSE Close;
  HBINK bink;
  volatile u32 DoingARead;
  volatile u32 BytesRead;
  volatile u32 TotalTime;
  volatile u32 ForegroundTime;
  volatile u32 BufSize;
  volatile u32 BufHighUsed;
  volatile u32 CurBufSize;
  volatile u32 CurBufUsed;
  volatile u8 iodata[128];
} BINKIO;

typedef s32  (RADLINK PTR4* BINKSNDOPEN)     (struct BINKSND PTR4* BnkSnd, u32 freq, s32 bits, s32 chans, u32 flags, HBINK bink);
typedef void (RADLINK PTR4* BINKSNDRESET)    (struct BINKSND PTR4* BnkSnd);
typedef s32  (RADLINK PTR4* BINKSNDREADY)    (struct BINKSND PTR4* BnkSnd);
typedef s32  (RADLINK PTR4* BINKSNDLOCK)     (struct BINKSND PTR4* BnkSnd, u8 PTR4* PTR4* addr, u32 PTR4* len);
typedef s32  (RADLINK PTR4* BINKSNDUNLOCK)   (struct BINKSND PTR4* BnkSnd, u32 filled);
typedef void (RADLINK PTR4* BINKSNDVOLUME)   (struct BINKSND PTR4* BnkSnd, s32 volume);
typedef void (RADLINK PTR4* BINKSNDPAN)      (struct BINKSND PTR4* BnkSnd, s32 pan);
typedef s32  (RADLINK PTR4* BINKSNDOFF)      (struct BINKSND PTR4* BnkSnd, s32 status);
typedef s32  (RADLINK PTR4* BINKSNDPAUSE)    (struct BINKSND PTR4* BnkSnd, s32 status);
typedef void (RADLINK PTR4* BINKSNDCLOSE)    (struct BINKSND PTR4* BnkSnd);

typedef BINKSNDOPEN  (RADLINK PTR4* BINKSNDSYSOPEN) (u32 param);

typedef struct BINKSND {
  BINKSNDRESET SetParam;
  BINKSNDRESET Reset;
  BINKSNDREADY Ready;
  BINKSNDLOCK Lock;
  BINKSNDUNLOCK Unlock;
  BINKSNDVOLUME Volume;
  BINKSNDPAN Pan;
  BINKSNDPAUSE Pause;
  BINKSNDOFF Off;
  BINKSNDCLOSE Close;
  u32 BestSizeIn16;
  u32 SoundDroppedOut;
  u32 freq;
  s32 bits,chans;
  u8 snddata[128];
} BINKSND;

typedef struct BINKRECT {
  s32 Left,Top,Width,Height;
} BINKRECT;

#define BINKMAXDIRTYRECTS 8

typedef struct BUNDLEPOINTERS {
  void* typeptr;
  void* type16ptr;
  void* colorptr;
  void* bits2ptr;
  void* motionXptr;
  void* motionYptr;
  void* dctptr;
  void* mdctptr;
  void* patptr;
} BUNDLEPOINTERS;


typedef struct BINK {
  u32 Width;             // Width (1 based, 640 for example)
  u32 Height;            // Height (1 based, 480 for example)
  u32 StretchWidth;      // Default stretch width
  u32 StretchHeight;     // Default stretch height (used for Y double)
  u32 Frames;            // Number of frames (1 based, 100 = 100 frames)
  u32 FrameNum;          // Frame to *be* displayed (1 based)
  u32 LastFrameNum;      // Last frame decompressed or skipped (1 based)

  u32 FrameRate;         // Frame Rate Numerator
  u32 FrameRateDiv;      // Frame Rate Divisor (frame rate=numerator/divisor)

  u32 ReadError;         // Non-zero if a read error has ocurred
  u32 OpenFlags;         // flags used on open
  u32 BinkType;          // Bink flags

  u32 Size;              // size of file
  u32 FrameSize;         // The current frame's size in bytes
  u32 SndSize;           // The current frame sound tracks' size in bytes

  BINKRECT FrameRects[BINKMAXDIRTYRECTS];// Dirty rects from BinkGetRects
  s32 NumRects;

  u32 YPlaneNum;         // which YPlane is current
  void PTR4* YPlane[2];  // pointer to the uncompressed Y (Cr and Cr follow)
  u32 YWidth;            // widths and heights of the video planes
  u32 YHeight;
  u32 UVWidth;
  u32 UVHeight;

  void PTR4* MaskPlane;  // pointer to the mask plane (Ywidth/16*Yheight/16)
  u32 MaskPitch;         // Mask Pitch
  u32 MaskLength;        // total length of the mask plane

  u32 LargestFrameSize;  // Largest frame size
  u32 InternalFrames;    // how many frames were potentially compressed
  
  s32 NumTracks;         // how many tracks

  u32 Highest1SecRate;   // Highest 1 sec data rate
  u32 Highest1SecFrame;  // Highest 1 sec data rate starting frame

  s32 Paused;            // is the bink movie paused?

  u32 BackgroundThread;  // handle to background thread

  // everything below is for internal Bink use

  void PTR4* compframe;  // compressed frame data
  void PTR4* preloadptr; // preloaded compressed frame data
  u32* frameoffsets;     // offsets of each of the frames

  BINKIO bio;            // IO structure
  u8 PTR4* ioptr;        // io buffer ptr
  u32 iosize;            // io buffer size
  u32 decompheight;      // height not include interlacing

  s32 trackindex;        // track index
  u32 PTR4* tracksizes;  // largest single frame of track
  u32 PTR4* tracktypes;  // type of each sound track
  s32 PTR4* trackIDs;    // external track numbers

  u32 numrects;          // number of rects from BinkGetRects

  u32 playedframes;      // how many frames have we played
  u32 firstframetime;    // very first frame start
  u32 startframetime;    // start frame start
  u32 startblittime;     // start of blit period
  u32 startsynctime;     // start of synched time
  u32 startsyncframe;    // frame of startsynctime
  u32 twoframestime;     // two frames worth of time
  u32 entireframetime;   // entire frame time

  u32 slowestframetime;  // slowest frame in ms
  u32 slowestframe;      // slowest frame number
  u32 slowest2frametime; // second slowest frame in ms
  u32 slowest2frame;     // second slowest frame

  u32 soundon;           // sound turned on?
  u32 videoon;           // video turned on?

  u32 totalmem;          // total memory used
  u32 timedecomp;        // total time decompressing
  u32 timeblit;          // total time blitting
  u32 timeopen;          // total open time

  u32 fileframerate;     // frame rate originally in the file
  u32 fileframeratediv;

  u32 threadcontrol;     // controls the background reading thread

  u32 runtimeframes;       // max frames for runtime analysis
  u32 runtimemoveamt;      // bytes to move each frame
  u32 PTR4* rtframetimes;  // start times for runtime frames
  u32 PTR4* rtdecomptimes; // decompress times for runtime frames
  u32 PTR4* rtblittimes;   // blit times for runtime frames
  u32 PTR4* rtreadtimes;   // read times for runtime frames

  u32 lastdecompframe;   // last frame number decompressed

  u32 sndbufsize;        // sound buffer size
  u8 PTR4* sndbuf;       // sound buffer
  u8 PTR4* sndend;       // end of the sound buffer
  u8 PTR4* sndwritepos;  // current write position
  u8 PTR4* sndreadpos;   // current read position
  u32 sndcomp;           // sound compression handle
  u32 sndamt;            // amount of sound currently in the buffer
  volatile u32 sndreenter; // re-entrancy check on the sound
  u32 sndconvert8;       // convert back to 8-bit sound at runtime
  BINKSND bsnd;          // SND structure
  u32 skippedlastblit;   // skipped last frame?
  u32 skippedblits;      // how many blits were skipped
  u32 soundskips;        // number of sound stops

  BUNDLEPOINTERS bunp;   // pointers to internal temporary memory
} BINK;


typedef struct BINKSUMMARY {
  u32 Width;             // Width of frames
  u32 Height;            // Height of frames
  u32 TotalTime;         // total time (ms)
  u32 FileFrameRate;     // frame rate
  u32 FileFrameRateDiv;  // frame rate divisor
  u32 FrameRate;         // frame rate
  u32 FrameRateDiv;      // frame rate divisor
  u32 TotalOpenTime;     // Time to open and prepare for decompression
  u32 TotalFrames;       // Total Frames
  u32 TotalPlayedFrames; // Total Frames played
  u32 SkippedFrames;     // Total number of skipped frames
  u32 SkippedBlits;      // Total number of skipped blits
  u32 SoundSkips;        // Total number of sound skips
  u32 TotalBlitTime;     // Total time spent blitting
  u32 TotalReadTime;     // Total time spent reading
  u32 TotalDecompTime;   // Total time spent decompressing
  u32 TotalBackReadTime; // Total time spent reading in background
  u32 TotalReadSpeed;    // Total io speed (bytes/second)
  u32 SlowestFrameTime;  // Slowest single frame time (ms)
  u32 Slowest2FrameTime; // Second slowest single frame time (ms)
  u32 SlowestFrameNum;   // Slowest single frame number
  u32 Slowest2FrameNum;  // Second slowest single frame number
  u32 AverageDataRate;   // Average data rate of the movie
  u32 AverageFrameSize;  // Average size of the frame
  u32 HighestMemAmount;  // Highest amount of memory allocated
  u32 TotalIOMemory;     // Total extra memory allocated
  u32 HighestIOUsed;     // Highest extra memory actually used
  u32 Highest1SecRate;   // Highest 1 second rate
  u32 Highest1SecFrame;  // Highest 1 second start frame
} BINKSUMMARY;


typedef struct BINKREALTIME {
  u32 FrameNum;          // Current frame number
  u32 FrameRate;         // frame rate
  u32 FrameRateDiv;      // frame rate divisor
  u32 Frames;            // frames in this sample period
  u32 FramesTime;        // time is ms for these frames
  u32 FramesDecompTime;  // time decompressing these frames
  u32 FramesReadTime;    // time reading these frames
  u32 FramesBlitTime;    // time blitting these frames
  u32 ReadBufferSize;    // size of read buffer
  u32 ReadBufferUsed;    // amount of read buffer currently used
  u32 FramesDataRate;    // data rate for these frames
} BINKREALTIME;

#define BINKMARKER 'fKIB'

typedef struct BINKHDR {
  u32 Marker;            // BNKa
  u32 Size;              // size of the file-8
  u32 Frames;            // Number of frames (1 based, 100 = 100 frames)
  u32 LargestFrameSize;  // Size in bytes of largest frame
  u32 InternalFrames;    // Number of internal frames

  u32 Width;             // Width (1 based, 640 for example)
  u32 Height;            // Height (1 based, 480 for example)
  u32 FrameRate;         // frame rate
  u32 FrameRateDiv;      // frame rate divisor (framerate/frameratediv=fps)

  u32 Flags;             // height compression options
  u32 NumTracks;         // number of tracks
} BINKHDR;


//=======================================================================
#define BINKFRAMERATE    0x00080000L // Override fr (call BinkFrameRate first)
#define BINKPRELOADALL   0x00100000L // Preload the entire animation
#define BINKSNDTRACK     0x00200000L // Set the track number to play
#define BINKNOSKIP       0x00400000L // Don't skip frames if falling behind
#define BINKNOFILLIOBUF  0x00800000L // Fill the IO buffer in SmackOpen
#define BINKSIMULATE     0x01000000L // Simulate the speed (call BinkSim first)
#define BINKFILEHANDLE   0x02000000L // Use when passing in a file handle
#define BINKIOSIZE       0x04000000L // Set an io size (call BinkIOSize first)
#define BINKIOPROCESSOR  0x08000000L // Set an io processor (call BinkIO first)
#define BINKFROMMEMORY   0x40000000L // Use when passing in a pointer to the file
#define BINKNOTHREADEDIO 0x80000000L // Don't use a background thread for IO

#define BINKSURFACEYINTERLACE 0x20000000L // Force interleaving Y scaling
#define BINKSURFACEYDOUBLE    0x10000000L // Force doubling Y scaling
#define BINKSURFACEYFORCENONE 0x30000000L // Force Y scaling off

#define BINKSURFACEFAST             0x00000000L
#define BINKSURFACESLOW             0x80000000L
#define BINKSURFACEDIRECT           0x40000000L

#define BINKSURFACEFLIPPED          0x08000000L
#define BINKSURFACECOPYALL          0x04000000L // copy all pixels (not just changed)
#define BINKSURFACENOSKIP           0x01000000L // don't skip the blit if behind in sound

#define BINKSURFACE24       0
#define BINKSURFACE32       1
#define BINKSURFACE555      2
#define BINKSURFACE565      3
#define BINKSURFACE655      4
#define BINKSURFACE664      5
#define BINKSURFACE8P       6
#define BINKSURFACEYUY2     7
#define BINKSURFACEUYVY     8
#define BINKSURFACEYV12     9
#define BINKSURFACEMASK    15

#define BINKGOTOQUICK          1

#define BINKGETKEYPREVIOUS     0
#define BINKGETKEYNEXT         1
#define BINKGETKEYCLOSEST      2
#define BINKGETKEYNOTEQUAL   128

//=======================================================================

RADEXPFUNC void PTR4* RADEXPLINK BinkLogoAddress(void);

RADEXPFUNC void RADEXPLINK BinkSetError(const char PTR4* err);
RADEXPFUNC char PTR4* RADEXPLINK BinkGetError(void);

RADEXPFUNC HBINK RADEXPLINK BinkOpen(const char PTR4* name,u32 flags);

#ifdef __RADMAC__
  #include <files.h>

  RADEXPFUNC HBINK RADEXPLINK BinkMacOpen(FSSpec* fsp,u32 flags);
#endif

RADEXPFUNC u32  RADEXPLINK BinkDoFrame(HBINK bnk);
RADEXPFUNC void RADEXPLINK BinkNextFrame(HBINK bnk);
RADEXPFUNC s32  RADEXPLINK BinkWait(HBINK bnk);
RADEXPFUNC void RADEXPLINK BinkClose(HBINK bnk);
RADEXPFUNC s32  RADEXPLINK BinkPause(HBINK bnk,s32 pause);
RADEXPFUNC s32  RADEXPLINK BinkCopyToBuffer(HBINK bnk,void* dest,u32 destpitch,u32 destheight,u32 destx,u32 desty,u32 flags);
RADEXPFUNC s32  RADEXPLINK BinkGetRects(HBINK bnk,u32 surfacetype);
RADEXPFUNC void RADEXPLINK BinkGoto(HBINK bnk,u32 frame,s32 flags);  // use 1 for the first frame
RADEXPFUNC u32  RADEXPLINK BinkGetKeyFrame(HBINK bnk,u32 frame,s32 flags);

RADEXPFUNC u32  RADEXPLINK BinkSetVideoOnOff(HBINK bnk,s32 onoff);
RADEXPFUNC u32  RADEXPLINK BinkSetSoundOnOff(HBINK bnk,s32 onoff);
RADEXPFUNC void RADEXPLINK BinkSetVolume(HBINK bnk,s32 volume);
RADEXPFUNC void RADEXPLINK BinkSetPan(HBINK bnk,s32 pan);
RADEXPFUNC void RADEXPLINK BinkService(HBINK bink);

RADEXPFUNC u32  RADEXPLINK BinkGetTrackType(HBINK bnk,u32 trackindex);
RADEXPFUNC u32  RADEXPLINK BinkGetTrackID(HBINK bnk,u32 trackindex);
RADEXPFUNC u32  RADEXPLINK BinkGetTrackData(HBINK bnk,void PTR4* dest,u32 trackindex);

RADEXPFUNC void RADEXPLINK BinkGetSummary(HBINK bnk,BINKSUMMARY PTR4* sum);
RADEXPFUNC void RADEXPLINK BinkGetRealtime(HBINK bink,BINKREALTIME PTR4* run,u32 frames);

#define BINKNOSOUND 0xffffffff

RADEXPFUNC void RADEXPLINK BinkSetSoundTrack(u32 track);
RADEXPFUNC void RADEXPLINK BinkSetIO(BINKIOOPEN io);
RADEXPFUNC void RADEXPLINK BinkSetFrameRate(u32 forcerate,u32 forceratediv);
RADEXPFUNC void RADEXPLINK BinkSetSimulate(u32 sim);
RADEXPFUNC void RADEXPLINK BinkSetIOSize(u32 iosize);

RADEXPFUNC s32  RADEXPLINK BinkSetSoundSystem(BINKSNDSYSOPEN open, u32 param);

#ifdef __RADWIN__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenDirectSound(u32 param); // don't call directly
  #define BinkSoundUseDirectSound(lpDS) BinkSetSoundSystem(BinkOpenDirectSound,(u32)lpDS)

  #define BinkTimerSetup()
  #define BinkTimerDone()
  #define BinkTimerRead timeGetTime

  #define INCLUDE_MMSYSTEM_H
  #include "windows.h"
  #include "windowsx.h"

  #ifdef __RADNT__          // to combat WIN32_LEAN_AND_MEAN

    #include "mmsystem.h"

    RADEXPFUNC s32 RADEXPLINK BinkDDSurfaceType(void PTR4* lpDDS);

  #endif

#endif

#ifndef __RADMAC__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenMiles(u32 param); // don't call directly
  #define BinkSoundUseMiles(hdigdriver) BinkSetSoundSystem(BinkOpenMiles,(u32)hdigdriver)

#endif


#ifndef __RADDOS__

//=========================================================================
typedef struct BINKBUFFER * HBINKBUFFER;

typedef struct BINKBUFFER {
  u32 Width;
  u32 Height;
  u32 WindowWidth;
  u32 WindowHeight;
  u32 SurfaceType;
  void* Buffer;
  u32 BufferPitch;
  s32 ClientOffsetX;
  s32 ClientOffsetY;
  u32 ScreenWidth;
  u32 ScreenHeight;
  u32 ScreenDepth;
  u32 ExtraWindowWidth;
  u32 ExtraWindowHeight;
  u32 ScaleFlags;
  u32 StretchWidth;
  u32 StretchHeight;

  s32 surface;
  void* ddsurface;
  void* ddclipper;
  s32 destx,desty;
  u32 HWND;
  s32 ddoverlay;
  s32 ddoffscreen;
  s32 lastovershow;

  s32 issoftcur;
  u32 cursorcount;
  void* buffertop;
  u32 type;
  s32 noclipping;

} BINKBUFFER;


#define BINKBUFFERSTRETCHXINT    0x80000000
#define BINKBUFFERSTRETCHX       0x40000000
#define BINKBUFFERSHRINKXINT     0x20000000
#define BINKBUFFERSHRINKX        0x10000000
#define BINKBUFFERSTRETCHYINT    0x08000000
#define BINKBUFFERSTRETCHY       0x04000000
#define BINKBUFFERSHRINKYINT     0x02000000
#define BINKBUFFERSHRINKY        0x01000000
#define BINKBUFFERRESOLUTION     0x00800000

#define BINKBUFFERAUTO                0
#define BINKBUFFERPRIMARY             1
#define BINKBUFFERYV12OVERLAY         2
#define BINKBUFFERYUY2OVERLAY         3
#define BINKBUFFERUYVYOVERLAY         4
#define BINKBUFFERYV12OFFSCREEN       5
#define BINKBUFFERYUY2OFFSCREEN       6
#define BINKBUFFERUYVYOFFSCREEN       7
#define BINKBUFFERRGBOFFSCREENVIDEO   8
#define BINKBUFFERRGBOFFSCREENSYSTEM  9
#define BINKBUFFERDRAWDIB            10
#define BINKBUFFERTYPEMASK           31

RADEXPFUNC HBINKBUFFER RADEXPLINK BinkBufferOpen( HWND wnd, u32 width, u32 height, u32 bufferflags);
RADEXPFUNC void RADEXPLINK BinkBufferClose( HBINKBUFFER buf);
RADEXPFUNC s32 RADEXPLINK BinkBufferLock( HBINKBUFFER buf);
RADEXPFUNC s32 RADEXPLINK BinkBufferUnlock( HBINKBUFFER buf);
RADEXPFUNC void RADEXPLINK BinkBufferSetResolution( s32 w, s32 h, s32 bits);
RADEXPFUNC void RADEXPLINK BinkBufferCheckWinPos( HBINKBUFFER buf, s32 PTR4* destx, s32 PTR4* desty);
RADEXPFUNC s32 RADEXPLINK BinkBufferSetOffset( HBINKBUFFER buf, s32 destx, s32 desty);
RADEXPFUNC void RADEXPLINK BinkBufferBlit( HBINKBUFFER buf, BINKRECT PTR4* rects, u32 numrects );
RADEXPFUNC s32 RADEXPLINK BinkBufferSetScale( HBINKBUFFER buf, u32 w, u32 h);
RADEXPFUNC s32 RADEXPLINK BinkBufferSetHWND( HBINKBUFFER buf, HWND newwnd);
RADEXPFUNC char PTR4* RADEXPLINK BinkBufferDescription( HBINKBUFFER buf);
RADEXPFUNC char PTR4* RADEXPLINK BinkBufferGetError();

RADEXPFUNC s32 RADEXPLINK BinkDDSurfaceType(void PTR4* lpDDS);
RADEXPFUNC s32 RADEXPLINK BinkIsSoftwareCursor(void PTR4* lpDDSP,HCURSOR cur);
RADEXPFUNC s32 RADEXPLINK BinkCheckCursor(HWND wnd,s32 x,s32 y,s32 w,s32 h);
RADEXPFUNC void RADEXPLINK BinkRestoreCursor(s32 checkcount);

#endif

RADDEFEND

#endif

#endif

