#include <jni.h>
#include <android/log.h>

#define TAG "EarlyFunctionExecution"

__attribute__((constructor))
void early_function() {
    __android_log_print(ANDROID_LOG_INFO, TAG, "early_function() called BEFORE JNI_OnLoad and onCreate");
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "JNI_OnLoad called");
    return JNI_VERSION_1_6;
}