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
# include <string>

# include <jni.h>

# ifndef WIN32
#   include <signal.h>
# endif

# include "VxcJNI.h"
# include "MatchVoiceChatExternals.h"
# include "../../source_files/MatchVoiceChat.h"

# ifdef __ANDROID__
#   include <android/log.h>
#   define LOG_TAG "mvc"
#   define LOG_ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#   define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
# else
#   define LOG_ERR(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#   define LOG_INFO(...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");
# endif

static VivoxClientApi::MatchVoiceChat *s_pMatchVoiceChat = NULL;
static JavaVM *g_vm = NULL; // This must be initialized in JNI_OnLoad

VivoxClientApi::MatchVoiceChat *getMVC()
{
    if (NULL == s_pMatchVoiceChat) {
        s_pMatchVoiceChat = new VivoxClientApi::MatchVoiceChat();
    }
    return s_pMatchVoiceChat;
}

void freeMVC()
{
    if (s_pMatchVoiceChat) {
        delete s_pMatchVoiceChat;
        s_pMatchVoiceChat = NULL;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

// void MatchVoiceChatCallback( VivoxClientApi::MatchVoiceChat* pMVC );
void vivox_mvc_setVM(JavaVM *vm)
{
    g_vm = vm;
    vx_jni_set_java_vm(g_vm);
}

JavaVM *vivox_mvc_getVM()
{
    return g_vm;
}

// I do not want to define __attribute__ ((visibility ("default"))) one more time,
// so I'll use JNIEXPORT and JNICALL for __sdtcall
JNIEXPORT void JNICALL vivox_mvc_destroy()
{
    freeMVC();
}

JNIEXPORT int JNICALL vivox_mvc_serverConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm)
{
    return getMVC()->ServerConnect(pszVivoxServer, pszIssuer, pszRealm) ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_serverDisconnect()
{
    return getMVC()->ServerDisconnect() ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_matchJoin(const char *pszMatchName, const char *pszMatchToken)
{
    return getMVC()->MatchJoin(pszMatchName, pszMatchToken) ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_setLoginCredentials(const char *pszUserId, const char *pszUserToken)
{
    return getMVC()->SetLoginCredentials(pszUserId, pszUserToken) ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_muteMyself(bool *value)
{
    return getMVC()->MuteMyself(value) ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_isMuted()
{
    return getMVC()->IsMuted() ? 1 : 0;
}

JNIEXPORT std::map<std::string, bool> JNICALL vivox_mvc_getSpeakingParticipants()
{
    return getMVC()->GetSpeakingParticipants();
}

JNIEXPORT int JNICALL vivox_mvc_matchLeave()
{
    return getMVC()->MatchLeave() ? 0 : 1;
}

JNIEXPORT int JNICALL vivox_mvc_getState()
{
    return getMVC()->GetState();
}

JNIEXPORT const char *JNICALL vivox_mvc_getStateName(int state)
{
    static std::string str = "";
    str = getMVC()->GetStateName((VivoxClientApi::MatchVoiceChat::EState)state).c_str();
    return str.c_str();
}

JNIEXPORT void JNICALL vivox_mvc_setCallback(FTMatchVoiceChatCallback lpCallback)
{
    getMVC()->SetCallback(lpCallback);
}

#ifdef __cplusplus
}
#endif
