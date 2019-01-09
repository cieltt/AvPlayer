/**
 * @author Lance
 * @date 2018/8/6
 */



#include "DNFFmpeg.h"
#include "macro.h"


void *prepareFFmpeg_(void *args) {
    DNFFmpeg *dnfFmpeg = static_cast<DNFFmpeg *>(args);
    dnfFmpeg->prepareFFmpeg();
    return 0;

}


DNFFmpeg::DNFFmpeg(JavaCallHelper *javaCallHelper, const char *dataSource)
        : javaCallHelper(javaCallHelper) {
    url = new char[strlen(dataSource) + 1];
    strcpy(url, dataSource);
    isPlaying = false;
    duration = 0;

}

DNFFmpeg::~DNFFmpeg() {
    DELETE(url);
    DELETE(audioChannel);
    DELETE(videoChannel);
    DELETE(javaCallHelper);

    if (formatContext) {
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        formatContext = NULL;
    }
}

void DNFFmpeg::prepare() {
    pthread_create(&pid_prepare, NULL, prepareFFmpeg_, this);
}

void DNFFmpeg::prepareFFmpeg() {

    //todo 最新版本好像不用 regiest_all了
    avformat_network_init();
    // 代表一个 视频/音频 包含了视频、音频的各种信息
    formatContext = avformat_alloc_context();
    //1、打开URL
    AVDictionary* opts = NULL;
    //设置超时3秒
    av_dict_set(&opts, "timeout", "3000000", 0);
    //强制指定AVFormatContext中AVInputFormat的。这个参数一般情况下可以设置为NULL，这样FFmpeg可以自动检测AVInputFormat。
    //输入文件的封装格式
    int ret = avformat_open_input(&formatContext, url, NULL, &opts);
    //av_err2str(ret)
    LOGE("%s open %d  %s", url, ret,av_err2str(ret));
    if (ret != 0) {
        if (javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }
    //2.查找流
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        if (javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }
    //视频时长（单位：微秒us，转换为秒需要除以1000000）
    duration = formatContext->duration / 1000000;
    LOGE("视频时间:%lld 秒", duration);
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecpar = formatContext->streams[i]->codecpar;
        //找到解码器
        AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
        if (!dec) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        //创建上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(dec);
        if (!codecContext) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }
        //复制参数
        if (avcodec_parameters_to_context(codecContext, codecpar) < 0) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        //打开解码器
        if (avcodec_open2(codecContext, dec, 0) != 0) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            return;
        }
        //时间基
        AVRational base =  formatContext->streams[i]->time_base;
        //音频
        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioChannel = new AudioChannel(i,codecContext, base);
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
//            int num = formatContext->streams[i]->avg_frame_rate.num;
//            int den = formatContext->streams[i]->avg_frame_rate.den;
            //帧率 num：分子
            int fps = av_q2d(formatContext->streams[i]->avg_frame_rate);
            videoChannel = new VideoChannel(i,codecContext, base,fps);
            videoChannel->setRenderCallback(renderFrame);
        }
    }

    //音视频都没有
    if (!audioChannel && !videoChannel) {
        javaCallHelper->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
        return;
    }
    javaCallHelper->onParpare(THREAD_CHILD);
}

void *startThread(void *args) {
    DNFFmpeg *ffmpeg = static_cast<DNFFmpeg *>(args);
    ffmpeg->play();
    return 0;
}


void DNFFmpeg::start() {
    isPlaying = true;
    if (audioChannel) {
        audioChannel->play();
    }
    if (videoChannel) {
        videoChannel->audioChannel = audioChannel;
        videoChannel->play();
    }
    pthread_create(&pid_play, NULL, startThread, this);
}


void DNFFmpeg::play() {
    int ret = 0;
    while (isPlaying) {
        //读取包
        AVPacket *packet = av_packet_alloc();
        ret = av_read_frame(formatContext, packet);
        if (audioChannel && audioChannel->pkt_queue.size() > 200) {
            LOGE("audio 积压...");
            av_usleep(100 * 1000);
        }
        if (videoChannel && videoChannel->pkt_queue.size() > 200) {
            LOGE("video 积压...");
            av_usleep(100 * 1000);
        }

        if (ret == 0) {
            //将数据包加入队列
            if (audioChannel && packet->stream_index == audioChannel->channelId) {
                audioChannel->pkt_queue.enQueue(packet);
            } else if (videoChannel && packet->stream_index == videoChannel->channelId) {
                videoChannel->pkt_queue.enQueue(packet);
            }
        } else if (ret == AVERROR_EOF) {
            //读取完毕 但是不一定播放完毕
            while (isPlaying) {
                if (videoChannel->pkt_queue.empty() && audioChannel->pkt_queue.empty()) {
                    break;
                }
                //等待10毫秒再次检查
                av_usleep(10 * 1000);
            }
            break;
        } else {
            break;
        }
    }

    isPlaying = 0;
    audioChannel->stop();

    videoChannel->stop();

}

void DNFFmpeg::setRenderCallback(RenderFrame renderFrame) {
    this->renderFrame = renderFrame;
}

void DNFFmpeg::stop() {
    pthread_join(pid_prepare,0);
    isPlaying = 0;
    pthread_join(pid_play, 0);
}


