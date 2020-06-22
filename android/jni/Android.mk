LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include include ../../source_files/vivoxclientapi/include
LOCAL_MODULE := mvc 
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_SRC_FILES := \
    MatchVoiceChatJNIExternals.cpp \
    MatchVoiceChatExternals.cpp \
    ../../source_files/vivoxclientapi/accountname.cpp \
    ../../source_files/vivoxclientapi/audiodeviceid.cpp \
    ../../source_files/vivoxclientapi/clientconnection.cpp \
    ../../source_files/vivoxclientapi/debugclientapieventhandler.cpp \
    ../../source_files/vivoxclientapi/easy.cpp \
    ../../source_files/vivoxclientapi/memallocators.cpp \
    ../../source_files/vivoxclientapi/uri.cpp \
    ../../source_files/vivoxclientapi/util.cpp \
    ../../source_files/vivoxclientapi/vivoxclientsdk.cpp \
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
LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)