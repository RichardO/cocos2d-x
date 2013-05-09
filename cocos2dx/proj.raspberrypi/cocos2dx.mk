all:

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
CCFLAGS += -MMD -Wall -fPIC -Wno-unused
CXXFLAGS += -MMD -Wall -fPIC -Wno-unused
ARFLAGS = cr

DEFINES += -DRASPBERRYPI

ifdef USE_BOX2D
DEFINES += -DCC_ENABLE_BOX2D_INTEGRATION=1
else
DEFINES += -DCC_ENABLE_CHIPMUNK_INTEGRATION=1
endif

THIS_MAKEFILE := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
ifndef COCOS_ROOT
COCOS_ROOT := $(realpath $(dir $(THIS_MAKEFILE))/../..)
endif
COCOS_SRC = $(COCOS_ROOT)/cocos2dx
OBJ_DIR ?= obj

LIB_DIR = $(COCOS_ROOT)/lib/raspberrypi
BIN_DIR = bin

INCLUDES +=  \
    -I$(COCOS_SRC) \
    -I$(COCOS_SRC)/cocoa \
    -I$(COCOS_SRC)/include \
    -I$(COCOS_SRC)/kazmath/include \
    -I$(COCOS_SRC)/platform/raspberrypi \
    -I$(CROSS_COMPILE_ROOT)/usr/include/arm-linux-gnueabihf \
    -I$(CROSS_COMPILE_ROOT)/opt/vc/include/interface/vmcs_host/linux \
    -I$(CROSS_COMPILE_ROOT)/opt/vc/include/interface/vcos/pthreads \
    -I$(CROSS_COMPILE_ROOT)/opt/vc/include \
    -I$(CROSS_COMPILE_ROOT)/usr/include \
    -I$(CROSS_COMPILE_ROOT)/usr/include/webp \
    -I$(CROSS_COMPILE_ROOT)/usr/include/freetype2 

ifeq ($(DEBUG), 1)
CCFLAGS += -g3 -O0
CXXFLAGS += -g3 -O0
DEFINES += -D_DEBUG -DCOCOS2D_DEBUG=1
OBJ_DIR := $(OBJ_DIR)/debug
LIB_DIR := $(LIB_DIR)/debug
BIN_DIR := $(BIN_DIR)/debug
else
CCFLAGS += -O3
CXXFLAGS += -O3
DEFINES += -DNDEBUG
OBJ_DIR := $(OBJ_DIR)/release
LIB_DIR := $(LIB_DIR)/release
BIN_DIR := $(BIN_DIR)/release
endif

ifndef V
LOG_CC = @echo " CC $@";
LOG_CXX = @echo " CXX $@";
LOG_AR = @echo " AR $@";
LOG_LINK = @echo " LINK $@";
endif

OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)
OBJECTS := $(subst ../,,$(OBJECTS))
OBJECTS := $(subst $(COCOS_ROOT)/,,$(OBJECTS))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS))
DEPS = $(OBJECTS:.o=.d)
CORE_MAKEFILE_LIST := $(MAKEFILE_LIST)
-include $(DEPS)

STATICLIBS_DIR = $(COCOS_ROOT)/cocos2dx/platform/third_party/raspberrypi/libraries
STATICLIBS = $(STATICLIBS_DIR)/libfontconfig.a \
             $(STATICLIBS_DIR)/libfreetype.a \
             $(STATICLIBS_DIR)/libjpeg.a \
             $(STATICLIBS_DIR)/libpng.a \
             $(STATICLIBS_DIR)/libtiff.a \
             $(STATICLIBS_DIR)/libwebp.a \
             $(STATICLIBS_DIR)/libexpat.a \
             $(STATICLIBS_DIR)/libz.a \
             $(STATICLIBS_DIR)/liblzma.a 

SHAREDLIBS += -lpthread -lbcm_host -lGLESv2 -lEGL -ljbig
SHAREDLIBS += -L$(LIB_DIR) -Wl,-rpath,$(abspath $(LIB_DIR))
SHAREDLIBS += -L$(CROSS_COMPILE_ROOT)/opt/vc/lib/ -Wl,-rpath,$(CROSS_COMPILE_ROOT)/opt/vc/lib/
SHAREDLIBS += -L$(COCOS_ROOT)/cocos2dx/platform/third_party/raspberrypi/libraries -Wl,-rpath,$(COCOS_ROOT)/cocos2dx/platform/third_party/raspberrypi/libraries
SHAREDLIBS += -L$(COCOS_ROOT)/CocosDenshion/third_party/raspberrypi/libraries -Wl,-rpath,$(COCOS_ROOT)/CocosDenshion/third_party/raspberrypi/libraries

LIBS = -lrt

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET) core

.PHONY: all clean

# If the parent Makefile defines $(EXECUTABLE) then define this as the target
# and create a 'make run' rule to run the app.
ifdef EXECUTABLE
TARGET := $(BIN_DIR)/$(EXECUTABLE)

all: $(TARGET)

run: $(TARGET)
	cd $(dir $^) && ./$(notdir $^)

.PHONY: run
endif
