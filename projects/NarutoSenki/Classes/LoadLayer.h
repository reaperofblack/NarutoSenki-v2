#pragma once
#include "Defines.h"
#include "GameLayer.h"
#include "BGLayer.h"
#include "HudLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../../../cocos2dx/platform/android/jni/JniHelper.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

class LoadLayer : public CCLayer
{
public:
	LoadLayer();
	~LoadLayer();

	CCArray *tempHeros;
	bool init();

	int loadNum;

	CREATE_FUNC(LoadLayer);

	CC_SYNTHESIZE_RETAIN(GameLayer *, _gameLayer, GameLayer);
	CC_SYNTHESIZE_RETAIN(BGLayer *, _bgLayer, BGLayer);
	CC_SYNTHESIZE_RETAIN(HudLayer *, _hudLayer, HudLayer);
	bool _isHardCoreMode;
	bool _isRandomChar;

	void preloadAudio();
	void preloadIMG();
	void playBGM(float dt);
	void onLoadFinish(float dt);

protected:
	void perloadCharIMG(const CCString *player);
	void setLoadingAnimation(const CCString *player, int index);
};
