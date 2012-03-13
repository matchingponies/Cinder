#pragma once

#include <vector>  
#include <string>

#include "cinder/Cinder.h"
#include "cinder/Thread.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace cel {

enum AudioError_t {
    NONE,
};

typedef std::shared_ptr<class CelPd> CelPdRef;

enum BufferState_t {
    WAITING = 0,
    READY,
    READABLE
};
class CelPd
{
public:
    //  Create and initialize the audio system
    static CelPdRef init(int inChannels, int outChannels, int sampleRate);

    //  Start the audio system playing
    void play();

    //  Pause the audio system
    void pause();

    //  Shut down audio system
    void close();

    //  Returns last error code
    AudioError_t error();

    //  Pd interface
    void  computeAudio(bool on);
    void* openFile(const char* filename, const char* dir);

    //  LibPD wrappers

    ~CelPd();

protected:
    SLObjectItf mEngineObject;
    SLEngineItf mEngineEngine;
    SLObjectItf mOutputMixObject;

    SLObjectItf bqRecorderObject;
    SLRecordItf bqRecorderRecord;
    SLAndroidSimpleBufferQueueItf bqRecorderBufferQueue;;

    SLObjectItf                   bqPlayerObject;
    SLPlayItf                     bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf               bqPlayerEffectSend;
    SLMuteSoloItf                 bqPlayerMuteSolo;
    SLVolumeItf                   bqPlayerVolume;

    // std::mutex                   mRecorderLock;
    std::mutex                   mPlayerLock;
    std::mutex                   mPdLock;
    std::condition_variable      mInputBufReady;
    std::condition_variable      mInputBufReadable;
    std::condition_variable      mOutputBufReady;
    std::shared_ptr<std::thread> mMixerThread;

    AudioError_t mError;

    CelPd(int inChannels, int outChannels, int sampleRate);

    void initSL(int inChannels, int outChannels, int sampleRate);
    void initInput(int channels, int sampleRate);
    void initOutput(int channels, int sampleRate);
    void setError(AudioError_t error);

    void playerLoop();
    void enqueueRecorder();
    void enqueuePlayer();
    // void recorderLoop();

    bool mPlayerRunning;
    bool mRecorderRunning;
    bool mOutputReady;
    bool mInputReady;

    int       mOutputBufIndex;
    int16_t* mOutputBuf[2];
    int       mInputBufIndex;
    int16_t* mInputBuf[2];

    int       mOutputBufSamples;
    int       mInputBufSamples;

    int mInputChannels;
    int mOutputChannels;

    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
    static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
};

}
