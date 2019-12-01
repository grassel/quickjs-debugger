#include <jni.h>
#include <string>

// in qjs.c
extern "C" {
    void quickjs_main();
}

extern "C" JNIEXPORT jstring JNICALL
Java_org_grassel_quickjsdebuggerandroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    quickjs_main();
    return env->NewStringUTF(hello.c_str());
}
