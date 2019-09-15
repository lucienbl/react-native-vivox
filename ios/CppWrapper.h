#import <Foundation/Foundation.h>

@interface CppWrapper : NSObject

- (void) connect:(NSString *)server issuer:(NSString *)issuer realm:(NSString *)realm;
- (void) setLoginCredentials:(NSString *)userId userToken:(NSString *)userToken;
- (void) joinMatch:(NSString *)channelId joinToken:(NSString *)joinToken;
- (void) disconnect;
- (void) leaveMatch;
- (const char *) getStateName;
- (int) getState;

@end
