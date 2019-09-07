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
#include <assert.h>
#include <sstream>
#include <string.h>
#include "vivox-config.h"
#include "MatchVoiceChat.h"

extern "C" {
#include "sha1/sha1.h"
#define SHA1_DIGEST_SIZE 20
}

// #include "mvc.h"

#ifndef WIN32
#  include <sys/time.h>
#else
#  include <Windows.h>
#endif

#define CHECK(cond) assert((cond));

# ifdef __ANDROID__
#   include <android/log.h>
#   define LOG_TAG "mvc"
#   define LOG_ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#   define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
# else
#   define LOG_ERR(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#   define LOG_INFO(...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");
# endif

# define REPORT_ERROR(msg, err) LOG_ERR("%s: error %ld: %s\n", __FUNCTION__, (long)(err), (msg));
# define INFO(msg) LOG_INFO("%s: %s\n", __FUNCTION__, (msg));

#define SECONDS(n) ((n) * 1000)

#define TIMEOUT_LOGIN_RETRY               SECONDS(15)
#define TIMEOUT_JOIN_RETRY                SECONDS(15)
#define TIMEOUT_CONNECT_RETRY             SECONDS(15)

using namespace VivoxClientApi;

#include "MatchVoiceChatImpl.h"
#include "vivoxclientapi/util.h"
#include "Escape.h"

MatchVoiceChatImpl::MatchVoiceChatImpl(MatchVoiceChat *pMVC) :
    m_pConnection(NULL),
    m_state(MatchVoiceChat::stateUninitialized),
    m_pCallback(NULL),
    m_pMVC(pMVC),
    m_bStopping(false),
    m_dblNextEventTime(0)
{
    CHECK(pMVC);
    InternalInitialize();
}

MatchVoiceChatImpl::~MatchVoiceChatImpl()
{
    InternalStop();
}

bool MatchVoiceChatImpl::InternalInitialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_state != MatchVoiceChat::stateUninitialized) {
        return true;
    }

    if (IsStopping()) {
        REPORT_ERROR("attempt to initialize while stopping", 0);
        return false;
    }

    m_thread = std::thread(&MatchVoiceChatImpl::ThreadProc, this);
    m_state = MatchVoiceChat::stateThreadStarted;
    return true;
}

void MatchVoiceChatImpl::SetState(MatchVoiceChat::EState newState)
{
    MatchVoiceChat::Callback pCallback = NULL;
    {
        // std::lock_guard<std::mutex> lock( m_mutex );
        printf("State: %s -> %s\n", GetStateName(m_state).c_str(), GetStateName(newState).c_str());
        if (!IsStopping() && newState != m_state) {
            pCallback = m_pCallback;
        }
        m_state = newState;
    }
    if (pCallback) {
        pCallback(m_pMVC);
    }
}

void MatchVoiceChatImpl::InternalStop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (IsStopping()) {
            return; // Already inside InternalStop() call, probably in another thread
        }

        if (m_state == MatchVoiceChat::stateUninitialized) {
            return; // Already stopped
        }

        m_bStopping = true;
    }

    SendCommand(new CommandStop);

    m_thread.join();
}

void MatchVoiceChatImpl::SendCommand(Command *pCommand)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_Queue.push(pCommand);
    m_eventCommand.notify_one();
}

MatchVoiceChat::EState MatchVoiceChatImpl::GetState()
{
    // std::lock_guard<std::mutex> lock( m_mutex );
    return m_state;
}

bool MatchVoiceChatImpl::ServerConnect(const char *pszVivoxServer, const char *pszIssuer, const char *pszRealm)
{
    if (!InternalInitialize()) {
        LOG_ERR("%s: InternalInitialize() failed", __FUNCTION__);
        return false;
    }
    if (!pszVivoxServer || strlen(pszVivoxServer) < 8) {
        LOG_ERR("%s: invalid Vivox server URL", __FUNCTION__);
        return false;
    }
    SendCommand(new CommandConnect(pszVivoxServer, pszIssuer, pszRealm));
    return true;
}

bool MatchVoiceChatImpl::MatchJoin(const char *pszMatchName, const char *pszMatchToken)
{
    if (!InternalInitialize()) {
        LOG_ERR("%s: InternalInitialize() failed", __FUNCTION__);
        return false;
    }
    SendCommand(new CommandJoin(pszMatchName, pszMatchToken));
    return true;
}

bool MatchVoiceChatImpl::SetLoginCredentials(const char *pszUserId, const char *pszUserToken)
{
    CHECK(!m_sIssuer.empty());

    if (!InternalInitialize()) {
        LOG_ERR("%s: InternalInitialize() failed", __FUNCTION__);
        return false;
    }
    m_sUserID = "." + m_sIssuer + "." + std::string(pszUserId) + ".";
    m_sUserToken = pszUserToken;
    return true;
}

bool MatchVoiceChatImpl::MatchLeave()
{
    if (!InternalInitialize()) {
        LOG_ERR("%s: InternalInitialize() failed", __FUNCTION__);
        return false;
    }
    SendCommand(new CommandLeave());
    return true;
}

bool MatchVoiceChatImpl::ServerDisconnect()
{
    if (!InternalInitialize()) {
        LOG_ERR("%s: InternalInitialize() failed", __FUNCTION__);
        return false;
    }
    SendCommand(new CommandDisconnect());
    return true;
}

void MatchVoiceChatImpl::ThreadProc()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        INFO("ThreadProc() started");
    }

    Initialize();

    while (!IsStopping() || (GetState() != MatchVoiceChat::stateThreadStarted)) {
        HandleCommands();
        HandleState();

        int iTimeout = GetNextEventTimeoutMs();
        std::unique_lock<std::mutex> lock(m_mutex);
        if (iTimeout > 0) {
            m_eventCommand.wait_for(lock, std::chrono::milliseconds(iTimeout));
        } else if (iTimeout < 0) {
            m_eventCommand.wait(lock);
        }
    }

    CHECK(IsStopping());
    CHECK(MatchVoiceChat::stateThreadStarted == GetState());

    // Cleanup
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // delete all remaining commands (if any)
        while (!m_Queue.empty()) {
            delete m_Queue.front();
            m_Queue.pop();
        }

        SetState(MatchVoiceChat::stateUninitialized);
    }
}

void MatchVoiceChatImpl::HandleCommands()
{
    Command *pCmd = NULL;

    do {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (IsStopping() || m_Queue.empty()) {
                return;
            }
            pCmd = m_Queue.front();
            m_Queue.pop();
        }

        switch (pCmd->GetType()) {
            case Command::cmdConnect:
            {
                OnCommandConnect(static_cast<CommandConnect &>(*pCmd));
                break;
            }
            case Command::cmdJoin:
            {
                OnCommandJoin(static_cast<CommandJoin &>(*pCmd));
                break;
            }
            case Command::cmdLeave:
            {
                OnCommandLeave(static_cast<CommandLeave &>(*pCmd));
                break;
            }
            case Command::cmdDisconnect:
            {
                OnCommandDisconnect(static_cast<CommandDisconnect &>(*pCmd));
                break;
            }
            case Command::cmdStop:
            {
                break;
            }
            case Command::cmdCallDelegate:
            {
                OnCommandCallDelegate(static_cast<CommandCallDelegate &>(*pCmd));
                break;
            }
            default:
            {
                INFO("Unexpected command");
                CHECK(false);
                break;
            }
        }
    } while (true);
}

void MatchVoiceChatImpl::OnCommandConnect(CommandConnect &cmd)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_sDesiredVivoxServerURL = cmd.m_sVivoxServer;
        m_sIssuer = cmd.m_sIssuer;
        m_sRealm = cmd.m_sRealm;

        bool bSameVivoxServer = (m_sVivoxServerURL == m_sDesiredVivoxServerURL);

        if (bSameVivoxServer) {
            LOG_INFO("%s: already targeting '%s'", __FUNCTION__, m_sVivoxServerURL.c_str());
            Retry();
            return;
        }

        m_sDesiredMatchName.clear(); // Will leave the current match anyway
        LOG_INFO("%s: will connect to '%s'", __FUNCTION__, m_sDesiredVivoxServerURL.c_str());
    }
}

void MatchVoiceChatImpl::OnCommandJoin(CommandJoin &cmd)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (IsStopping()) {
        LOG_INFO("will not join due to shutdown in progress");
        return;
    }

    m_sMatchToken = cmd.m_sMatchToken;
    m_sDesiredMatchName = cmd.m_sMatchName;
    bool bSameMatch = (m_sDesiredMatchName == m_sMatchName);

    if (bSameMatch) {
        LOG_INFO("%s: match is already set to '%s'", __FUNCTION__, m_sMatchName.c_str());
        Retry();
        return;
    }
    LOG_INFO("%s: will join '%s'", __FUNCTION__, m_sDesiredMatchName.c_str());
}

void MatchVoiceChatImpl::OnCommandLeave(CommandLeave & /* cmd */)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG_INFO("%s", __FUNCTION__);
    m_sDesiredMatchName.clear();
}

void MatchVoiceChatImpl::OnCommandDisconnect(CommandDisconnect & /* cmd */)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG_INFO("%s", __FUNCTION__);
    m_sDesiredVivoxServerURL.clear();
}

void MatchVoiceChatImpl::OnCommandCallDelegate(CommandCallDelegate &cmd)
{
    cmd.m_pf_func(cmd.m_arg0);
}

void MatchVoiceChatImpl::HandleState()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    while (true) {
        bool bIsStopping      = IsStopping();
        bool bSameVivoxServer = (m_sVivoxServerURL == m_sDesiredVivoxServerURL);
        bool bSameMatch       = (m_sMatchName      == m_sDesiredMatchName);

        switch (m_state) {
            case MatchVoiceChat::stateUninitialized:
            case MatchVoiceChat::stateSDKInitFailed:
            {
                return; // Nothing to do
            }
            case MatchVoiceChat::stateThreadStarted:
            case MatchVoiceChat::stateSDKInitializing:
            case MatchVoiceChat::stateSDKUninitializing:
            {
                CHECK(false); // Unexpected state (synchronous operations)
                return;
            }
            case MatchVoiceChat::stateSDKInitialized:
            {
                if (bIsStopping) {
                    Uninitialize();
                    CHECK(m_state == MatchVoiceChat::stateThreadStarted);
                    return;
                }
                if (bSameVivoxServer) {
                    if (m_sDesiredVivoxServerURL.empty()) {
                        return; // Nothing to do
                    } else {
                        Connect();
                    }
                } else {
                    m_sVivoxServerURL = m_sDesiredVivoxServerURL;
                    if (m_sVivoxServerURL.empty()) {
                        return;
                    } else {
                        Connect();
                    }
                }
                break;
            }
            case MatchVoiceChat::stateConnecting:
            case MatchVoiceChat::stateDisconnecting:
            case MatchVoiceChat::stateLoggingIn:
            case MatchVoiceChat::stateLoggingOut:
            case MatchVoiceChat::stateJoiningChannel:
            case MatchVoiceChat::stateLeavingChannel:
            {
                return; // Waiting for the operation to complete
            }
            case MatchVoiceChat::stateConnectRetry:
            {
                if (bIsStopping) {
                    TimerStop();
                    SetState(MatchVoiceChat::stateSDKInitialized);
                    Uninitialize();
                    CHECK(m_state == MatchVoiceChat::stateThreadStarted);
                    return;
                }
                if (bSameVivoxServer) {
                    CHECK(!m_sVivoxServerURL.empty());
                    if (!IsTimeoutElapsed()) {
                        return; // waiting
                    }
                    Connect();
                } else {
                    TimerStop();
                    m_sVivoxServerURL = m_sDesiredVivoxServerURL;
                    if (m_sVivoxServerURL.empty()) {
                        SetState(MatchVoiceChat::stateSDKInitialized);
                        return;
                    } else {
                        Connect();
                    }
                }
                break;
            }
            case MatchVoiceChat::stateConnected:
            {
                CHECK(!m_sVivoxServerURL.empty());
                if (bIsStopping) {
                    Disconnect();
                } else if (bSameVivoxServer) {
                    Login();
                } else {
                    Disconnect();
                }
                break;
            }
            case MatchVoiceChat::stateLoginRetry:
            {
                CHECK(!m_sVivoxServerURL.empty());
                if (bIsStopping) {
                    TimerStop();
                    SetState(MatchVoiceChat::stateConnected);
                    Disconnect();
                } else if (bSameVivoxServer) {
                    if (!IsTimeoutElapsed()) {
                        return; // waiting
                    }
                    Login();
                } else {
                    TimerStop();
                    SetState(MatchVoiceChat::stateConnected);
                    Disconnect();
                }
                break;
            }
            case MatchVoiceChat::stateInvalidLoginCredentials:
            {
                CHECK(!m_sVivoxServerURL.empty());
                if (bIsStopping) {
                    SetState(MatchVoiceChat::stateConnected);
                    Disconnect();
                } else if (bSameVivoxServer) {
                    return; // Leave as is
                } else {
                    SetState(MatchVoiceChat::stateConnected);
                    Disconnect();
                }
                break;
            }
            case MatchVoiceChat::stateLoggedIn:
            {
                CHECK(!m_sVivoxServerURL.empty());
                if (bIsStopping) {
                    Logout();
                } else if (bSameVivoxServer) {
                    m_sMatchName = m_sDesiredMatchName;
                    if (m_sMatchName.empty()) {
                        return; // Nothing to do
                    }
                    RetrieveChannelID();
                } else {
                    Logout();
                }
                break;
            }
            case MatchVoiceChat::stateHaveChannelID:
            {
                CHECK(!m_sVivoxServerURL.empty());
                CHECK(!m_sMatchName.empty());

                if (bIsStopping) {
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                } else if (bSameVivoxServer) {
                    if (bSameMatch) {
                        Join();
                    } else {
                        m_sMatchName = m_sDesiredMatchName;
                        SetState(MatchVoiceChat::stateLoggedIn);
                        if (m_sMatchName.empty()) {
                            return; // Nothing to do
                        }
                        RetrieveChannelID();
                    }
                } else {
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                }
                break;
            }
            case MatchVoiceChat::stateJoinChannelRetry:
            {
                CHECK(!m_sVivoxServerURL.empty());
                CHECK(!m_sMatchName.empty());

                if (bIsStopping) {
                    TimerStop();
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                } else if (bSameVivoxServer) {
                    if (bSameMatch) {
                        if (!IsTimeoutElapsed()) {
                            return; // waiting
                        }
                        TimerStop();
                        Join();
                    } else {
                        TimerStop();
                        m_sMatchName = m_sDesiredMatchName;
                        if (m_sMatchName.empty()) {
                            SetState(MatchVoiceChat::stateLoggedIn);
                            return; // Nothing to do
                        }
                        RetrieveChannelID();
                    }
                } else {
                    TimerStop();
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                }
                break;
            }
            case MatchVoiceChat::stateInvalidChannelCredentials:
            {
                CHECK(!m_sVivoxServerURL.empty());
                CHECK(!m_sMatchName.empty());

                if (bIsStopping) {
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                } else if (bSameVivoxServer) {
                    if (bSameMatch) {
                        return; // Nothing has changed
                    } else {
                        m_sMatchName = m_sDesiredMatchName;
                        SetState(MatchVoiceChat::stateLoggedIn);
                        if (m_sMatchName.empty()) {
                            return; // Nothing to do
                        }
                        RetrieveChannelID();
                    }
                } else {
                    SetState(MatchVoiceChat::stateLoggedIn);
                    Logout();
                }
                break;
            }
            case MatchVoiceChat::stateInMatch:
            {
                CHECK(!m_sVivoxServerURL.empty());
                CHECK(!m_sMatchName.empty());

                if (!bIsStopping && bSameVivoxServer && bSameMatch) {
                    return; // Nothing has changed
                } else {
                    Leave();
                }
                break;
            }
            default:
            {
                CHECK(false); // Unknown state
                break;
            }
        }
    }
}

// return the current time in milliseconds
/* static */
double MatchVoiceChatImpl::NowMs()
{
    double dblMilliseconds;
#ifdef WIN32
    FILETIME t;
    GetSystemTimeAsFileTime(&t);
    dblMilliseconds = t.dwHighDateTime;
    dblMilliseconds *= 4294967296.0;
    dblMilliseconds += t.dwLowDateTime;
    // this is in 100 nanosecond intervals at this point, covert to seconds by dividing by 10*1000000
    dblMilliseconds /= 10 * 1000;
#else
    timeval t;
    gettimeofday(&t, NULL);
    dblMilliseconds = t.tv_sec * 1000000.0;
    dblMilliseconds += t.tv_usec;
    dblMilliseconds /= 1000.0;
#endif
    return dblMilliseconds;
}

void MatchVoiceChatImpl::TimerStart(int iMilliseconds)
{
    m_dblNextEventTime = NowMs() + iMilliseconds;
}

void MatchVoiceChatImpl::TimerStop()
{
    m_dblNextEventTime = 0;
}

bool MatchVoiceChatImpl::IsTimeoutElapsed()
{
    return m_dblNextEventTime != 0 && NowMs() >= m_dblNextEventTime;
}

int MatchVoiceChatImpl::GetNextEventTimeoutMs()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (IsStopping()) {
        return 0;
    }
    if (!m_Queue.empty()) {
        return 0;
    }

    switch (m_state) {
        case MatchVoiceChat::stateSDKInitialized:
        case MatchVoiceChat::stateConnected:
        {
            if (!m_sDesiredVivoxServerURL.empty()) {
                return 0; // Let's connect/Login!
            }
            break;
        }
        case MatchVoiceChat::stateLoggedIn:
        case MatchVoiceChat::stateHaveChannelID:
        {
            if (!m_sDesiredMatchName.empty()) {
                return 0; // Let's retrieve channel ID/join!
            }
        }
        default:
        {
            break;
        }
    }

    if (m_dblNextEventTime <= 0) {
        return -1; // infinity
    }

    double dblRemaining = m_dblNextEventTime - NowMs();

    if (dblRemaining <= 0) {
        return 0; // Timeout elapsed
    }

    return (dblRemaining >= INT_MAX) ? INT_MAX : (int)dblRemaining;
}

#define MVC_STATE_NAME(theStateName) case MatchVoiceChat::state##theStateName: \
    { sName =#theStateName; break; }

std::string MatchVoiceChatImpl::GetStateName(MatchVoiceChat::EState state)
{
    std::string sName;

    switch (state) {
        MVC_STATE_NAME(Uninitialized)
        MVC_STATE_NAME(ThreadStarted)
        MVC_STATE_NAME(SDKInitializing)
        MVC_STATE_NAME(SDKInitFailed)
        MVC_STATE_NAME(SDKInitialized)
        MVC_STATE_NAME(SDKUninitializing)
        MVC_STATE_NAME(Connecting)
        MVC_STATE_NAME(ConnectRetry)
        MVC_STATE_NAME(Connected)
        MVC_STATE_NAME(Disconnecting)
        MVC_STATE_NAME(LoggingIn)
        MVC_STATE_NAME(LoginRetry)
        MVC_STATE_NAME(InvalidLoginCredentials)
        MVC_STATE_NAME(LoggedIn)
        MVC_STATE_NAME(LoggingOut)
        MVC_STATE_NAME(HaveChannelID)
        MVC_STATE_NAME(JoiningChannel)
        MVC_STATE_NAME(JoinChannelRetry)
        MVC_STATE_NAME(InvalidChannelCredentials)
        MVC_STATE_NAME(InMatch)
        MVC_STATE_NAME(LeavingChannel)
        default:
        {
            std::stringstream ss;
            ss << "<unknown state " << (int)state << ">";
            sName = ss.str();
            break;
        }
    }

    return sName;
}

void MatchVoiceChatImpl::SetTimeoutInState(MatchVoiceChat::EState newState, int iMilliseconds)
{
    TimerStart(iMilliseconds);
    SetState(newState);
}

/* statis */
std::string MatchVoiceChatImpl::URLEscape(const char *s, unsigned int nLen)
{
    std::stringstream ss;

    const char *es = MVCEscape::escape(s, nLen);
    if (NULL != es) {
        ss << es;
        MVCEscape::release((void *) es);
    } else {
        ss << s;
    }

    return ss.str();
}

void MatchVoiceChatImpl::SetCallback(MatchVoiceChat::Callback pfnCallback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pCallback = pfnCallback;
}

void MatchVoiceChatImpl::Initialize()
{
    LOG_INFO("Initialize (version %s) ...", (SDK_VERSION));

    CHECK(m_state == MatchVoiceChat::stateThreadStarted);

    SetState(MatchVoiceChat::stateSDKInitializing);

    CHECK(NULL == m_pConnection);

    LogLevel logLevel =
#ifdef _DEBUG
        LogLevelDebug
#else
        LogLevelNone
#endif
    ;

    m_pConnection = new ClientConnection;

    VCSStatus status = m_pConnection->Initialize(this, logLevel, false, false);

    if (status != 0) {
        LOG_ERR("Initialize() returned (%d) %s", status, GetErrorString(status));
        SetState(MatchVoiceChat::stateSDKInitFailed);
        return;
    }

    SetState(MatchVoiceChat::stateSDKInitialized);
}

void MatchVoiceChatImpl::Uninitialize()
{
    LOG_INFO("Uninitializing...");
    CHECK(m_state == MatchVoiceChat::stateSDKInitialized);
    CHECK(m_pConnection);
    SetState(MatchVoiceChat::stateSDKUninitializing);
    m_pConnection->Uninitialize();
    delete m_pConnection;
    m_pConnection = NULL;
    SetState(MatchVoiceChat::stateThreadStarted);
}

void MatchVoiceChatImpl::RetrieveChannelID()
{
    LOG_INFO("Retrieving channel ID...");

    CHECK(!m_sMatchName.empty());
    CHECK(!m_sRealm.empty());
    CHECK(m_state == MatchVoiceChat::stateLoggedIn);

    std::stringstream s;
    s << "sip:confctl-g-" << m_sIssuer << "." << m_sMatchName << "@" << m_sRealm;
    m_sMatchChannelURI = s.str();
    INFO(m_sMatchChannelURI.c_str());

    SetState(MatchVoiceChat::stateHaveChannelID);
}

void MatchVoiceChatImpl::Connect()
{
    CHECK(m_state == MatchVoiceChat::stateSDKInitialized || m_state == MatchVoiceChat::stateConnectRetry);
    CHECK(m_pConnection);
    CHECK(!m_sVivoxServerURL.empty());

    Uri server(m_sVivoxServerURL.c_str());
    LOG_INFO("Connecting to %s", server.ToString());

    SetState(MatchVoiceChat::stateConnecting);

    VCSStatus status = m_pConnection->Connect(server);
    if (status != 0) {
        LOG_ERR("%s: Connect(%s) returned (%d) %s", __FUNCTION__, server.ToString(), status, GetErrorString(status));
        SetTimeoutInState(MatchVoiceChat::stateConnectRetry, TIMEOUT_CONNECT_RETRY);
    }
}

void MatchVoiceChatImpl::Disconnect()
{
    CHECK(m_state == MatchVoiceChat::stateConnected);
    CHECK(m_pConnection);
    CHECK(!m_sVivoxServerURL.empty());

    SetState(MatchVoiceChat::stateDisconnecting);

    Uri server(m_sVivoxServerURL.c_str());
    LOG_INFO("Disconnecting from %s", server.ToString());

    m_pConnection->Disconnect(server);
}

void MatchVoiceChatImpl::Login()
{
    CHECK(m_state == MatchVoiceChat::stateConnected || m_state == MatchVoiceChat::stateLoginRetry);
    CHECK(m_pConnection);
    CHECK(m_sUserID && m_sUserToken);

    LOG_INFO("Logging in ...");

    SetState(MatchVoiceChat::stateLoggingIn);

    AccountName account(m_sUserID.c_str());

    std::string token = m_sUserToken;

    VCSStatus status = m_pConnection->Login(account, token.c_str());
    if (status != 0) {
        LOG_ERR("%s: Login(%s) returned (%d) %s", __FUNCTION__, account.ToString(), status, GetErrorString(status));
        SetTimeoutInState(MatchVoiceChat::stateLoginRetry, TIMEOUT_LOGIN_RETRY);
    }
}

void MatchVoiceChatImpl::Logout()
{
    CHECK(m_state == MatchVoiceChat::stateLoggedIn);
    CHECK(m_pConnection);
    CHECK(m_sUserID);

    LOG_INFO("Logging out ...");

    SetState(MatchVoiceChat::stateLoggingOut);

    AccountName account(m_sUserID.c_str());
    VCSStatus status = m_pConnection->Logout(account);
    if (status != 0) {
        LOG_ERR("%s: Logout(%s) returned (%d) %s", __FUNCTION__, account.ToString(), status, GetErrorString(status));
        LOG_ERR("%s: assuming we are logged out", __FUNCTION__);
        SetState(MatchVoiceChat::stateConnected);
    }
}

void MatchVoiceChatImpl::Join()
{
    CHECK(m_state == MatchVoiceChat::stateHaveChannelID || m_state == MatchVoiceChat::stateJoinChannelRetry);
    CHECK(m_pConnection);
    CHECK(!m_sMatchChannelURI.empty());
    CHECK(m_sUserID && m_sMatchToken);

    SetState(MatchVoiceChat::stateJoiningChannel);

    AccountName account(m_sUserID.c_str());
    Uri channel(m_sMatchChannelURI.c_str());

    std::string token = m_sMatchToken;
    VCSStatus status = m_pConnection->JoinChannel(account, channel, token.c_str());

    if (status != 0) {
        LOG_ERR("%s: m_sdk.JoinChannel(%s) returned (%d) %s", __FUNCTION__, channel.ToString(), status, GetErrorString(status));
        SetTimeoutInState(MatchVoiceChat::stateJoinChannelRetry, TIMEOUT_JOIN_RETRY);
    }
}

void MatchVoiceChatImpl::Leave()
{
    CHECK(m_state == MatchVoiceChat::stateInMatch);
    CHECK(m_pConnection);
    CHECK(!m_sMatchChannelURI.empty());
    CHECK(m_sUserID);

    LOG_INFO("Leaving channel ...");

    SetState(MatchVoiceChat::stateLeavingChannel);

    AccountName account(m_sUserID.c_str());
    Uri channel(m_sMatchChannelURI.c_str());

    VCSStatus status = m_pConnection->LeaveChannel(account, channel);
    if (status != 0) {
        LOG_ERR("%s: LeaveChannel(%s) returned (%d) %s", __FUNCTION__, account.ToString(), status, GetErrorString(status));
        LOG_ERR("%s: assuming we have left the channel", __FUNCTION__);
        SetState(MatchVoiceChat::stateHaveChannelID);
    }
}

void MatchVoiceChatImpl::InvokeOnUIThread(void(pf_func)(void *arg0), void *arg0)
{
    SendCommand(new CommandCallDelegate(pf_func, arg0));
}

# ifdef __ANDROID__
int LogLevel2AndroidLogPrio(IClientApiEventHandler::LogLevel level)
{
    switch (level) {
        case IClientApiEventHandler::LogLevelNone:
            return ANDROID_LOG_FATAL;
        case IClientApiEventHandler::LogLevelError:
            return ANDROID_LOG_ERROR;
        case IClientApiEventHandler::LogLevelWarning:
            return ANDROID_LOG_WARN;
        case IClientApiEventHandler::LogLevelInfo:
            return ANDROID_LOG_INFO;
        case IClientApiEventHandler::LogLevelDebug:
            return ANDROID_LOG_DEBUG;
        case IClientApiEventHandler::LogLevelTrace:
            return ANDROID_LOG_VERBOSE;
        default:
        {
            CHECK(false); // unexpected log level
            return ANDROID_LOG_FATAL;
        }
    }
}
# endif // __ANDROID__

void MatchVoiceChatImpl::onLogStatementEmitted(
        LogLevel level,
        long long nativeMillisecondsSinceEpoch,
        long threadId,
        const char *logMessage
        )
{
# ifdef __ANDROID__
    __android_log_print(LogLevel2AndroidLogPrio(level), "vivoxclientapi", "%5ld [%lld] %s", threadId, nativeMillisecondsSinceEpoch, logMessage);
# else
    (void)level;
    (void)nativeMillisecondsSinceEpoch;
    (void)threadId;
    (void)logMessage;
# endif // __ANDROID__
}

void MatchVoiceChatImpl::onAssert(
        const char *filename,
        int line,
        const char *message
        )
{
# ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_FATAL, "vivoxclientapi", "ASSERTION FAILED at %s:%d: %s", filename, line, message);
# else
    (void)filename;
    (void)line;
    (void)message;
# endif // __ANDROID__
    CHECK(false);
}

void MatchVoiceChatImpl::onConnectCompleted(const Uri &server)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s: %s", __FUNCTION__, server.ToString());

    CHECK(m_state == MatchVoiceChat::stateConnecting);

    if (m_state == MatchVoiceChat::stateConnecting) {
        SetState(MatchVoiceChat::stateConnected);
    }
}

void MatchVoiceChatImpl::onConnectFailed(const Uri &server, VCSStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: %s (%d)", __FUNCTION__, server.ToString(), status);

    CHECK(m_state == MatchVoiceChat::stateConnecting);

    if (m_state == MatchVoiceChat::stateConnecting) {
        SetTimeoutInState(MatchVoiceChat::stateConnectRetry, TIMEOUT_CONNECT_RETRY);
    }
}

void MatchVoiceChatImpl::onDisconnected(const Uri &server, VCSStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s in %s: %s (%d)", __FUNCTION__, GetStateName(m_state).c_str(), server.ToString(), status);

    m_sMatchName.clear();
    m_sVivoxServerURL.clear();
    TimerStop();

    if (
        m_state != MatchVoiceChat::stateUninitialized &&
        m_state != MatchVoiceChat::stateThreadStarted &&
        m_state != MatchVoiceChat::stateSDKInitializing &&
        m_state != MatchVoiceChat::stateSDKInitFailed &&
        m_state != MatchVoiceChat::stateSDKInitialized &&
        m_state != MatchVoiceChat::stateSDKUninitializing)
    {
        SetState(MatchVoiceChat::stateSDKInitialized);
    } else {
        CHECK(false); // Unexpected state
    }
}

void MatchVoiceChatImpl::onLoginCompleted(const AccountName &accountName)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s: '%s'", __FUNCTION__, accountName.ToString());

    CHECK(m_state == MatchVoiceChat::stateLoggingIn);

    if (m_state == MatchVoiceChat::stateLoggingIn) {
        SetState(MatchVoiceChat::stateLoggedIn);
    }
}

void MatchVoiceChatImpl::onInvalidLoginCredentials(const AccountName &accountName)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: '%s'", __FUNCTION__, accountName.ToString());

    CHECK(m_state == MatchVoiceChat::stateLoggingIn);

    if (m_state == MatchVoiceChat::stateLoggingIn) {
        SetState(MatchVoiceChat::stateInvalidLoginCredentials);
    }
}

void MatchVoiceChatImpl::onLoginFailed(const AccountName &accountName, VCSStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: '%s' - (%d) %s", __FUNCTION__, accountName.ToString(), status, GetErrorString(status));

    CHECK(m_state == MatchVoiceChat::stateLoggingIn);

    if (m_state == MatchVoiceChat::stateLoggingIn) {
        SetTimeoutInState(MatchVoiceChat::stateLoginRetry, TIMEOUT_LOGIN_RETRY);
    }
}

void MatchVoiceChatImpl::onLogoutCompleted(const AccountName &accountName)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s: '%s'", __FUNCTION__, accountName.ToString());

    CHECK(m_state == MatchVoiceChat::stateLoggingOut);

    if (m_state == MatchVoiceChat::stateLoggingOut) {
        SetState(MatchVoiceChat::stateConnected);
    }
}

void MatchVoiceChatImpl::onLogoutFailed(const AccountName &accountName, VCSStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: '%s' (%d)", __FUNCTION__, accountName.ToString(), status);

    CHECK(m_state == MatchVoiceChat::stateLoggingOut);

    if (m_state == MatchVoiceChat::stateLoggingOut) {
        LOG_ERR("%s: assuming we are logged out anyway", __FUNCTION__);
        SetState(MatchVoiceChat::stateConnected);
    }
}

void MatchVoiceChatImpl::onChannelJoined(const AccountName &accountName, const Uri &channelUri)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s: '%s' (%s)", __FUNCTION__, accountName.ToString(), channelUri.ToString());

    CHECK(m_state == MatchVoiceChat::stateJoiningChannel);

    if (m_state == MatchVoiceChat::stateJoiningChannel) {
        SetState(MatchVoiceChat::stateInMatch);
    }
}

void MatchVoiceChatImpl::onInvalidChannelCredentials(const AccountName &accountName, const Uri &channelUri)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: '%s' (%s)", __FUNCTION__, accountName.ToString(), channelUri.ToString());

    CHECK(m_state == MatchVoiceChat::stateJoiningChannel);

    if (m_state == MatchVoiceChat::stateJoiningChannel) {
        SetState(MatchVoiceChat::stateInvalidChannelCredentials);
    }
}

void MatchVoiceChatImpl::onChannelJoinFailed(const AccountName &accountName, const Uri &channelUri, VCSStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_ERR("%s: '%s' (%s) - (%d) %s", __FUNCTION__, accountName.ToString(), channelUri.ToString(), status, GetErrorString(status));

    CHECK(m_state == MatchVoiceChat::stateJoiningChannel);

    if (m_state == MatchVoiceChat::stateJoiningChannel) {
        SetTimeoutInState(MatchVoiceChat::stateJoinChannelRetry, TIMEOUT_JOIN_RETRY);
    }
}

void MatchVoiceChatImpl::InternalOnChannelLeft()
{
    LOG_INFO("%s", __FUNCTION__);

    CHECK(m_state == MatchVoiceChat::stateInMatch || m_state == MatchVoiceChat::stateLeavingChannel);

    m_sMatchName.clear();

    if (m_state == MatchVoiceChat::stateInMatch || m_state == MatchVoiceChat::stateLeavingChannel) {
        SetState(MatchVoiceChat::stateLoggedIn);
    }
}

void MatchVoiceChatImpl::onChannelExited(const AccountName &accountName, const Uri &channelUri, VCSStatus reasonCode)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s in %s: '%s' (%s) - (%d)", __FUNCTION__, GetStateName(m_state).c_str(), accountName.ToString(), channelUri.ToString(), reasonCode);

    InternalOnChannelLeft();
}

void MatchVoiceChatImpl::onCallStatsUpdated(const AccountName &accountName, vx_call_stats_t & /* stats */, bool isFinal)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("%s in %s: '%s' (%s)", __FUNCTION__, GetStateName(m_state).c_str(), accountName.ToString(), isFinal ? "final" : "not final");

    if (isFinal) {
        InternalOnChannelLeft();
    }
}

void MatchVoiceChatImpl::Retry()
{
    // TODO: implement operation retry in some states
    // ...
}
