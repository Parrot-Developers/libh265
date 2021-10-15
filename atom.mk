
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libh265
LOCAL_CATEGORY_PATH := libs
LOCAL_DESCRIPTION := H.265 bitstream reader/writer library
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -DH265_API_EXPORTS -fvisibility=hidden -std=gnu99
LOCAL_SRC_FILES := \
	src/h265.c \
	src/h265_bitstream.c \
	src/h265_ctx.c \
	src/h265_dump.c \
	src/h265_reader.c \
	src/h265_types.c \
	src/h265_writer.c
LOCAL_PRIVATE_LIBRARIES := \
	json \
	libulog
include $(BUILD_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := h265-dump
LOCAL_DESCRIPTION := H.265 bitstream dump tool
LOCAL_CATEGORY_PATH := libs/h265
LOCAL_CFLAGS := -std=gnu99
LOCAL_SRC_FILES := \
	tools/h265_dump.c
LOCAL_LIBRARIES := \
	json \
	libh265 \
	libulog
include $(BUILD_EXECUTABLE)
