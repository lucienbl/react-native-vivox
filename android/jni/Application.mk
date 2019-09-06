APP_PLATFORM := android-14
APP_PIE := true
APP_STL := c++_static
APP_ABI := armeabi-v7a x86 arm64-v8a

APP_CFLAGS += -ffast-math -D__ANDROID__
# APP_CFLAGS += -Wall -Wextra
# APP_CFLAGS += -Werror
APP_CFLAGS += -DHAVE_ASCTIME_R=1 -DHAVE_CTIME_R=1 -DHAVE_GMTIME_R=1 -DHAVE_LOCALTIME_R=1

ifeq (1,$(NDK_DEBUG))
APP_OPTIM := debug
APP_CFLAGS += -O0 -g -DDEBUG -D_DEBUG
# APP_CFLAGS += -ftrapv # trap in of integer overflow
APP_CFLAGS += -DUSE_PARANOID_ALLOCATOR
else
APP_OPTIM := release
APP_CFLAGS += -O3 -DNDEBUG
endif
