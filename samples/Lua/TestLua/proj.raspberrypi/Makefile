EXECUTABLE = TestLua

COCOS_ROOT = ../../../..
INCLUDES =  -I../ -I../Classes -I$(COCOS_ROOT)/CocosDenshion/include \
    -I$(COCOS_ROOT)/scripting/lua/lua \
    -I$(COCOS_ROOT)/scripting/lua/tolua \
    -I$(COCOS_ROOT)/scripting/lua/cocos2dx_support

SOURCES = main.cpp ../Classes/AppDelegate.cpp

SHAREDLIBS += -lcocos2d -lcocosdenshion -llua
COCOS_LIBS = $(LIB_DIR)/libcocos2d.so $(LIB_DIR)/libcocosdenshion.so $(LIB_DIR)/liblua.so

##Using OpenAL
SHAREDLIBS += -lopenal -lalut

ifneq ($(NOVORBIS),1)
SHAREDLIBS += -logg -lvorbis -lvorbisfile
else
DEFINES += -DDISABLE_VORBIS
endif

include $(COCOS_ROOT)/cocos2dx/proj.raspberrypi/cocos2dx.mk

$(TARGET): $(OBJECTS) $(STATICLIBS) $(COCOS_LIBS) $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	cp -R -n ../../../../samples/Cpp/TestCpp/Resources ../../../../samples/Lua/TestLua
	$(LOG_LINK)$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(SHAREDLIBS) $(STATICLIBS) $(LIBS)

$(OBJ_DIR)/%.o: ../%.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(VISIBILITY) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(VISIBILITY) -c $< -o $@
