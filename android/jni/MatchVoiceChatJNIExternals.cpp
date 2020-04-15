/* Copyright (c) 2016-2018 by Mercer Road Corp
 *
 * Permission to use, copy, modify or distribute this software in binary or source form
 * for any purpose is allowed only under explicit prior consent in writing from Mercer Road Corp
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND MERCER ROAD CORP DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL MERCER ROAD CORP
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

# include <stdio.h>
# include <iostream>
# include <vector>

# include <jni.h>

# ifndef WIN32
#   include <signal.h>
# endif

# include "../../source_files/MatchVoiceChat.h"
# include "MatchVoiceChatExternals.h"

# ifdef __ANDROID__
#   include <android/log.h>
#   define LOG_TAG "mvc"
#   define LOG_ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#   define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
# else
#   define LOG_ERR(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#   define LOG_INFO(...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");
# endif

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void * /* reserved */)
{
    LOG_INFO("%s: calling vivox_mvc_setVM( jvm )...", __FUNCTION__);
    vivox_mvc_setVM(jvm);
    return JNI_VERSION_1_2;
}

JNIEXPORT void JNI_OnUnload(JavaVM *, void *)
{
    vivox_mvc_setVM(NULL);
    vivox_mvc_destroy();
}


JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_serverConnect(JNIEnv *jenv, jobject /* obj */, jstring vivoxServer, jstring issuer, jstring realm)
{
    const char *pszVivoxServer = (const char *)jenv->GetStringUTFChars(vivoxServer, 0);
    const char *pszIssuer = (const char *)jenv->GetStringUTFChars(issuer, 0);
    const char *pszRealm = (const char *)jenv->GetStringUTFChars(realm, 0);

    LOG_INFO("%s: calling ServerConnect()...", __FUNCTION__);
    bool bResult = vivox_mvc_serverConnect(pszVivoxServer, pszIssuer, pszRealm) ? false : true;

    if (NULL != pszRealm) {
        jenv->ReleaseStringUTFChars(realm, pszRealm);
        pszRealm = NULL;
    }
    if (NULL != pszIssuer) {
        jenv->ReleaseStringUTFChars(issuer, pszIssuer);
        pszIssuer = NULL;
    }
    if (NULL != pszVivoxServer) {
        jenv->ReleaseStringUTFChars(vivoxServer, pszVivoxServer);
        pszVivoxServer = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_serverDisconnect(JNIEnv * /* jenv */, jobject /* obj */)
{
    return vivox_mvc_serverDisconnect() ? false : true;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_matchJoin(JNIEnv *jenv, jobject /* obj */, jstring matchName, jstring matchToken)
{
    const char *pszMatchName  = (const char *)jenv->GetStringUTFChars(matchName,  0);
    const char *pszMatchToken  = (const char *)jenv->GetStringUTFChars(matchToken,  0);

    bool bResult = vivox_mvc_matchJoin(pszMatchName, pszMatchToken) ? false : true;

    if (NULL != pszMatchName) {
        jenv->ReleaseStringUTFChars(matchName, pszMatchName);
        pszMatchName = NULL;
    }

    if (NULL != pszMatchToken) {
        jenv->ReleaseStringUTFChars(matchToken, pszMatchToken);
        pszMatchToken = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_setLoginCredentials(JNIEnv *jenv, jobject /* obj */, jstring userId, jstring userToken)
{
    const char *pszUserId  = (const char *)jenv->GetStringUTFChars(userId,  0);
    const char *pszUserToken  = (const char *)jenv->GetStringUTFChars(userToken,  0);

    bool bResult = vivox_mvc_setLoginCredentials(pszUserId, pszUserToken) ? false : true;

    if (NULL != pszUserId) {
        jenv->ReleaseStringUTFChars(userId, pszUserId);
        pszUserId = NULL;
    }

    if (NULL != pszUserToken) {
        jenv->ReleaseStringUTFChars(userToken, pszUserToken);
        pszUserToken = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_muteMyself(JNIEnv *jenv, jobject /* obj */, jboolean value)
{
    bool *pszValue = (bool *)(value == JNI_TRUE);

    bool bResult = vivox_mvc_muteMyself(pszValue) ? false : true;

    if (NULL != pszValue) {
        pszValue = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_setParticipantMutedForMe(JNIEnv *jenv, jobject /* obj */, jstring targetUserId, jboolean muted)
{
    const char *pszTargetUserId  = (const char *)jenv->GetStringUTFChars(targetUserId,  0);
    bool *pszMuted = (bool *)(muted == JNI_TRUE);

    bool bResult = vivox_mvc_setParticipantMutedForMe(pszTargetUserId, pszMuted) ? false : true;

    if (NULL != pszTargetUserId) {
        pszTargetUserId = NULL;
    }
    if (NULL != pszMuted) {
        pszMuted = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_setParticipantAudioOutputDeviceVolumeForMe(JNIEnv *jenv, jobject /* obj */, jstring targetUserId, jint volume)
{
    const char *pszTargetUserId  = (const char *)jenv->GetStringUTFChars(targetUserId,  0);
    int pszVolume = (int)volume;

    bool bResult = vivox_mvc_setParticipantAudioOutputDeviceVolumeForMe(pszTargetUserId, pszVolume) ? false : true;

    if (NULL != pszTargetUserId) {
        pszTargetUserId = NULL;
    }
    if (NULL != pszVolume) {
        pszVolume = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_setAudioOutputDeviceMuted(JNIEnv *jenv, jobject /* obj */, jboolean muted)
{
    bool *pszMuted = (bool *)(muted == JNI_TRUE);

    bool bResult = vivox_mvc_setAudioOutputDeviceMuted(pszMuted) ? false : true;

    if (NULL != pszMuted) {
        pszMuted = NULL;
    }

    return bResult;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_isMuted(JNIEnv *jenv, jobject /* obj */)
{
    return vivox_mvc_isMuted() != 0;
}

JNIEXPORT jobject JNICALL Java_com_reactnative_vivox_MatchVoiceChat_getSpeakingParticipants(JNIEnv *jenv, jobject /* obj */)
{
    std::map<std::string, bool> map = vivox_mvc_getSpeakingParticipants();

    jclass clazz = jenv->FindClass("java/util/HashMap");
 	jmethodID init = jenv->GetMethodID(clazz, "<init>", "()V");
 	jobject propertyMap = jenv->NewObject(clazz, init);
 	jmethodID putMethod = jenv->GetMethodID(
 		clazz,
        "put",
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
 	);

    for( auto it = map.begin(); it != map.end(); ++it ) {
     	jstring key = (*jenv).NewStringUTF((*it).first.c_str());
     	jstring value = (*jenv).NewStringUTF((*it).second ? "true" : "false");
 		(*jenv).CallObjectMethod(
 			propertyMap,
 			putMethod,
 			key,
 			value
 		);
 	}
    return propertyMap;
}

JNIEXPORT jboolean JNICALL Java_com_reactnative_vivox_MatchVoiceChat_matchLeave(JNIEnv * /* jenv */, jobject /* obj */)
{
    return vivox_mvc_matchLeave() ? false : true;
}

jclass g_CallbackClass  = NULL;
jmethodID g_CallbackMethod = NULL;

void MatchVoiceChatCallback(VivoxClientApi::MatchVoiceChat *pMVC)
{
    JNIEnv *jenv = NULL;
    JavaVM *vm   = vivox_mvc_getVM();

    LOG_INFO("%s: state = %s", __FUNCTION__, VivoxClientApi::MatchVoiceChat::GetStateName(pMVC->GetState()).c_str());

    if (NULL == vm) {
        LOG_ERR("%s: ERROR, vm == NULL", __FUNCTION__);
        return;
    }

    int getEnvStat = vm->GetEnv((void **)&jenv, JNI_VERSION_1_2);

    if (getEnvStat == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(&jenv, NULL) != 0) {
            LOG_ERR("%s: ERROR, AttachCurrentThread", __FUNCTION__);
            return;
        }
    } else if (getEnvStat == JNI_OK) {
        //
    } else if (getEnvStat == JNI_EVERSION) {
        LOG_ERR("%s: ERROR, JNI_EVERSION", __FUNCTION__);
        return;
    }

    if (NULL == g_CallbackMethod) {
        LOG_ERR("%s: ERROR, g_CallbackMethod == NULL", __FUNCTION__);
        return;
    }

    LOG_INFO("%s: call CallStaticVoidMethod( vivoxStatusCallback )", __FUNCTION__);
    jenv->CallStaticVoidMethod(g_CallbackClass, g_CallbackMethod);

    if (jenv->ExceptionCheck()) {
        jenv->ExceptionDescribe();
    }

    vm->DetachCurrentThread();
}

JNIEXPORT jint JNICALL Java_com_reactnative_vivox_MatchVoiceChat_getState(JNIEnv * /* jenv */, jobject /* obj */)
{
    return vivox_mvc_getState();
}

JNIEXPORT jstring JNICALL Java_com_reactnative_vivox_MatchVoiceChat_getStateName(JNIEnv *jenv, jobject /* obj */, jint state)
{
    return jenv->NewStringUTF(vivox_mvc_getStateName(state));
}

JNIEXPORT void JNICALL Java_com_reactnative_vivox_MatchVoiceChat_setCallback(JNIEnv *jenv, jobject /* obj */, jobject theClass, jstring staticMethod)
{
    if (g_CallbackClass) {
        LOG_INFO("%s: clearing the callback", __FUNCTION__);
        jenv->DeleteGlobalRef(g_CallbackClass);
        g_CallbackClass = NULL;
        g_CallbackMethod = NULL;
        vivox_mvc_setCallback(NULL);
    }

    if (!theClass) {
        LOG_INFO("%s: theClass == NULL", __FUNCTION__);
        return;
    }

    jclass cls = (jclass)theClass;

    const char *pszStaticMethodName = (const char *)jenv->GetStringUTFChars(staticMethod,  0);

    jmethodID methodID = jenv->GetStaticMethodID(cls, pszStaticMethodName, "()V"); // "(Ljava/lang/String;)V"

    if (methodID) {
        LOG_INFO("%s: setting callback \"%s\"...", __FUNCTION__, pszStaticMethodName);
        g_CallbackClass = (jclass)jenv->NewGlobalRef(theClass);
        g_CallbackMethod = methodID;
        vivox_mvc_setCallback(MatchVoiceChatCallback);
        LOG_INFO("%s: callback set", __FUNCTION__);
    } else {
        LOG_ERR("%s: method '%s' not found", __FUNCTION__, pszStaticMethodName);
    }

    if (NULL != pszStaticMethodName) {
        jenv->ReleaseStringUTFChars(staticMethod, pszStaticMethodName);
        pszStaticMethodName = NULL;
    }
}

#ifdef __cplusplus
}
#endif
