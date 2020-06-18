#include <TargetConditionals.h>
#if !TARGET_OS_SIMULATOR
#import "Vivox.h"
#import <Foundation/Foundation.h>
#import <React/RCTLog.h>
#import "CppWrapper.h"

@implementation Vivox {
    CppWrapper *cppWrapper;
}

RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(connect:(NSString *)server issuer:(NSString *)issuer realm:(NSString *)realm) {
    cppWrapper = [[CppWrapper alloc] init];
    [cppWrapper connect:server issuer:issuer realm:realm];
}

RCT_EXPORT_METHOD(setLoginCredentials:(NSString *)userId userToken:(NSString *)userToken) {
    [cppWrapper setLoginCredentials:userId userToken:userToken];
}

RCT_EXPORT_METHOD(joinMatch:(NSString *)channelId joinToken:(NSString *)joinToken) {
    [cppWrapper joinMatch:channelId joinToken:joinToken];
}

RCT_EXPORT_METHOD(disconnect) {
    [cppWrapper disconnect];
}

RCT_EXPORT_METHOD(leaveMatch) {
    [cppWrapper leaveMatch];
}

RCT_EXPORT_METHOD(muteMyself:(BOOL *)value) {
    [cppWrapper muteMyself:value];
}

RCT_EXPORT_METHOD(setParticipantMutedForMe:(NSString *)targetUserId muted:(BOOL *)muted) {
    [cppWrapper setParticipantMutedForMe:targetUserId muted:muted];
}

RCT_EXPORT_METHOD(setParticipantAudioOutputDeviceVolumeForMe:(NSString *)targetUserId volume:(int)volume) {
    [cppWrapper setParticipantAudioOutputDeviceVolumeForMe:targetUserId volume:volume];
}

RCT_EXPORT_METHOD(setAudioOutputDeviceMuted:(BOOL *)muted) {
    [cppWrapper setAudioOutputDeviceMuted:muted];
}

RCT_REMAP_METHOD(getState, getStateResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    int state = [cppWrapper getState];
    resolve([NSNumber numberWithInt:state]);
}

RCT_REMAP_METHOD(getStateName, getStateNameResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    const char * state = [cppWrapper getStateName];
    resolve([NSString stringWithUTF8String:state]);
}

RCT_REMAP_METHOD(isMuted, isMutedResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    resolve([NSNumber numberWithBool:[cppWrapper isMuted]]);
}

RCT_REMAP_METHOD(getSpeakingParticipants, getSpeakingParticipantsResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    resolve([cppWrapper getSpeakingParticipants]);
}

@end
#endif
