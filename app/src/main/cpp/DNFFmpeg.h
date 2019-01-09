/**
 * @author Lance
 * @date 2018/8/6
 */

#ifndef DNPLAYER_DNFFMPEG_H
#define DNPLAYER_DNFFMPEG_H


#include <pthread.h>
#include <android/native_window.h>
#include "JavaCallHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>

}


class DNFFmpeg {
public:
    DNFFmpeg(JavaCallHelper *javaCallHelper, const char *dataSource);

    ~DNFFmpeg();

    void prepare();

    void prepareFFmpeg();

    void start();

    void play();

    void setRenderCallback(RenderFrame renderFrame);

    void stop();

    uint64_t getDuration() {
        return duration;
    }

private:
    char *url;
    JavaCallHelper *javaCallHelper;

    pthread_t pid_prepare;
    pthread_t pid_play;
    AVFormatContext *formatContext;

    int64_t duration;

    RenderFrame renderFrame;

    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;

    bool isPlaying;

};


#endif //DNPLAYER_DNFFMPEG_H
