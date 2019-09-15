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
                  
RCT_REMAP_METHOD(getState, getStateResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    int state = [cppWrapper getState];
    if (state) {
        resolve([NSNumber numberWithInt:state]);
    } else {
        NSError *error = NULL;
        reject(@"no_state", @"No state available", error);
    }
}
                  
RCT_REMAP_METHOD(getStateName, getStateNameResolver:(RCTPromiseResolveBlock)resolve
rejecter:(RCTPromiseRejectBlock)reject) {
    const char * state = [cppWrapper getStateName];
    if (state) {
        resolve([NSString stringWithUTF8String:state]);
    } else {
        NSError *error = NULL;
        reject(@"no_state", @"No state available", error);
    }
}

@end
