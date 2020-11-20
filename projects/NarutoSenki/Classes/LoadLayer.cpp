#include "LoadLayer.h"

#include <time.h>
#include <fstream>

using namespace std;

LoadLayer::LoadLayer()
{
	tempHeros = nullptr;
	loadNum = 0;
	_hudLayer = nullptr;
	_bgLayer = nullptr;
	_hudLayer = nullptr;
	_isHardCoreMode = true;
	_isRandomChar = false;
}

LoadLayer::~LoadLayer()
{

	CC_SAFE_RELEASE(tempHeros);
}

bool LoadLayer::init()
{

	bool bRet = false;
	do
	{
		CC_BREAK_IF(!CCLayer::init());

		//produce the menu_bar
		CCSprite *menu_bar_b = CCSprite::create("menu_bar2.png");
		menu_bar_b->setAnchorPoint(ccp(0, 0));
		FULL_SCREEN_SPRITE(menu_bar_b);
		addChild(menu_bar_b, 2);

		CCSprite *menu_bar_t = CCSprite::create("menu_bar3.png");
		menu_bar_t->setAnchorPoint(ccp(0, 0));
		menu_bar_t->setPosition(ccp(0, winSize.height - menu_bar_t->getContentSize().height));
		FULL_SCREEN_SPRITE(menu_bar_t);
		addChild(menu_bar_t, 2);

		CCSprite *loading_title = CCSprite::createWithSpriteFrameName("loading_title.png");
		loading_title->setAnchorPoint(ccp(0, 0));
		loading_title->setPosition(ccp(2, winSize.height - loading_title->getContentSize().height - 2));
		addChild(loading_title, 3);

		//produce the cloud
		CCSprite *cloud_left = CCSprite::createWithSpriteFrameName("cloud.png");
		cloud_left->setPosition(ccp(0, 15));
		cloud_left->setFlipX(true);
		cloud_left->setFlipY(true);
		cloud_left->setAnchorPoint(ccp(0, 0));
		addChild(cloud_left, 1);

		CCActionInterval *cmv1 = CCMoveBy::create(1, ccp(-15, 0));
		CCAction *cseq1 = CCRepeatForever::create(CCSequence::create(cmv1, cmv1->reverse(), nullptr));
		cloud_left->runAction(cseq1);

		CCSprite *cloud_right = CCSprite::createWithSpriteFrameName("cloud.png");
		cloud_right->setPosition(ccp(winSize.width - cloud_right->getContentSize().width,
									 winSize.height - (cloud_right->getContentSize().height + 15)));
		cloud_right->setAnchorPoint(ccp(0, 0));
		addChild(cloud_right, 1);

		CCActionInterval *cmv2 = CCMoveBy::create(1, ccp(15, 0));
		CCAction *cseq2 = CCRepeatForever::create(CCSequence::create(cmv2, cmv2->reverse(), nullptr));
		cloud_right->runAction(cseq2);

		bRet = true;
	} while (0);

	return bRet;
};

void LoadLayer::preloadIMG()
{
#define player_is(name) strcmp(player->getCString(), #name) == 0

	CCObject *pObject = nullptr;
	int i = 0;
	const char *path;

	CCARRAY_FOREACH(tempHeros, pObject)
	{

		CCDictionary *tempdict = (CCDictionary *)pObject;

		CCString *player = CCString::create(tempdict->valueForKey("character")->getCString());
		CCTexture2D::PVRImagesHavePremultipliedAlpha(true);

		if (strcmp(player->getCString(), "Kiba") != 0 &&
			strcmp(player->getCString(), "Kakuzu") != 0)
		{
			path = CCString::createWithFormat("Element/Skills/%s_Skill.plist", player->getCString())->getCString();
			addSprites(path);
		}
		KTools::prepareFileOGG(player->getCString());

		path = CCString::createWithFormat("Element/%s/%s.plist", player->getCString(), player->getCString())->getCString();
		addSprites(path);
		// Add extra sprites
		if (player_is(Jiraiya))
		{
			addSprites(mkpath(SageJiraiya));
			KTools::prepareFileOGG("SageJiraiya");
		}
		else if (player_is(Kankuro))
		{
			addSprites(mkpath(Karasu));
			addSprites(mkpath(Sanshouuo));
			addSprites(mkpath(Saso));
		}
		else if (player_is(Chiyo))
		{
			addSprites(mkpath(Parents));
		}
		else if (player_is(Kakuzu))
		{
			addSprites(mkpath(MaskRaidon));
			addSprites(mkpath(MaskFudon));
			addSprites(mkpath(MaskKadon));
		}
		else if (player_is(Naruto))
		{
			addSprites(mkpath(RikudoNaruto));
			addSprites(mkpath(SageNaruto));
			addSprites(mkpath(Kurama));
			KTools::prepareFileOGG("SageNaruto");
			KTools::prepareFileOGG("RikudoNaruto");
		}
		else if (player_is(Lee))
		{
			addSprites(mkpath(RockLee));
		}
		else if (player_is(Tsunade))
		{
			addSprites(mkpath(Slug));
		}
		else if (player_is(Kakashi))
		{
			addSprites(mkpath(DogWall));
		}
		else if (player_is(Deidara))
		{
			addSprites(mkpath(Centipede));
		}
		else if (player_is(Pain))
		{
			addSprites(mkpath(DevaPath));
			addSprites(mkpath(AsuraPath));
			addSprites(mkpath(AnimalPath));
		}
		else if (player_is(Sasuke))
		{
			addSprites(mkpath(ImmortalSasuke));
			KTools::prepareFileOGG("ImmortalSasuke");
		}
		else if (player_is(Kiba))
		{
			addSprites(mkpath(Akamaru));
		}

		if (i == 0 || ((i == 1 || i == 2) && _isHardCoreMode))
		{

			CCSprite *loadingAvator = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_Walk_01.png", player->getCString())->getCString());
			loadingAvator->setFlipX(true);
			loadingAvator->setPosition(ccp(winSize.width - 100 + i * 16, 30));
			loadingAvator->setAnchorPoint(ccp(0, 0));

			CCArray *animeFrames = CCArray::create();
			CCString *str;

			const char *file = CCString::createWithFormat("%s_Walk_", player->getCString())->getCString();
			int frameCount;
			if (strcmp(player->getCString(), "Konan") == 0)
			{
				frameCount = 1;
			}
			else
			{
				frameCount = 7;
			}

			for (int i = 1; i < frameCount; i++)
			{
				str = CCString::createWithFormat("%s%02d.png", file, i);
				CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str->getCString());
				animeFrames->addObject(frame);
			}

			CCAnimation *tempAnimation = CCAnimation::createWithSpriteFrames(animeFrames, float(1.0 / 10.0));
			CCAction *tempAction = CCAnimate::create(tempAnimation);
			CCArray *seqArray = CCArray::createWithObject(tempAction);
			CCAction *seq;
			seq = CCRepeatForever::create(CCSequence::create(seqArray));

			addChild(loadingAvator);
			loadingAvator->runAction(seq);
		}

		i++;
	};

	tempHeros->retain();
	if (_isHardCoreMode)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Element/Roshi/Roshi.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Element/Han/Han.plist");
		KTools::prepareFileOGG("Roshi");
		KTools::prepareFileOGG("Han");
	}
	addSprites(kFlog_Kotetsu);
	addSprites(kFlog_FemalePain);

	addSprites(kFlog_Izumo);
	addSprites(kFlog_Kakashi);

	addSprites(kFlog_Pain);
	addSprites(kFlog_Obito);

	KTools::prepareFileOGG("Effect");
	KTools::prepareFileOGG("Ougis");

	try
	{
		srand((int)time(0));
		int num = rand() % 3 + 1;
		CCSprite *tips = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("tip%d.png", num)->getCString());
		tips->setPosition(ccp(winSize.width / 2, winSize.height / 2));
		addChild(tips);
	}
	catch (...)
	{
		CCLOG("error");
		return;
	}

	CCSprite *loading = CCSprite::createWithSpriteFrameName("loading_font.png");

	loading->setPosition(ccp(winSize.width - 120, 45));
	CCActionInterval *fade = CCFadeOut::create(1.0f);
	CCAction *fadeseq = CCRepeatForever::create(CCSequence::create(fade, fade->reverse(), nullptr));
	addChild(loading);
	loading->runAction(fadeseq);

	scheduleOnce(schedule_selector(LoadLayer::playBGM), 1.0f);
	scheduleOnce(schedule_selector(LoadLayer::onLoadFinish), 3.0f);
}

void LoadLayer::playBGM(float dt)
{
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(LOADING_MUSIC, false);
}

void LoadLayer::preloadAudio()
{

	const char *bg_src;
	if (_isHardCoreMode)
	{
		bg_src = "blue_bg.png";
	}
	else
	{
		bg_src = "red_bg.png";
	}
	CCSprite *bgSprite = CCSprite::create(bg_src);

	FULL_SCREEN_SPRITE(bgSprite);
	bgSprite->setAnchorPoint(ccp(0, 0));
	bgSprite->setPosition(ccp(0, 0));
	addChild(bgSprite, -5);

	// KTools::prepareFileMD5();
	// std::string str2 = "fkhfnPG8";
	// KTools::decode(str2);
	// std::string filePath = CCFileUtils::sharedFileUtils()->getWritablePath() + CCString::createWithFormat("%s.xml", str2.c_str())->getCString();
	// std::string strFileMD5 = CMD5Checksum::GetMD5(filePath);
	// remove(filePath.c_str());
	// CCString *code1 = CCString::createWithFormat("%s%s", strFileMD5.substr(16, 5).c_str(), strFileMD5.substr(6, 5).c_str());
	// if (strcmp(code1->getCString(), "e0213bffe1") != 0)
	// {
	// const char *soundpath = "Audio";
	//return;
	//KTools::dfsFolder(soundpath,0,1);
	// }

	preloadIMG();
}

void LoadLayer::onLoadFinish(float dt)
{

	CCScene *gameScene = CCScene::create();

	_gameLayer = GameLayer::create();
	_gameLayer->Heros = tempHeros;
	_gameLayer->_isHardCoreGame = _isHardCoreMode;
	_gameLayer->_isRandomChar = _isRandomChar;
	_gameLayer->initHeros();

	_bgLayer = BGLayer::create();
	_bgLayer->setDelegate(_gameLayer);

	_hudLayer = HudLayer::create();
	_hudLayer->setDelegate(_gameLayer);
	//_hudLayer->initHeroInterface();

	_gameLayer->setHudLayer(_hudLayer);
	_gameLayer->setTotalKills(CCString::create("0"));
	_gameLayer->setTotalTM(CCString::create("0"));

	gameScene->addChild(_gameLayer, GlTag);
	gameScene->addChild(_bgLayer, BgTag);
	gameScene->addChild(_hudLayer, HudTag);

	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, gameScene));
}
