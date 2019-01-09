/**
 * @author Lance
 * @date 2018/8/6
 */

#ifndef DNPLAYER_JAVACALLHELPER_H
#define DNPLAYER_JAVACALLHELPER_H


#include <jni.h>


class JavaCallHelper {


public:
    JavaCallHelper(JavaVM *_javaVM, JNIEnv *_env, jobject &_jobj);

    ~JavaCallHelper();

    void onError(int thread, int code);

    void onParpare(int thread);

public:
    JavaVM *javaVM;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmid_error;
    jmethodID jmid_prepare;

};


#endif //DNPLAYER_JAVACALLHELPER_H
