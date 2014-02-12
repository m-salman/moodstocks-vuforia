LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := QCAR-prebuilt
LOCAL_SRC_FILES = $(VFR_SDK)/build/lib/$(TARGET_ARCH_ABI)/libVuforia.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/$(VFR_SDK)/build/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VuforiaWrapper
LOCAL_CFLAGS := -Wno-write-strings -Wno-psabi -DUSE_OPENGL_ES_2_0
LOCAL_LDLIBS := -lGLESv2
LOCAL_SHARED_LIBRARIES := QCAR-prebuilt
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../CommonVuforiaWrapper
LOCAL_SRC_FILES := ../../CommonVuforiaWrapper/MSVCallback.cpp \
                   ../../CommonVuforiaWrapper/MSVCamera.cpp \
                   ../../CommonVuforiaWrapper/MSVController.cpp \
                   ../../CommonVuforiaWrapper/MSVMesh.cpp \
                   ../../CommonVuforiaWrapper/MSVRenderer.cpp \
                   ../../CommonVuforiaWrapper/MSVState.cpp \
                   ../../CommonVuforiaWrapper/MSVTargetInfo.cpp \
                   ../../CommonVuforiaWrapper/MSVTexture.cpp \
                   ../../CommonVuforiaWrapper/MSVTextureCallback.cpp \
                   ../../CommonVuforiaWrapper/MSVTracker.cpp
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := MoodstocksVuforia
LOCAL_SHARED_LIBRARIES := VuforiaWrapper
LOCAL_SRC_FILES := Callback.cpp \
                   EnvStorage.cpp \
                   JNIRenderer.cpp \
                   JNIVuforiaController.cpp \
                   Mesh.cpp \
                   Texture.cpp \
                   TextureCallback.cpp
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)
