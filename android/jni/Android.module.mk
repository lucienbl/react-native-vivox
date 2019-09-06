LOCAL_PATH := $(call my-dir)

# --------------------------------------------------------------------------------
# libmvc.so
# --------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := vivox-mvc
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libmvc$(TARGET_SONAME_EXTENSION)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

# --------------------------------------------------------------------------------
