LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tinygettext

LOCAL_CFLAGS += -fexceptions

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/src/*.cpp))

### Uncomment these lines if you use SDL2 in your
### project and you want to have support for reading
### .po files from assets
#LOCAL_CFLAGS += -DSDL2_ENABLED
#LOCAL_SHARED_LIBRARIES += SDL2
#LOCAL_LDLIBS += -landroid


include $(BUILD_SHARED_LIBRARY)
