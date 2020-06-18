#include <TargetConditionals.h>
#if !TARGET_OS_SIMULATOR
#import <React/RCTBridgeModule.h>

@interface Vivox : NSObject <RCTBridgeModule>

@end
#endif
