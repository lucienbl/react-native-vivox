LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_MODULE := mvc

LOCAL_CFLAGS += -fvisibility=hidden -fPIC

VIVOX_MODULES_DIR=/Volumes/Lucien/Tools/vivox/5.8.0-android/SDK/Libraries

LOCAL_SRC_FILES := \
	MatchVoiceChatJNIExternals.cpp \
	MatchVoiceChatExternals.cpp \
	../../source_files/MatchVoiceChat.cpp \
	../../source_files/MatchVoiceChatImpl.cpp \
	../../source_files/Escape.cpp

LOCAL_SRC_FILES += ../../source_files/sha1/sha1.c

LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := vivox-sdk

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vivox-sdk
LOCAL_SRC_FILES := Libraries/$(APP_VARIANT)/$(TARGET_ARCH_ABI)/libvivox-sdk.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)
