APP_STL := c++_static
# APP_STL := c++_shared

APP_ABI := armeabi-v7a x86 x86_64 arm64-v8a

APP_CFLAGS += -ffast-math -D__ANDROID__
# APP_CFLAGS += -Wall -Wextra
# APP_CFLAGS += -Werror
APP_CFLAGS += -DHAVE_ASCTIME_R=1 -DHAVE_CTIME_R=1 -DHAVE_GMTIME_R=1 -DHAVE_LOCALTIME_R=1 -D__STDC_FORMAT_MACROS=1

ifeq (1,$(NDK_DEBUG))
APP_VARIANT := Debug
APP_OPTIM := debug
APP_CFLAGS += -O0 -g -DDEBUG -D_DEBUG
APP_CFLAGS += -DUSE_PARANOID_ALLOCATOR
# APP_CFLAGS += -ftrapv # trap in of integer overflow
else
APP_VARIANT := Release
APP_OPTIM := release
APP_CFLAGS += -O3 -DNDEBUG
endif
APP_CPPFLAGS += -std=gnu++11
