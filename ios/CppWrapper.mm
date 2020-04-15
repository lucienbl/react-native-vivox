#import "CppWrapper.h"
#import <React/RCTLog.h>
#import "../source_files/MatchVoiceChat.h"
#import "AVAudioHelper.h"

@implementation CppWrapper {
    VivoxClientApi::MatchVoiceChat matchVoiceChat;
    AVAudioHelper *avAudioHelper;
}

- (void) connect:(NSString *)server issuer:(NSString *)issuer realm:(NSString *)realm {
    RCTLogInfo(@"Connecting...");
    const char * serverString = [server cStringUsingEncoding:NSASCIIStringEncoding];
    const char * issuerString = [issuer cStringUsingEncoding:NSASCIIStringEncoding];
    const char * realmString = [realm cStringUsingEncoding:NSASCIIStringEncoding];

    matchVoiceChat.ServerConnect(serverString, issuerString, realmString);
    [avAudioHelper setSpeakerPhone:true];
}

- (void) setLoginCredentials:(NSString *)userId userToken:(NSString *)userToken {
    RCTLogInfo(@"Setting credentials...");
    const char * userIdString = [userId cStringUsingEncoding:NSASCIIStringEncoding];
    const char * userTokenString = [userToken cStringUsingEncoding:NSASCIIStringEncoding];

    matchVoiceChat.SetLoginCredentials(userIdString, userTokenString);
}

- (void) joinMatch:(NSString *)channelId joinToken:(NSString *)joinToken {
    RCTLogInfo(@"Joining match...");
    const char * channelIdString = [channelId cStringUsingEncoding:NSASCIIStringEncoding];
    const char * joinTokenString = [joinToken cStringUsingEncoding:NSASCIIStringEncoding];

    matchVoiceChat.MatchJoin(channelIdString, joinTokenString);
}

- (void) disconnect {
    RCTLogInfo(@"Disconnecting...");
    matchVoiceChat.ServerDisconnect();
}

- (void) leaveMatch {
    RCTLogInfo(@"Leaving match...");
    matchVoiceChat.MatchLeave();
}

- (const char *) getStateName {
    return matchVoiceChat.GetStateName(matchVoiceChat.GetState()).c_str();
}

- (int) getState {
    return matchVoiceChat.GetState();
}

- (void) muteMyself:(BOOL *)value {
    bool* mute = (bool *) (value == (BOOL *)YES);
    matchVoiceChat.MuteMyself(mute);
}

- (BOOL) isMuted {
    return matchVoiceChat.IsMuted();
}

- (NSDictionary *) getSpeakingParticipants {
    const std::map<std::string, bool> map = matchVoiceChat.GetSpeakingParticipants();
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    for (auto it = map.begin(); it != map.end(); it++) {
        [dict setObject:@(it->second) forKey:[NSString stringWithUTF8String:it->first.c_str()]];
    }
    return dict;
}

- (void) setParticipantMutedForMe:(NSString *)targetUserId muted:(BOOL *)muted {
    const char * targetUserIdString = [targetUserId cStringUsingEncoding:NSASCIIStringEncoding];
    bool * mute = (bool *) (muted == (BOOL *)YES);

    matchVoiceChat.SetParticipantMutedForMe(targetUserIdString, mute);
}

- (void) setParticipantAudioOutputDeviceVolumeForMe:(NSString *)targetUserId volume:(int *)volume {
    const char * targetUserIdString = [targetUserId cStringUsingEncoding:NSASCIIStringEncoding];

    matchVoiceChat.SetParticipantAudioOutputDeviceVolumeForMe(targetUserIdString, volume);
}

- (void) setAudioOutputDeviceMuted:(BOOL *)muted {
    bool * mute = (bool *) (muted == (BOOL *)YES);

    matchVoiceChat.SetAudioOutputDeviceMuted(mute);
}

@end
