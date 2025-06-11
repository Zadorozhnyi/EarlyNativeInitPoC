#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define TAG "EarlyFunctionExecution"

// Global flag accessible via JNI
static bool isRooted = false;

// Central root detection logic (C++ only, focused on playIntegrityFix)
bool detectPlayIntegrityFix() {
    const char* paths_to_check[] = {
        "/data/local/tmp/playintegrityfix",
        "/system/bin/playintegrityfix",
        "/sbin/playintegrityfix",
        "/vendor/bin/playintegrityfix",
        "/system/xbin/playintegrityfix",
        "/data/adb/modules/playintegrityfix",
        "/data/adb/modules_update/playintegrityfix",
        "/data/adb/magisk_merge/modules/playintegrityfix",
//        "/sdcard/playintegrityfix_mock" // just for testing!!
    };

    for (const auto& path : paths_to_check) {
        if (access(path, F_OK) == 0) {
            __android_log_print(ANDROID_LOG_WARN, TAG, "playIntegrityFix detected at: %s", path);
            return true;
        }
    }

    // Optional: scan /data/adb/modules/ for suspicious folders
    DIR* dir = opendir("/data/adb/modules/");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strstr(entry->d_name, "integrity") || strstr(entry->d_name, "play")) {
                __android_log_print(ANDROID_LOG_WARN, TAG, "Suspicious module: %s", entry->d_name);
                closedir(dir);
                return true;
            }
        }
        closedir(dir);
    }

    return false;
}

__attribute__((constructor))
void early_function() {
    __android_log_print(ANDROID_LOG_INFO, TAG, "early_function() called BEFORE JNI_OnLoad and onCreate");

    isRooted = detectPlayIntegrityFix();

    if (isRooted) {
        __android_log_print(ANDROID_LOG_WARN, TAG, "Root check (PIF): DETECTED");
    } else {
        __android_log_print(ANDROID_LOG_INFO, TAG, "Root check (PIF): NOT detected");
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_poc_MainActivity_isDeviceRooted(JNIEnv* env, jobject /* this */) {
    return isRooted ? JNI_TRUE : JNI_FALSE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "JNI_OnLoad called");
    return JNI_VERSION_1_6;
}