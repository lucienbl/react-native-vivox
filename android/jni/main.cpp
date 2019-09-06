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

// #include "mvc.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#ifdef WIN32
#else
#include <signal.h>
#endif

#include "MatchVoiceChat.h"

#define TEST_VIVOX_SERVER_URL "http://vd1.vivox.com/api2"
#define TEST_MATCH_SERVER_URL "http://gameserver1.vivox.com/cgi-bin/matchid.pl"
#define TEST_MATCH_NAME       "test"

// MatchVoiceChatAgent mvc;
VivoxClientApi::MatchVoiceChat g_MatchVoiceChat;

#ifdef WIN32
BOOL CtrlHandler(DWORD fdwCtrlType)
{
    if (fdwCtrlType == CTRL_C_EVENT) {
        // mvc.PrintLog("control", "error", "System", "Control-C received - printing accumulated stats and terminating.");
        // mvc.Interrupt();
        g_MatchVoiceChat.Stop();
        return FALSE;
    }
    return TRUE;
}
#else
void CtrlHandler(int)
{
    signal(SIGINT, SIG_DFL);
    // mvc.PrintLog("control", "error", "System", "Control-C received - printing accumulated stats and terminating.");
    // mvc.Interrupt();
    g_MatchVoiceChat.ServerDisconnect();
}
#endif


// Splits up the user input
std::vector<std::string> split(const char *s)
{
    std::vector<std::string> ss;
    std::string t = s;
    size_t lastpos = 0;
    for (;;) {
        size_t pos = t.find(" ", lastpos);
        if (pos > lastpos && pos != 0) {
            if (pos == std::string::npos) {
                if (lastpos < t.size()) {
                    ss.push_back(t.substr(lastpos, t.size() - lastpos));
                }
                break;
            } else {
                ss.push_back(t.substr(lastpos, pos - lastpos));
            }
        }
        lastpos = pos + 1;
        if (lastpos >= t.size()) {
            break;
        }
    }
    return ss;
}

void MyCallback(VivoxClientApi::MatchVoiceChat *pMVC)
{
    printf("%s: state = %s\n", __FUNCTION__, VivoxiClientApi::MatchVoiceChat::GetStateName(pMVC->GetState()).c_str());
}

///////////////  MAIN  ////////////////

int main(int argc, const char *argv[])
{
#ifdef WIN32
    if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE)) {
        mvc.PrintLog("control", "error", "System", "Failed to set console handler.");
        return 1;
    }
#else
    signal(SIGINT, CtrlHandler);
#endif
    int status = 1;

    g_MatchVoiceChat.SetCallback(MyCallback);
    while (std::cin.good()) {
        std::string sCmd;
        std::cout << std::endl << "Command: ";
        std::getline(std::cin, sCmd);
        std::vector<std::string> vCmdParts = split(sCmd.c_str());
        size_t iPartsCount = vCmdParts.size();

        if (iPartsCount > 0) {
            std::string sCommandName = vCmdParts.at(0);
            if (sCommandName == "connect") {
                if (3 == iPartsCount) {
                    g_MatchVoiceChat.ServerConnect(vCmdParts.at(1).c_str(), vCmdParts.at(2).c_str());
                } else {
                    std::cout << "Error: 'connect' command must have 2 parameters - vivox_server_url match_server_url" << std::endl;
                }
            } else if (sCommandName == "c") {
                if (1 == iPartsCount) {
                    g_MatchVoiceChat.ServerConnect(TEST_VIVOX_SERVER_URL, TEST_MATCH_SERVER_URL);
                } else {
                    std::cout << "'c' command has no parameters" << std::endl;
                }
            } else if (sCommandName == "join") {
                if (2 == iPartsCount) {
                    g_MatchVoiceChat.MatchJoin(vCmdParts.at(1).c_str());
                } else {
                    std::cout << "Error: 'join' command must have 1 parameter - match_name" << std::endl;
                }
            } else if (sCommandName == "j") {
                if (1 == iPartsCount) {
                    g_MatchVoiceChat.MatchJoin(TEST_MATCH_NAME);
                } else {
                    std::cout << "'j' command has no parameters" << std::endl;
                }
            } else if (sCommandName == "t") {
                if (1 == iPartsCount) {
                    g_MatchVoiceChat.ServerConnect(TEST_VIVOX_SERVER_URL, TEST_MATCH_SERVER_URL);
                    g_MatchVoiceChat.MatchJoin(TEST_MATCH_NAME);
                } else {
                    std::cout << "'t' command has no parameters" << std::endl;
                }
            } else if (sCommandName == "leave" || sCommandName == "l") {
                if (1 == iPartsCount) {
                    g_MatchVoiceChat.MatchLeave();
                } else {
                    std::cout << "'leave' command has no parameters" << std::endl;
                }
            } else if (sCommandName == "disconnect" || sCommandName == "d") {
                if (1 == iPartsCount) {
                    g_MatchVoiceChat.ServerDisconnect();
                } else {
                    std::cout << "'disconnect' command has no parameters" << std::endl;
                }
            } else if (sCommandName == "exit" || sCommandName == "x" || sCommandName == "quit" || sCommandName == "q") {
                break;
            } else {
                std::cout << "Unknown command '" << sCommandName << "'" << std::endl;
            }
        }
    }
    /*
    if ( !mvc.ParseInput(argc, argv) )
      status = mvc.Run();
    */
    g_MatchVoiceChat.ServerDisconnect();
    return status;
}
