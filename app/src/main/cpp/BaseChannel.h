/**
 * @author Lance
 * @date 2018/8/6
 */

#ifndef DNPLAYER_BASECHANNEL_H
#define DNPLAYER_BASECHANNEL_H

#include "safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class BaseChannel {
public:
    BaseChannel(int id, AVCodecContext *avCodecContext, AVRational base) : channelId(id),
                                                                           avCodecContext(
                                                                                   avCodecContext),
                                                                           time_base(base) {
        pkt_queue.setReleaseHandle(releaseAvPacket);
    };

    virtual ~BaseChannel() {
        if (avCodecContext) {
            avcodec_close(avCodecContext);
            avCodecContext = 0;
        }
        pkt_queue.clear();
    };


    virtual void play() = 0;

    virtual void stop() = 0;

    static void releaseAvFrame(AVFrame *&frame) {
        if (frame) {
            av_frame_free(&frame);
            frame = 0;
        }
    }


    static void releaseAvPacket(AVPacket *&packet) {
        if (packet) {
            av_packet_free(&packet);
            packet = 0;
        }
    }

    SafeQueue<AVPacket *> pkt_queue;
    double clock = 0;
    int channelId;
    bool isPlaying = false;
    AVRational time_base;
    AVCodecContext *avCodecContext;
};


#endif //DNPLAYER_AUDIOCHANNEL_H
