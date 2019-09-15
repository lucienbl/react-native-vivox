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

#ifndef MatchVoiceChat_h
#define MatchVoiceChat_h

#include <string>

namespace VivoxClientApi {
class MatchVoiceChatImpl;

class MatchVoiceChat
{
public:
    MatchVoiceChat();
    virtual ~MatchVoiceChat();

    // Main Controls

public:
    bool ServerConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm);
    bool MatchJoin(const char *pszMatchName, const char *pszMatchToken);
    bool SetLoginCredentials(const char *pszUserId, const char *pszUserToken);
    bool MatchLeave();
    bool ServerDisconnect();
    bool MuteMyself(bool *value);
    bool IsMuted();

    // State

public:
    enum EState {
        stateUninitialized,
        stateThreadStarted,
        stateSDKInitializing,
        stateSDKInitFailed,
        stateSDKInitialized,
        stateSDKUninitializing,
        stateConnecting,
        stateConnectRetry,
        stateConnected,
        stateDisconnecting,
        stateLoggingIn,
        stateLoginRetry,
        stateInvalidLoginCredentials,
        stateLoggedIn,
        stateLoggingOut,
        stateHaveChannelID,
        stateJoiningChannel,
        stateJoinChannelRetry,
        stateInvalidChannelCredentials,
        stateInMatch,
        stateLeavingChannel,
    };
    EState GetState();
    static std::string GetStateName(EState state);

    // ext status

public:
    typedef void (*Callback)(MatchVoiceChat *pThis);
    void SetCallback(Callback pfnCallback);

protected:
    MatchVoiceChatImpl *m_pImpl;
};
}
#endif /* MatchVoiceChat_h */
