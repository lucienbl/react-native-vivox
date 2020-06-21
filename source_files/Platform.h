#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_SIMULATOR
        #define IOS_SIMULATOR 1
    #endif
#endif
