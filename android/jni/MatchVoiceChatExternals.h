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

# include <jni.h>

# ifdef __cplusplus
# include "../../source_files/MatchVoiceChat.h"

typedef void (*FTMatchVoiceChatCallback)(VivoxClientApi::MatchVoiceChat *pMVC);

extern "C" {
void vivox_mvc_setCallback(FTMatchVoiceChatCallback lpCallback);
# endif

void    vivox_mvc_setVM(JavaVM *vm);
JavaVM *vivox_mvc_getVM();

void vivox_mvc_destroy();

int vivox_mvc_serverConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm);
int vivox_mvc_serverDisconnect();

int vivox_mvc_matchJoin(const char *pszMatchName, const char *pszMatchToken);
int vivox_mvc_setLoginCredentials(const char *pszUserId, const char *pszUserToken);
int vivox_mvc_muteMyself(bool *value);
int vivox_mvc_isMuted();
std::map<std::string, bool> vivox_mvc_getSpeakingParticipants();
int vivox_mvc_matchLeave();
int vivox_mvc_setParticipantMutedForMe(const char *pszTargetUserId, bool *pszMuted);

int vivox_mvc_getState();
const char *vivox_mvc_getStateName(int state);

# ifdef __cplusplus
}
# endif
