LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := lib_socket_static

LOCAL_MODULE_FILENAME := libsocket

LOCAL_CFLAGS += -DANDROID_NDK=1

LOCAL_SRC_FILES := odsocket/ODSocket.cpp \
ExtensionRequestMessage.cpp \
ExtensionResponseMessage.cpp \
LoginRequestMessage.cpp \
LoginResponseMessage.cpp \
MsgSeqNoManager.cpp \
RecvBuffer.cpp \
SocketClient.cpp \
SocketEncryptManager.cpp \
SocketMessageReaderImpl.cpp \
SocketMessageWriterImpl.cpp \
SocketUtil.cpp \
ZLibString.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_LDLIBS += -llog

LOCAL_STATIC_LIBRARIES := 

include $(BUILD_STATIC_LIBRARY)

