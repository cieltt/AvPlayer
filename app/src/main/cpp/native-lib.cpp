#include <jni.h>
#include <string>

extern "C"{
#include <libavcodec/avcodec.h>
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ciel_AvPlayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
//    av_version_info();

    char buffer[80];
    av_version_info();
    sprintf(buffer, "my11 ffmpeg%s", av_version_info());
    return env->NewStringUTF(buffer);
}
