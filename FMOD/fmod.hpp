/* ========================================================================================== */
/* FMOD Ex - C++ header file. Copyright (c), Firelight Technologies Pty, Ltd. 2004-2008.      */
/*                                                                                            */
/* Use this header in conjunction with fmod.h (which contains all the constants / callbacks)  */
/* to develop using C++ classes.                                                              */
/* ========================================================================================== */

#ifndef _FMOD_HPP
#define _FMOD_HPP

#include "fmod.h"

/*
    Constant and defines
*/

/*
    FMOD Namespace
*/
namespace FMOD
{
    class System;
    class Sound;
    class Channel;
    class ChannelGroup;
    class SoundGroup;
    class Reverb;
    class DSP;
    class DSPConnection;
    class Geometry;

    /*
        FMOD global system functions (optional).
    */
    inline FMOD_RESULT Memory_Initialize(void *poolmem, int poollen, FMOD_MEMORY_ALLOCCALLBACK useralloc, FMOD_MEMORY_REALLOCCALLBACK userrealloc, FMOD_MEMORY_FREECALLBACK userfree) { return FMOD_Memory_Initialize(poolmem, poollen, useralloc, userrealloc, userfree); }
    inline FMOD_RESULT Memory_GetStats  (int *currentalloced, int *maxalloced) { return FMOD_Memory_GetStats(currentalloced, maxalloced); }
    inline FMOD_RESULT Debug_SetLevel(FMOD_DEBUGLEVEL level)  { return FMOD_Debug_SetLevel(level); }
    inline FMOD_RESULT Debug_GetLevel(FMOD_DEBUGLEVEL *level) { return FMOD_Debug_GetLevel(level); }
    inline FMOD_RESULT File_SetDiskBusy(int busy) { return FMOD_File_SetDiskBusy(busy); }
    inline FMOD_RESULT File_GetDiskBusy(int *busy) { return FMOD_File_GetDiskBusy(busy); }

    /*
        FMOD System factory functions.
    */
    inline FMOD_RESULT System_Create(System **system) { return FMOD_System_Create((FMOD_SYSTEM **)system); }

    /*
       'System' API
    */

    class System
    {
      private:

        System();   /* Constructor made private so user cannot statically instance a System class.  
                       System_Create must be used. */
      public:

        FMOD_RESULT __stdcall release                ();
                                                 
        // Pre-init functions.
        FMOD_RESULT __stdcall setOutput              (FMOD_OUTPUTTYPE output);
        FMOD_RESULT __stdcall getOutput              (FMOD_OUTPUTTYPE *output);
        FMOD_RESULT __stdcall getNumDrivers          (int *numdrivers);
        FMOD_RESULT __stdcall getDriverInfo          (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT __stdcall getDriverCaps          (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode);
        FMOD_RESULT __stdcall setDriver              (int driver);
        FMOD_RESULT __stdcall getDriver              (int *driver);
        FMOD_RESULT __stdcall setHardwareChannels    (int min2d, int max2d, int min3d, int max3d);
        FMOD_RESULT __stdcall setSoftwareChannels    (int numsoftwarechannels);
        FMOD_RESULT __stdcall getSoftwareChannels    (int *numsoftwarechannels);
        FMOD_RESULT __stdcall setSoftwareFormat      (int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod);
        FMOD_RESULT __stdcall getSoftwareFormat      (int *samplerate, FMOD_SOUND_FORMAT *format, int *numoutputchannels, int *maxinputchannels, FMOD_DSP_RESAMPLER *resamplemethod, int *bits);
        FMOD_RESULT __stdcall setDSPBufferSize       (unsigned int bufferlength, int numbuffers);
        FMOD_RESULT __stdcall getDSPBufferSize       (unsigned int *bufferlength, int *numbuffers);
        FMOD_RESULT __stdcall setFileSystem          (FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blockalign);
        FMOD_RESULT __stdcall attachFileSystem       (FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
        FMOD_RESULT __stdcall setAdvancedSettings    (FMOD_ADVANCEDSETTINGS *settings);
        FMOD_RESULT __stdcall getAdvancedSettings    (FMOD_ADVANCEDSETTINGS *settings);
        FMOD_RESULT __stdcall setSpeakerMode         (FMOD_SPEAKERMODE speakermode);
        FMOD_RESULT __stdcall getSpeakerMode         (FMOD_SPEAKERMODE *speakermode);
        FMOD_RESULT __stdcall setCallback            (FMOD_SYSTEM_CALLBACKTYPE type, FMOD_SYSTEM_CALLBACK callback);
                                                
        // Plug-in support                       
        FMOD_RESULT __stdcall setPluginPath          (const char *path);
        FMOD_RESULT __stdcall loadPlugin             (const char *filename, FMOD_PLUGINTYPE *plugintype, int *index);
        FMOD_RESULT __stdcall getNumPlugins          (FMOD_PLUGINTYPE plugintype, int *numplugins);
        FMOD_RESULT __stdcall getPluginInfo          (FMOD_PLUGINTYPE plugintype, int index, char *name, int namelen, unsigned int *version);
        FMOD_RESULT __stdcall unloadPlugin           (FMOD_PLUGINTYPE plugintype, int index);
        FMOD_RESULT __stdcall setOutputByPlugin      (int index);
        FMOD_RESULT __stdcall getOutputByPlugin      (int *index);
        FMOD_RESULT __stdcall createCodec            (FMOD_CODEC_DESCRIPTION *description);
                                                 
        // Init/Close                            
        FMOD_RESULT __stdcall init                   (int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata);
        FMOD_RESULT __stdcall close                  ();
                                                 
        // General post-init system functions    
        FMOD_RESULT __stdcall update                 ();        /* IMPORTANT! CALL THIS ONCE PER FRAME! */
                                                 
        FMOD_RESULT __stdcall set3DSettings          (float dopplerscale, float distancefactor, float rolloffscale);
        FMOD_RESULT __stdcall get3DSettings          (float *dopplerscale, float *distancefactor, float *rolloffscale);
        FMOD_RESULT __stdcall set3DNumListeners      (int numlisteners);
        FMOD_RESULT __stdcall get3DNumListeners      (int *numlisteners);
        FMOD_RESULT __stdcall set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
        FMOD_RESULT __stdcall get3DListenerAttributes(int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up);
        FMOD_RESULT __stdcall set3DRolloffCallback   (FMOD_3D_ROLLOFFCALLBACK callback);
        FMOD_RESULT __stdcall set3DSpeakerPosition   (FMOD_SPEAKER speaker, float x, float y, bool active);
        FMOD_RESULT __stdcall get3DSpeakerPosition   (FMOD_SPEAKER speaker, float *x, float *y, bool *active);

        FMOD_RESULT __stdcall setStreamBufferSize    (unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
        FMOD_RESULT __stdcall getStreamBufferSize    (unsigned int *filebuffersize, FMOD_TIMEUNIT *filebuffersizetype);
                                                
        // System information functions.        
        FMOD_RESULT __stdcall getVersion             (unsigned int *version);
        FMOD_RESULT __stdcall getOutputHandle        (void **handle);
        FMOD_RESULT __stdcall getChannelsPlaying     (int *channels);
        FMOD_RESULT __stdcall getHardwareChannels    (int *num2d, int *num3d, int *total);
        FMOD_RESULT __stdcall getCPUUsage            (float *dsp, float *stream, float *update, float *total);
        FMOD_RESULT __stdcall getSoundRAM            (int *currentalloced, int *maxalloced, int *total);
        FMOD_RESULT __stdcall getNumCDROMDrives      (int *numdrives);
        FMOD_RESULT __stdcall getCDROMDriveName      (int drive, char *drivename, int drivenamelen, char *scsiname, int scsinamelen, char *devicename, int devicenamelen);
        FMOD_RESULT __stdcall getSpectrum            (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT __stdcall getWaveData            (float *wavearray, int numvalues, int channeloffset);
                                                
        // Sound/DSP/Channel/FX creation and retrieval.       
        FMOD_RESULT __stdcall createSound            (const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound);
        FMOD_RESULT __stdcall createStream           (const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound);
        FMOD_RESULT __stdcall createDSP              (FMOD_DSP_DESCRIPTION *description, DSP **dsp);
        FMOD_RESULT __stdcall createDSPByType        (FMOD_DSP_TYPE type, DSP **dsp);
        FMOD_RESULT __stdcall createDSPByIndex       (int index, DSP **dsp);
        FMOD_RESULT __stdcall createChannelGroup     (const char *name, ChannelGroup **channelgroup);
        FMOD_RESULT __stdcall createSoundGroup       (const char *name, SoundGroup **soundgroup);
        FMOD_RESULT __stdcall createReverb           (Reverb **reverb); 
                                                
        FMOD_RESULT __stdcall playSound              (FMOD_CHANNELINDEX channelid, Sound *sound, bool paused, Channel **channel);
        FMOD_RESULT __stdcall playDSP                (FMOD_CHANNELINDEX channelid, DSP *dsp, bool paused, Channel **channel);
        FMOD_RESULT __stdcall getChannel             (int channelid, Channel **channel);
        FMOD_RESULT __stdcall getMasterChannelGroup  (ChannelGroup **channelgroup);
        FMOD_RESULT __stdcall getMasterSoundGroup    (SoundGroup **soundgroup);
                                              
        // Reverb API                           
        FMOD_RESULT __stdcall setReverbProperties    (const FMOD_REVERB_PROPERTIES *prop);
        FMOD_RESULT __stdcall getReverbProperties    (FMOD_REVERB_PROPERTIES *prop);
        FMOD_RESULT __stdcall setReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop);
        FMOD_RESULT __stdcall getReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop);
                                                 
        // System level DSP access.
        FMOD_RESULT __stdcall getDSPHead             (DSP **dsp);
        FMOD_RESULT __stdcall addDSP                 (DSP *dsp, DSPConnection **connection);
        FMOD_RESULT __stdcall lockDSP                ();
        FMOD_RESULT __stdcall unlockDSP              ();
        FMOD_RESULT __stdcall getDSPClock            (unsigned int *hi, unsigned int *lo);
                                               
        // Recording API.
        FMOD_RESULT __stdcall setRecordDriver        (int driver);
        FMOD_RESULT __stdcall getRecordDriver        (int *driver);
        FMOD_RESULT __stdcall getRecordNumDrivers    (int *numdrivers);
        FMOD_RESULT __stdcall getRecordDriverInfo    (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT __stdcall getRecordDriverCaps    (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency);
        FMOD_RESULT __stdcall getRecordPosition      (unsigned int *position);  

        FMOD_RESULT __stdcall recordStart            (Sound *sound, bool loop);
        FMOD_RESULT __stdcall recordStop             ();
        FMOD_RESULT __stdcall isRecording            (bool *recording);

        // Geometry API.
        FMOD_RESULT __stdcall createGeometry         (int maxpolygons, int maxvertices, Geometry **geometry);
        FMOD_RESULT __stdcall setGeometrySettings    (float maxworldsize);
        FMOD_RESULT __stdcall getGeometrySettings    (float *maxworldsize);
        FMOD_RESULT __stdcall loadGeometry           (const void *data, int datasize, Geometry **geometry);

        // Network functions.
        FMOD_RESULT __stdcall setNetworkProxy        (const char *proxy);
        FMOD_RESULT __stdcall getNetworkProxy        (char *proxy, int proxylen);
        FMOD_RESULT __stdcall setNetworkTimeout      (int timeout);
        FMOD_RESULT __stdcall getNetworkTimeout      (int *timeout);
                                              
        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };

    /*
        'Sound' API
    */
    class Sound
    {
      private:

        Sound();   /* Constructor made private so user cannot statically instance a Sound class.
                      Appropriate Sound creation or retrieval function must be used. */
      public:

        FMOD_RESULT __stdcall release                ();
        FMOD_RESULT __stdcall getSystemObject        (System **system);

        // Standard sound manipulation functions.                                                
        FMOD_RESULT __stdcall lock                   (unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
        FMOD_RESULT __stdcall unlock                 (void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
        FMOD_RESULT __stdcall setDefaults            (float frequency, float volume, float pan, int priority);
        FMOD_RESULT __stdcall getDefaults            (float *frequency, float *volume, float *pan,  int *priority);
        FMOD_RESULT __stdcall setVariations          (float frequencyvar, float volumevar, float panvar);
        FMOD_RESULT __stdcall getVariations          (float *frequencyvar, float *volumevar, float *panvar);
        FMOD_RESULT __stdcall set3DMinMaxDistance    (float min, float max);
        FMOD_RESULT __stdcall get3DMinMaxDistance    (float *min, float *max);
        FMOD_RESULT __stdcall set3DConeSettings      (float insideconeangle, float outsideconeangle, float outsidevolume);
        FMOD_RESULT __stdcall get3DConeSettings      (float *insideconeangle, float *outsideconeangle, float *outsidevolume);
        FMOD_RESULT __stdcall set3DCustomRolloff     (FMOD_VECTOR *points, int numpoints);
        FMOD_RESULT __stdcall get3DCustomRolloff     (FMOD_VECTOR **points, int *numpoints);
        FMOD_RESULT __stdcall setSubSound            (int index, Sound *subsound);
        FMOD_RESULT __stdcall getSubSound            (int index, Sound **subsound);
        FMOD_RESULT __stdcall setSubSoundSentence    (int *subsoundlist, int numsubsounds);
        FMOD_RESULT __stdcall getName                (char *name, int namelen);
        FMOD_RESULT __stdcall getLength              (unsigned int *length, FMOD_TIMEUNIT lengthtype);
        FMOD_RESULT __stdcall getFormat              (FMOD_SOUND_TYPE *type, FMOD_SOUND_FORMAT *format, int *channels, int *bits);
        FMOD_RESULT __stdcall getNumSubSounds        (int *numsubsounds);
        FMOD_RESULT __stdcall getNumTags             (int *numtags, int *numtagsupdated);
        FMOD_RESULT __stdcall getTag                 (const char *name, int index, FMOD_TAG *tag);
        FMOD_RESULT __stdcall getOpenState           (FMOD_OPENSTATE *openstate, unsigned int *percentbuffered, bool *starving);
        FMOD_RESULT __stdcall readData               (void *buffer, unsigned int lenbytes, unsigned int *read);
        FMOD_RESULT __stdcall seekData               (unsigned int pcm);

        FMOD_RESULT __stdcall setSoundGroup          (SoundGroup *soundgroup);
        FMOD_RESULT __stdcall getSoundGroup          (SoundGroup **soundgroup);

        // Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.        
        FMOD_RESULT __stdcall getNumSyncPoints       (int *numsyncpoints);
        FMOD_RESULT __stdcall getSyncPoint           (int index, FMOD_SYNCPOINT **point);
        FMOD_RESULT __stdcall getSyncPointInfo       (FMOD_SYNCPOINT *point, char *name, int namelen, unsigned int *offset, FMOD_TIMEUNIT offsettype);
        FMOD_RESULT __stdcall addSyncPoint           (unsigned int offset, FMOD_TIMEUNIT offsettype, const char *name, FMOD_SYNCPOINT **point);
        FMOD_RESULT __stdcall deleteSyncPoint        (FMOD_SYNCPOINT *point);

        // Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
        FMOD_RESULT __stdcall setMode                (FMOD_MODE mode);
        FMOD_RESULT __stdcall getMode                (FMOD_MODE *mode);
        FMOD_RESULT __stdcall setLoopCount           (int loopcount);
        FMOD_RESULT __stdcall getLoopCount           (int *loopcount);
        FMOD_RESULT __stdcall setLoopPoints          (unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
        FMOD_RESULT __stdcall getLoopPoints          (unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype);

        // For MOD/S3M/XM/IT/MID sequenced formats only.
        FMOD_RESULT __stdcall getMusicNumChannels    (int *numchannels);
        FMOD_RESULT __stdcall setMusicChannelVolume  (int channel, float volume);
        FMOD_RESULT __stdcall getMusicChannelVolume  (int channel, float *volume);
                            
        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };

    /*
        'Channel' API.
    */ 
    class Channel
    {
      private:

        Channel();   /* Constructor made private so user cannot statically instance a Channel class.  
                        Appropriate Channel creation or retrieval function must be used. */
      public:

        FMOD_RESULT __stdcall getSystemObject        (System **system);

        FMOD_RESULT __stdcall stop                   ();
        FMOD_RESULT __stdcall setPaused              (bool paused);
        FMOD_RESULT __stdcall getPaused              (bool *paused);
        FMOD_RESULT __stdcall setVolume              (float volume);
        FMOD_RESULT __stdcall getVolume              (float *volume);
        FMOD_RESULT __stdcall setFrequency           (float frequency);
        FMOD_RESULT __stdcall getFrequency           (float *frequency);
        FMOD_RESULT __stdcall setPan                 (float pan);
        FMOD_RESULT __stdcall getPan                 (float *pan);
        FMOD_RESULT __stdcall setDelay               (FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo);
        FMOD_RESULT __stdcall getDelay               (FMOD_DELAYTYPE delaytype, unsigned int *delayhi, unsigned int *delaylo);
        FMOD_RESULT __stdcall setSpeakerMix          (float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
        FMOD_RESULT __stdcall getSpeakerMix          (float *frontleft, float *frontright, float *center, float *lfe, float *backleft, float *backright, float *sideleft, float *sideright);
        FMOD_RESULT __stdcall setSpeakerLevels       (FMOD_SPEAKER speaker, float *levels, int numlevels);
        FMOD_RESULT __stdcall getSpeakerLevels       (FMOD_SPEAKER speaker, float *levels, int numlevels);
        FMOD_RESULT __stdcall setInputChannelMix     (float *levels, int numlevels);
        FMOD_RESULT __stdcall getInputChannelMix     (float *levels, int numlevels);
        FMOD_RESULT __stdcall setMute                (bool mute);
        FMOD_RESULT __stdcall getMute                (bool *mute);
        FMOD_RESULT __stdcall setPriority            (int priority);
        FMOD_RESULT __stdcall getPriority            (int *priority);
        FMOD_RESULT __stdcall setPosition            (unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT __stdcall getPosition            (unsigned int *position, FMOD_TIMEUNIT postype);
        FMOD_RESULT __stdcall setReverbProperties    (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT __stdcall getReverbProperties    (FMOD_REVERB_CHANNELPROPERTIES *prop);

        FMOD_RESULT __stdcall setChannelGroup        (ChannelGroup *channelgroup);
        FMOD_RESULT __stdcall getChannelGroup        (ChannelGroup **channelgroup);
        FMOD_RESULT __stdcall setCallback            (FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command);

        // 3D functionality.
        FMOD_RESULT __stdcall set3DAttributes        (const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
        FMOD_RESULT __stdcall get3DAttributes        (FMOD_VECTOR *pos, FMOD_VECTOR *vel);
        FMOD_RESULT __stdcall set3DMinMaxDistance    (float mindistance, float maxdistance);
        FMOD_RESULT __stdcall get3DMinMaxDistance    (float *mindistance, float *maxdistance);
        FMOD_RESULT __stdcall set3DConeSettings      (float insideconeangle, float outsideconeangle, float outsidevolume);
        FMOD_RESULT __stdcall get3DConeSettings      (float *insideconeangle, float *outsideconeangle, float *outsidevolume);
        FMOD_RESULT __stdcall set3DConeOrientation   (FMOD_VECTOR *orientation);
        FMOD_RESULT __stdcall get3DConeOrientation   (FMOD_VECTOR *orientation);
        FMOD_RESULT __stdcall set3DCustomRolloff     (FMOD_VECTOR *points, int numpoints);
        FMOD_RESULT __stdcall get3DCustomRolloff     (FMOD_VECTOR **points, int *numpoints);
        FMOD_RESULT __stdcall set3DOcclusion         (float directocclusion, float reverbocclusion);
        FMOD_RESULT __stdcall get3DOcclusion         (float *directocclusion, float *reverbocclusion);
        FMOD_RESULT __stdcall set3DSpread            (float angle);
        FMOD_RESULT __stdcall get3DSpread            (float *angle);
        FMOD_RESULT __stdcall set3DPanLevel          (float level);
        FMOD_RESULT __stdcall get3DPanLevel          (float *level);
        FMOD_RESULT __stdcall set3DDopplerLevel      (float level);
        FMOD_RESULT __stdcall get3DDopplerLevel      (float *level);

        // DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
        FMOD_RESULT __stdcall getDSPHead             (DSP **dsp);
        FMOD_RESULT __stdcall addDSP                 (DSP *dsp, DSPConnection **connection);

        // Information only functions.
        FMOD_RESULT __stdcall isPlaying              (bool *isplaying);
        FMOD_RESULT __stdcall isVirtual              (bool *isvirtual);
        FMOD_RESULT __stdcall getAudibility          (float *audibility);
        FMOD_RESULT __stdcall getCurrentSound        (Sound **sound);
        FMOD_RESULT __stdcall getSpectrum            (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT __stdcall getWaveData            (float *wavearray, int numvalues, int channeloffset);
        FMOD_RESULT __stdcall getIndex               (int *index);
                                                
        // Functions also found in Sound class but here they can be set per channel.
        FMOD_RESULT __stdcall setMode                (FMOD_MODE mode);
        FMOD_RESULT __stdcall getMode                (FMOD_MODE *mode);
        FMOD_RESULT __stdcall setLoopCount           (int loopcount);
        FMOD_RESULT __stdcall getLoopCount           (int *loopcount);
        FMOD_RESULT __stdcall setLoopPoints          (unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
        FMOD_RESULT __stdcall getLoopPoints          (unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype);

        // Userdata set/get.                                                
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };

    /*
        'ChannelGroup' API
    */
    class ChannelGroup
    {
      private:

        ChannelGroup();   /* Constructor made private so user cannot statically instance a ChannelGroup class.  
                             Appropriate ChannelGroup creation or retrieval function must be used. */
      public:

        FMOD_RESULT __stdcall release                 ();
        FMOD_RESULT __stdcall getSystemObject         (System **system);

        // Channelgroup scale values.  (changes attributes relative to the channels, doesn't overwrite them)
        FMOD_RESULT __stdcall setVolume               (float volume);
        FMOD_RESULT __stdcall getVolume               (float *volume);
        FMOD_RESULT __stdcall setPitch                (float pitch);
        FMOD_RESULT __stdcall getPitch                (float *pitch);
        FMOD_RESULT __stdcall set3DOcclusion          (float directocclusion, float reverbocclusion);
        FMOD_RESULT __stdcall get3DOcclusion          (float *directocclusion, float *reverbocclusion);
        FMOD_RESULT __stdcall setPaused               (bool paused);
        FMOD_RESULT __stdcall getPaused               (bool *paused);
        FMOD_RESULT __stdcall setMute                 (bool mute);
        FMOD_RESULT __stdcall getMute                 (bool *mute);

        // Channelgroup override values.  (recursively overwrites whatever settings the channels had)
        FMOD_RESULT __stdcall stop                    ();
        FMOD_RESULT __stdcall overrideVolume          (float volume);
        FMOD_RESULT __stdcall overrideFrequency       (float frequency);
        FMOD_RESULT __stdcall overridePan             (float pan);
        FMOD_RESULT __stdcall overrideReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT __stdcall override3DAttributes    (const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
        FMOD_RESULT __stdcall overrideSpeakerMix      (float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);

        // Nested channel groups.
        FMOD_RESULT __stdcall addGroup                (ChannelGroup *group);
        FMOD_RESULT __stdcall getNumGroups            (int *numgroups);
        FMOD_RESULT __stdcall getGroup                (int index, ChannelGroup **group);
        FMOD_RESULT __stdcall getParentGroup          (ChannelGroup **group);

        // DSP functionality only for channel groups playing sounds created with FMOD_SOFTWARE.
        FMOD_RESULT __stdcall getDSPHead              (DSP **dsp);
        FMOD_RESULT __stdcall addDSP                  (DSP *dsp, DSPConnection **connection);

        // Information only functions.
        FMOD_RESULT __stdcall getName                 (char *name, int namelen);
        FMOD_RESULT __stdcall getNumChannels          (int *numchannels);
        FMOD_RESULT __stdcall getChannel              (int index, Channel **channel);
        FMOD_RESULT __stdcall getSpectrum             (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT __stdcall getWaveData             (float *wavearray, int numvalues, int channeloffset);

        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData             (void *userdata);
        FMOD_RESULT __stdcall getUserData             (void **userdata);
    };

    /*
        'SoundGroup' API
    */
    class SoundGroup
    {
      private:

        SoundGroup();       /* Constructor made private so user cannot statically instance a SoundGroup class.  
                               Appropriate SoundGroup creation or retrieval function must be used. */
      public:

        FMOD_RESULT __stdcall release                ();
        FMOD_RESULT __stdcall getSystemObject        (System **system);

        // SoundGroup control functions.
        FMOD_RESULT __stdcall setMaxAudible          (int maxaudible);
        FMOD_RESULT __stdcall getMaxAudible          (int *maxaudible);
        FMOD_RESULT __stdcall setMaxAudibleBehavior  (FMOD_SOUNDGROUP_BEHAVIOR behavior);
        FMOD_RESULT __stdcall getMaxAudibleBehavior  (FMOD_SOUNDGROUP_BEHAVIOR *behavior);
        FMOD_RESULT __stdcall setMuteFadeSpeed       (float speed);
        FMOD_RESULT __stdcall getMuteFadeSpeed       (float *speed);
        FMOD_RESULT __stdcall setVolume              (float volume);
        FMOD_RESULT __stdcall getVolume              (float *volume);
        FMOD_RESULT __stdcall stop                   ();

        // Information only functions.
        FMOD_RESULT __stdcall getName                (char *name, int namelen);
        FMOD_RESULT __stdcall getNumSounds           (int *numsounds);
        FMOD_RESULT __stdcall getSound               (int index, Sound **sound);
        FMOD_RESULT __stdcall getNumPlaying          (int *numplaying);

        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };

    /*
        'DSP' API
    */
    class DSP
    {
      private:

        DSP();   /* Constructor made private so user cannot statically instance a DSP class.  
                    Appropriate DSP creation or retrieval function must be used. */
      public:

        FMOD_RESULT __stdcall release                ();
        FMOD_RESULT __stdcall getSystemObject        (System **system);

        // Connection / disconnection / input and output enumeration.
        FMOD_RESULT __stdcall addInput               (DSP *target, DSPConnection **connection);
        FMOD_RESULT __stdcall disconnectFrom         (DSP *target);
        FMOD_RESULT __stdcall disconnectAll          (bool inputs, bool outputs);
        FMOD_RESULT __stdcall remove                 ();
        FMOD_RESULT __stdcall getNumInputs           (int *numinputs);
        FMOD_RESULT __stdcall getNumOutputs          (int *numoutputs);
        FMOD_RESULT __stdcall getInput               (int index, DSP **input, DSPConnection **inputconnection);
        FMOD_RESULT __stdcall getOutput              (int index, DSP **output, DSPConnection **outputconnection);

        // DSP unit control.
        FMOD_RESULT __stdcall setActive              (bool active);
        FMOD_RESULT __stdcall getActive              (bool *active);
        FMOD_RESULT __stdcall setBypass              (bool bypass);
        FMOD_RESULT __stdcall getBypass              (bool *bypass);
        FMOD_RESULT __stdcall reset                  ();

        // DSP parameter control.
        FMOD_RESULT __stdcall setParameter           (int index, float value);
        FMOD_RESULT __stdcall getParameter           (int index, float *value, char *valuestr, int valuestrlen);
        FMOD_RESULT __stdcall getNumParameters       (int *numparams);
        FMOD_RESULT __stdcall getParameterInfo       (int index, char *name, char *label, char *description, int descriptionlen, float *min, float *max);
        FMOD_RESULT __stdcall showConfigDialog       (void *hwnd, bool show);
        
        // DSP attributes.        
        FMOD_RESULT __stdcall getInfo                (char *name, unsigned int *version, int *channels, int *configwidth, int *configheight);
        FMOD_RESULT __stdcall getType                (FMOD_DSP_TYPE *type); 
        FMOD_RESULT __stdcall setDefaults            (float frequency, float volume, float pan, int priority);
        FMOD_RESULT __stdcall getDefaults            (float *frequency, float *volume, float *pan, int *priority);
                                                
        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };


    /*
        'DSPConnection' API
    */
    class DSPConnection
    {
      private:

        DSPConnection();    /* Constructor made private so user cannot statically instance a DSPConnection class.  
                               Appropriate DSPConnection creation or retrieval function must be used. */

      public:

        FMOD_RESULT __stdcall getInput              (DSP **input);
        FMOD_RESULT __stdcall getOutput             (DSP **output);
        FMOD_RESULT __stdcall setMix                (float volume);
        FMOD_RESULT __stdcall getMix                (float *volume);
        FMOD_RESULT __stdcall setLevels             (FMOD_SPEAKER speaker, float *levels, int numlevels);
        FMOD_RESULT __stdcall getLevels             (FMOD_SPEAKER speaker, float *levels, int numlevels);

        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData           (void *userdata);
        FMOD_RESULT __stdcall getUserData           (void **userdata);
    };


    /*
        'Geometry' API
    */
    class Geometry
    {
      private:

        Geometry();   /* Constructor made private so user cannot statically instance a Geometry class.  
                         Appropriate Geometry creation or retrieval function must be used. */

      public:        

        FMOD_RESULT __stdcall release                ();

        // Polygon manipulation.
        FMOD_RESULT __stdcall addPolygon             (float directocclusion, float reverbocclusion, bool doublesided, int numvertices, const FMOD_VECTOR *vertices, int *polygonindex);
        FMOD_RESULT __stdcall getNumPolygons         (int *numpolygons); 
        FMOD_RESULT __stdcall getMaxPolygons         (int *maxpolygons, int *maxvertices);
        FMOD_RESULT __stdcall getPolygonNumVertices  (int index, int *numvertices);
        FMOD_RESULT __stdcall setPolygonVertex       (int index, int vertexindex, const FMOD_VECTOR *vertex); 
        FMOD_RESULT __stdcall getPolygonVertex       (int index, int vertexindex, FMOD_VECTOR *vertex);
        FMOD_RESULT __stdcall setPolygonAttributes   (int index, float directocclusion, float reverbocclusion, bool doublesided); 
        FMOD_RESULT __stdcall getPolygonAttributes   (int index, float *directocclusion, float *reverbocclusion, bool *doublesided); 

        // Object manipulation.
        FMOD_RESULT __stdcall setActive              (bool active);                                                 
        FMOD_RESULT __stdcall getActive              (bool *active);                                                 
        FMOD_RESULT __stdcall setRotation            (const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
        FMOD_RESULT __stdcall getRotation            (FMOD_VECTOR *forward, FMOD_VECTOR *up);
        FMOD_RESULT __stdcall setPosition            (const FMOD_VECTOR *position);
        FMOD_RESULT __stdcall getPosition            (FMOD_VECTOR *position);
        FMOD_RESULT __stdcall setScale               (const FMOD_VECTOR *scale);
        FMOD_RESULT __stdcall getScale               (FMOD_VECTOR *scale);
        FMOD_RESULT __stdcall save                   (void *data, int *datasize);

        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);
    };


    /*
        'Reverb' API
    */
    class Reverb
    {
      private:

        Reverb();    /*  Constructor made private so user cannot statically instance a Reverb class.  
                         Appropriate Reverb creation or retrieval function must be used. */

      public:    

        FMOD_RESULT __stdcall release                ();

        // Reverb manipulation.
        FMOD_RESULT __stdcall set3DAttributes        (const FMOD_VECTOR *position, float mindistance, float maxdistance);
        FMOD_RESULT __stdcall get3DAttributes        (FMOD_VECTOR *position, float *mindistance,float *maxdistance);
        FMOD_RESULT __stdcall setProperties          (const FMOD_REVERB_PROPERTIES *properties);
        FMOD_RESULT __stdcall getProperties          (FMOD_REVERB_PROPERTIES *properties);
        FMOD_RESULT __stdcall setActive              (bool active);
        FMOD_RESULT __stdcall getActive              (bool *active);

        // Userdata set/get.
        FMOD_RESULT __stdcall setUserData            (void *userdata);
        FMOD_RESULT __stdcall getUserData            (void **userdata);    
    };
}

#endif
