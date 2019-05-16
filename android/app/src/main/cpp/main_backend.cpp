/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>
#include <assert.h>

#include "BackendServer.h"


static BackendServer gBackendServer;

// Android log function wrappers
static const char* kTAG = "Tanca";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

extern "C" {

/*
 * Interface to Java side to initialize the file server at startup
 */
JNIEXPORT void JNICALL
Java_eu_d8s_Tanca_MainActivity_initialize(JNIEnv *env, jobject instance) {
    LOGI("[TANCA] Coucou !");
}

}
