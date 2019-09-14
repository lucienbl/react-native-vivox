LOCAL_PATH := $(call my-dir)

# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := mvc

LOCAL_CFLAGS += -fvisibility=hidden -fPIC

# # https://source.android.com/source/jack.html#jack_limitations
# # have to have separate state for javac because modern Android Jack toolchain
# # doesn't create *.class files
# javah : LOCAL_PATH = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
# 	javac -d $(NDK_APP_OUT)/$(LOCAL_MODULE) $(LOCAL_PATH)/../src/main/java/com/vivox/mvc/MvcActivity.java
# 	javah -d $(LOCAL_PATH) -classpath :$$ANDROID_HOME/platforms/$(TARGET_PLATFORM)/android.jar:. com.vivox.mvc.MvcActivity

# LOCAL_C_INCLUDES := \
# 	${PROJECT_SOURCE_DIR}/src/util/mvc

LOCAL_SRC_FILES := \
	MatchVoiceChatJNIExternals.cpp \
	MatchVoiceChatExternals.cpp \
	../../ios/vivoxsdkclient/MatchVoiceChat.cpp \
	../../ios/vivoxsdkclient/MatchVoiceChatImpl.cpp \
	../../ios/vivoxsdkclient/Escape.cpp

LOCAL_SRC_FILES += ../../ios/vivoxsdkclient/sha1/sha1.c

LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES := vivoxclientapi-module
LOCAL_SHARED_LIBRARIES := vivox-sdk

include $(BUILD_SHARED_LIBRARY)

# ------------------------------------------------------------------------------

-include $(LOCAL_PATH)/../test/Android.mk

# ------------------------------------------------------------------------------

ifeq (,$(VIVOX_MODULES_DIR))
$(error define $$(VIVOX_MODULES_DIR))
endif

$(call import-add-path,$(VIVOX_MODULES_DIR))
$(call import-module,vivox-sdk)
$(call import-module,vivoxclientapi-module)
