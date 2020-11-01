LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# For Android Studio
#$(call import-add-path,$(LOCAL_PATH)/../../../../..)
#$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2dx)
#$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2dx/platform/third_party/android/prebuilt)
#$(call import-add-path,$(LOCAL_PATH)/../../../../../external)
#$(call import-add-path,$(LOCAL_PATH)/../../../../../extensions)
#$(call import-add-path,$(LOCAL_PATH)/../../../../../CocosDenshion/android)

$(call import-module,$(LOCAL_C_INCLUDES)/yasio/jni/cxx)
LOCAL_WHOLE_STATIC_LIBRARIES += yasio_static

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := narutosenki/main.cpp \
		    ../../../Classes/AppDelegate.cpp \
            ../../../Classes/ActionButton.cpp \
            ../../../Classes/CharacterBase.cpp \
            ../../../Classes/BGLayer.cpp \
            ../../../Classes/Characters.cpp \
            ../../../Classes/CreditsLayer.cpp \
            ../../../Classes/Effect.cpp \
            ../../../Classes/Element.cpp \
            ../../../Classes/GameLayer.cpp \
            ../../../Classes/GameOver.cpp \
            ../../../Classes/GameScene.cpp \
            ../../../Classes/GearLayer.cpp \
            ../../../Classes/HPBar.cpp \
            ../../../Classes/HudLayer.cpp \
            ../../../Classes/JoyStick.cpp \
            ../../../Classes/LoadLayer.cpp \
            ../../../Classes/NetworkLayer.cpp \
            ../../../Classes/PauseLayer.cpp \
            ../../../Classes/RankingLayer.cpp \
            ../../../Classes/SelectLayer.cpp \
            ../../../Classes/SkillLayer.cpp \
            ../../../Classes/StartMenu.cpp \
            ../../../Classes/MyUtils/CCScrewLayer.cpp \
            ../../../Classes/MyUtils/CCShake.cpp \
            ../../../Classes/MyUtils/CCStrokeLabel.cpp \
            ../../../Classes/MyUtils/KTools.cpp \
            ../../../Classes/MyUtils/MD5ChecksumDefines.cpp \
            ../../../Classes/MyUtils/sqlite3.c \
            ../../../Classes/yasio/ibstream.cpp \
            ../../../Classes/yasio/obstream.cpp \
            ../../../Classes/yasio/xxsocket.cpp \
            ../../../Classes/yasio/platform/yasio_jni.cpp \
            ../../../Classes/yasio/yasio.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes


LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
