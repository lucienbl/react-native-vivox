#include <TargetConditionals.h>
#if !TARGET_OS_SIMULATOR
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

#include <stdio.h>
#include "MatchVoiceChat.h"
#include "MatchVoiceChatImpl.h"
#include "vivoxclientapi/include/vivoxclientsdk.h"


using namespace VivoxClientApi;

MatchVoiceChat::MatchVoiceChat()
{
    m_pImpl = new MatchVoiceChatImpl(this);
}

MatchVoiceChat::~MatchVoiceChat()
{
    delete m_pImpl;
    m_pImpl = NULL;
}

bool MatchVoiceChat::ServerConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm)
{
    return m_pImpl->ServerConnect(pszVivoxServer, pszIssuer, pszRealm);
}

bool MatchVoiceChat::MatchJoin(const char *pszMatchName, const char *pszMatchToken)
{
    return m_pImpl->MatchJoin(pszMatchName, pszMatchToken);
}

bool MatchVoiceChat::SetLoginCredentials(const char *pszUserId, const char *pszUserToken)
{
    return m_pImpl->SetLoginCredentials(pszUserId, pszUserToken);
}

bool MatchVoiceChat::MuteMyself(bool *value)
{
    return m_pImpl->MuteMyself(value);
}

bool MatchVoiceChat::IsMuted()
{
    return m_pImpl->IsMuted();
}

std::map<std::string, bool> MatchVoiceChat::GetSpeakingParticipants()
{
    return m_pImpl->GetSpeakingParticipants();
}

bool MatchVoiceChat::SetParticipantMutedForMe(const char *targetUserId, bool *muted)
{
    return m_pImpl->SetParticipantMutedForMe(targetUserId, muted);
}

bool MatchVoiceChat::SetParticipantAudioOutputDeviceVolumeForMe(const char *targetUserId, int volume)
{
    return m_pImpl->SetParticipantAudioOutputDeviceVolumeForMe(targetUserId, volume);
}

bool MatchVoiceChat::SetAudioOutputDeviceMuted(bool *muted)
{
    return m_pImpl->SetAudioOutputDeviceMuted(muted);
}

bool MatchVoiceChat::MatchLeave()
{
    return m_pImpl->MatchLeave();
}

bool MatchVoiceChat::ServerDisconnect()
{
    return m_pImpl->ServerDisconnect();
}

MatchVoiceChat::EState MatchVoiceChat::GetState()
{
    return m_pImpl->GetState();
}

/* static */
std::string MatchVoiceChat::GetStateName(MatchVoiceChat::EState state)
{
    return MatchVoiceChatImpl::GetStateName(state);
}

void MatchVoiceChat::SetCallback(Callback pfnCallback)
{
    return m_pImpl->SetCallback(pfnCallback);
}
#endif
