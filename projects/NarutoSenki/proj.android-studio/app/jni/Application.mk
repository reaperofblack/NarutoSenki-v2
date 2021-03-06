APP_STL := c++_shared

APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -fsigned-char -fexceptions
APP_LDFLAGS := -latomic

#APP_ALLOW_MISSING_DEPS=true\

#APP_ABI := armeabi-v7a
APP_SHORT_COMMANDS := true


ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DNDEBUG -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_OPTIM := release
endif
