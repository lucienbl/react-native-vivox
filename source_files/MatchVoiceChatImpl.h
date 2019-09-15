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

#include <queue>
#include <string>

#include <mutex>
#include <thread>
#include <condition_variable>

#include "vivoxclientapi/include/vivoxclientsdk.h"

namespace VivoxClientApi {
class MatchVoiceChatImpl :
    public IClientApiEventHandler
{
public:
    MatchVoiceChatImpl(MatchVoiceChat *pMVC);
    virtual ~MatchVoiceChatImpl();

    // Main Controls

public:
    bool ServerConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm);
    bool MatchJoin(const char *pszMatchName, const char *pszMatchToken);
    bool SetLoginCredentials(const char *pszUserId, const char *pszUserToken);
    bool MatchLeave();
    bool ServerDisconnect();
    bool MuteMyself(bool *value);

    // State

public:
    MatchVoiceChat::EState GetState();
    static std::string GetStateName(MatchVoiceChat::EState state);

    // ext status

public:
    void SetCallback(MatchVoiceChat::Callback pfnCallback);

protected:
    class Command
    {
    public:
        enum EType {
            cmdConnect,
            cmdJoin,
            cmdLeave,
            cmdDisconnect,
            cmdStop,
            cmdCallDelegate,
        };
        Command(EType type) :
            m_type(type) {}
        virtual ~Command() {}

        EType GetType() const { return m_type; }

    protected:
        EType m_type;
    };

    class CommandConnect :
        public Command
    {
    public:
        CommandConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm) :
            Command(cmdConnect),
            m_sVivoxServer(pszVivoxServer),
            m_sIssuer(pszIssuer),
            m_sRealm(pszRealm)
        {
        }

        std::string m_sVivoxServer; // HTTP(S) URL of the vivox server to log in
        std::string m_sIssuer;
        std::string m_sRealm;
    };

    class CommandJoin :
        public Command
    {
    public:
        CommandJoin(const char *pszMatchName, const char *pszMatchToken) :
            Command(cmdJoin),
            m_sMatchName(pszMatchName),
            m_sMatchToken(pszMatchToken)
        {
        }

        std::string m_sMatchName; // string identifying the match
        std::string m_sMatchToken;
    };

    class CommandLeave :
        public Command
    {
    public:
        CommandLeave() :
            Command(cmdLeave) {}
    };

    class CommandDisconnect :
        public Command
    {
    public:
        CommandDisconnect() :
            Command(cmdDisconnect) {}
    };

    class CommandStop :
        public Command
    {
    public:
        CommandStop() :
            Command(cmdStop) {}
    };

    class CommandCallDelegate :
        public Command
    {
    public:
        CommandCallDelegate(void(*pf_func)(void *arg0), void *arg0) :
            Command(cmdCallDelegate),
            m_pf_func(pf_func),
            m_arg0(arg0)
        {
        }

        void (*m_pf_func)(void *arg0);
        void *m_arg0;
    };

    // Thread performing all the operations

protected:
    ClientConnection *m_pConnection;
    MatchVoiceChat::EState m_state;
    std::thread m_thread;
    std::condition_variable m_eventCommand;
    std::mutex m_mutex;
    std::queue<Command *> m_Queue;
    MatchVoiceChat::Callback m_pCallback;
    MatchVoiceChat *m_pMVC;
    bool m_bStopping;

    bool InternalInitialize();
    void InternalStop();
    void ThreadProc();

    void SetState(MatchVoiceChat::EState newState);

    bool IsStopping() { return m_bStopping; }

    // Commands
    void SendCommand(Command *pCommand);
    void HandleCommands();

    void OnCommandConnect(CommandConnect &cmd);
    void OnCommandJoin(CommandJoin &cmd);
    void OnCommandLeave(CommandLeave &cmd);
    void OnCommandDisconnect(CommandDisconnect &cmd);
    void OnCommandCallDelegate(CommandCallDelegate &cmd);

    // Timers
    double m_dblNextEventTime;

    void HandleState();
    int  GetNextEventTimeoutMs();
    void SetTimeoutInState(MatchVoiceChat::EState newState, int iMilliseconds);
    bool IsTimeoutElapsed();
    void TimerStop();
    void TimerStart(int iMilliseconds);

    static double NowMs();

    // Current match parameters
    std::string m_sUserID; // empty string for anonymous user
    std::string m_sUserToken;
    std::string m_sVivoxServerURL;
    std::string m_sMatchName;
    std::string m_sMatchToken;
    std::string m_sMatchChannelURI;

    std::string m_sIssuer;
    std::string m_sRealm;

    // Desired match parameters - trategy target
    std::string m_sDesiredVivoxServerURL;
    std::string m_sDesiredMatchName;

    // URL parameter escaping helper
    static std::string URLEscape(const char *s, unsigned int nLen);

protected:
    // const char *m_accessTokenIssuer;

    void Initialize();
    void Uninitialize();
    void RetrieveChannelID();
    void Connect();

    void Disconnect();

    void Login();

    void Logout();

    void Join();

    void Leave();

    void Retry();

    void InternalOnChannelLeft(); // common code to handle onChannelExited and onCallStatsUpdated(final)

    // IClientApiEventHandler implementation

public:
    virtual void InvokeOnUIThread(void(pf_func)(void *arg0), void *arg0);
    virtual void onLogStatementEmitted(LogLevel level, long long nativeMillisecondsSinceEpoch, long threadId, const char *logMessage);
    virtual void onAssert(const char *filename, int line, const char *message);
    virtual void onConnectCompleted(const Uri &server);
    virtual void onConnectCompletedEx(const Uri & /*server*/, vx_backend_type /*backendType*/, const char * /*connectorHandle*/) {}
    virtual void onConnectFailed(const Uri &server, VCSStatus status);
    virtual void onDisconnected(const Uri &server, VCSStatus status);
    virtual void onLoginCompleted(const AccountName &accountName);
    virtual void onLoginCompletedEx(const AccountName & /*accountName*/, const char * /*accountHandle*/) {}
    virtual void onInvalidLoginCredentials(const AccountName &accountName);
    virtual void onLoginFailed(const AccountName &accountName, VCSStatus status);
    virtual void onLogoutCompleted(const AccountName &accountName);
    virtual void onLogoutFailed(const AccountName &accountName, VCSStatus status);
    virtual void onChannelJoined(const AccountName &accountName, const Uri &channelUri);
    virtual void onChannelJoinedEx(const AccountName & /*accountName*/, const Uri & /*channelUri*/, const char * /*sessionGroupHandle*/, const char * /*sessionHandle*/) {}
    virtual void onInvalidChannelCredentials(const AccountName &accountName, const Uri &channelUri);
    virtual void onChannelJoinFailed(const AccountName &accountName, const Uri &channelUri, VCSStatus status);
    virtual void onChannelExited(const AccountName &accountName, const Uri &channelUri, VCSStatus reasonCode);
    virtual void onCallStatsUpdated(const AccountName &accountName, vx_call_stats_t &stats, bool isFinal);

    // IClientApiEventHandler callbacks not used by MatchVoiceChat
    virtual void onParticipantAdded(const AccountName & /* accountName */, const Uri & /* channelUri */, const Uri & /* participantUri */, bool /* isLoggedInUser */) {}
    virtual void onParticipantLeft(const AccountName & /* accountName */, const Uri & /* channelUri */, const Uri & /* participantUri */, bool /* isLoggedInUser */, ParticipantLeftReason /* reason */) {}
    virtual void onParticipantUpdated(const AccountName & /* accountName */, const Uri & /* channelUri */, const Uri & /* participantUri */, bool /* isLoggedInUser */, bool /* speaking */, double /* vuMeterEnergy */, bool /* isMutedForAll */) {}
    virtual void onParticipantKickedCompleted(const AccountName & /* accountName */, const Uri & /* channelUri */, const Uri & /* participantUri */) {}
    virtual void onParticipantKickFailed(const AccountName & /* accountName */, const Uri & /* channelUri */, const Uri & /* participantUri */, VCSStatus /* status */) {}
    virtual void onStartPlayFileIntoChannels(const AccountName & /* accountName */, const char * /* filename */) {}
    virtual void onStartPlayFileIntoChannelsFailed(const AccountName & /* accountName */, const char * /* filename */, VCSStatus /* status */) {}
    virtual void onPlayFileIntoChannelsStopped(const AccountName & /* accountName */, const char * /* filename */) {}
    virtual void onAvailableAudioDevicesChanged() {}
    virtual void onDefaultSystemAudioInputDeviceChanged(const AudioDeviceId & /* deviceId */) {}
    virtual void onDefaultCommunicationAudioInputDeviceChanged(const AudioDeviceId & /* deviceId */) {}
    virtual void onSetAudioInputDeviceCompleted(const AudioDeviceId & /* deviceId */) {}
    virtual void onSetAudioInputDeviceFailed(const AudioDeviceId & /* deviceId */, VCSStatus /* status */) {}
    virtual void onDefaultSystemAudioOutputDeviceChanged(const AudioDeviceId & /* deviceId */) {}
    virtual void onDefaultCommunicationAudioOutputDeviceChanged(const AudioDeviceId & /* deviceId */) {}
    virtual void onSetAudioOutputDeviceCompleted(const AudioDeviceId & /* deviceId */) {}
    virtual void onSetAudioOutputDeviceFailed(const AudioDeviceId & /* deviceId */, VCSStatus /* status */) {}
    virtual void onSetChannelAudioOutputDeviceVolumeCompleted(const AccountName & /* accountName */, const Uri & /* channelUri */, int /* volume */) {}
    virtual void onSetChannelAudioOutputDeviceVolumeFailed(const AccountName & /* accountName */, const Uri & /* channelUri */, int /* volume */, VCSStatus /* status */) {}
    virtual void onSetParticipantAudioOutputDeviceVolumeForMeCompleted(const AccountName & /* accountName */, const Uri & /* targetUser */, const Uri & /* channelUri */, int /* volume */) {}
    virtual void onSetParticipantAudioOutputDeviceVolumeForMeFailed(const AccountName & /* accountName */, const Uri & /* targetUser */, const Uri & /* channelUri */, int /* volume */, VCSStatus /* status */) {}
    virtual void onSetParticipantMutedForAllCompleted(const AccountName & /* accountName */, const Uri & /* target */, const Uri & /* channelUri */, bool /* muted */) {}
    virtual void onSetParticipantMutedForAllFailed(const AccountName & /* accountName */, const Uri & /* target */, const Uri & /* channelUri */, bool /* muted */, VCSStatus /* status */) {}
    virtual void onSetParticipantMutedForMeCompleted(const AccountName & /* accountName */, const Uri & /* target */, const Uri & /* channelUri */, bool /* muted */) {}
    virtual void onSetParticipantMutedForMeFailed(const AccountName & /* accountName */, const Uri & /* target */, const Uri & /* channelUri */, bool /* muted */, VCSStatus /* status */) {}
    virtual void onMuteAllCompleted(const AccountName & /* accountName */, const Uri & /* channelUri */, bool /* muted */) {}
    virtual void onMuteAllFailed(const AccountName & /* accountName */, const Uri & /* channelUri */, bool /* muted */, VCSStatus /* status */) {}
    virtual void onSetChannelTransmissionToSpecificChannelCompleted(const AccountName & /* accountName */, const Uri & /* channelUri */) {}
    virtual void onSetChannelTransmissionToSpecificChannelFailed(const AccountName & /* accountName */, const Uri & /* channelUri */, VCSStatus /* status */) {}
    virtual void onSetChannelTransmissionToAllCompleted(const AccountName & /* accountName */) {}
    virtual void onSetChannelTransmissionToAllFailed(const AccountName & /* accountName */, VCSStatus /* status */) {}
    virtual void onSetChannelTransmissionToNoneCompleted(const AccountName & /* accountName */) {}
    virtual void onSetChannelTransmissionToNoneFailed(const AccountName & /* accountName */, VCSStatus /* status */) {}
    virtual void onAudioInputDeviceTestPlaybackCompleted() {}
    virtual void onSessionGroupRemoved(const AccountName & /* accountName */, const char * /* sessionGroupHandle */) {}
    virtual void onGetStats(vx_resp_sessiongroup_get_stats * /* resp */) {}
};
}
