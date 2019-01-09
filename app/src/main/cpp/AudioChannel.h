/**
 * @author Lance
 * @date 2018/8/6
 */

#ifndef DNPLAYER_AUDIOCHANNEL_H
#define DNPLAYER_AUDIOCHANNEL_H

#include "BaseChannel.h"
#include <pthread.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libswresample/swresample.h>

}


class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id,AVCodecContext *avCodecContext, AVRational base);

    virtual ~AudioChannel();

    virtual void play();

    virtual void stop();

    void initOpenSL();

    void releaseOpenSL();

    int decodePcm();


private:
    pthread_t pid_audio_play;

    /**
     * opensl es
     */
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

    //
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay = NULL;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;


    SwrContext *swr_ctx = NULL;
    int out_channels;
    int out_samplesize;
public:
    uint8_t *buffer;

};


#endif //DNPLAYER_AUDIOCHANNEL_H
