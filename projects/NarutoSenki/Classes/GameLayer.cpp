#include "Defines.h"
#include "CharacterBase.h"
#include "GameLayer.h"
#include "BGLayer.h"
#include "HudLayer.h"
#include "StartMenu.h"
#include "Core/Provider.hpp"

GameLayer *_gLayer = nullptr;
bool _isFullScreen = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
GLFWwindow *_window = nullptr;
#endif

GameLayer::GameLayer()
{
	_isAttackButtonRelease = true;
	_isSkillFinish = true;

	_KonohaFlogArray = nullptr;
	_AkatsukiFlogArray = nullptr;
	_CharacterArray = nullptr;

	_second = 0;
	_minute = 0;
	_playNum = 2;

	kEXPBound = 25;
	aEXPBound = 25;

	totalKills = nullptr;
	totalTM = nullptr;
	_isShacking = false;
	_isSurrender = false;
	_isGuardian = false;

	_isStarted = false;
	_isExiting = false;

	ougisChar = nullptr;
	controlChar = nullptr;
	_isOugis2Game = false;
	_isHardCoreGame = false;
	_isRandomChar = false;
	team = 1;
	currentPlayer = nullptr;
	isPosting = false;
	postTime = 0;

	_isGear = false;
	_isPause = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_lastPressedMovementKey = -100;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	_lastPressedMovementKey = -100;
	_window = CCEGLView::sharedOpenGLView()->m_window;
#endif
}

GameLayer::~GameLayer()
{
	//CC_SAFE_RELEASE(totalKills);
	CC_SAFE_RELEASE(totalTM);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CCDirector::sharedDirector()->getOpenGLView()->setAccelerometerKeyHook(nullptr);
	CC_SAFE_DELETE(_gLayer);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	glfwSetKeyCallback(_window, nullptr);
	CC_SAFE_DELETE(_gLayer);
#endif
}

bool GameLayer::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!CCLayer::init());
		CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);

		setTouchEnabled(true);

		bRet = true;

	} while (0);

	_gLayer = this;

	return bRet;
}

void GameLayer::onEnter()
{
	if (_isExiting)
	{
		onLeft();
		return;
	}

	if (currentPlayer && !ougisChar)
	{
		if (currentPlayer->getActionState() == State::WALK)
		{
			currentPlayer->idle();
		}
	}

	CCLayer::onEnter();

	if (_isSurrender)
	{
		onGameOver(false);
	}
}

void GameLayer::onExit()
{
	CCLayer::onExit();

	if (_isExiting)
	{
		_isExiting = false;
		CC_SAFE_RELEASE(_TowerArray);
		CC_SAFE_RELEASE(_KonohaFlogArray);
		CC_SAFE_RELEASE(_AkatsukiFlogArray);
		CC_SAFE_RELEASE(_CharacterArray);
		_gLayer = nullptr;
	}
}

void GameLayer::initTileMap()
{

	if (_isHardCoreGame)
	{
		srand((int)time(0));
		randomMap = random(4) + 1;
	}
	else
	{
		randomMap = 0;
	}

	const char *filePath;
	if (randomMap == 0)
	{
		filePath = S_MAP01;
	}
	else if (randomMap == 1)
	{
		filePath = S_MAP02;
	}
	else if (randomMap == 2)
	{
		filePath = S_MAP03;
	}
	else if (randomMap == 3)
	{
		filePath = S_MAP04;
	}
	else if (randomMap == 4)
	{
		filePath = S_MAP05;
	}

	// std::string key = KTools::getKeycode(filePath);
	// int id = KTools::checkMD5(filePath);
	// if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// {
	// if (strcmp(key.c_str(), KeyList[id]) != 0)
	// {
	// 	return;
	// }
	// }

	currentMap = CCTMXTiledMap::create(filePath);

	addChild(currentMap, currentMapTag);
}

void GameLayer::initGard()
{

	int index = 0;
	srand((int)time(0));
	index = random(2);
	Hero *Guardian;

	const char *groupName;

	if (team > 0)
	{
		groupName = Akatsuki;
	}
	else
	{
		groupName = Konoha;
	}
	if (index == 0)
	{
		Guardian = Provider::create(CCString::create(Guardian_Roshi), CCString::create("Com"), CCString::create(groupName));
	}
	else if (index == 1)
	{
		Guardian = Provider::create(CCString::create(Guardian_Han), CCString::create("Com"), CCString::create(groupName));
	}

	if (team > 0)
	{
		Guardian->setPosition(ccp(2800, 80));
		Guardian->setSpawnPoint(ccp(2800, 80));
	}
	else
	{
		Guardian->setPosition(ccp(272, 80));
		Guardian->setSpawnPoint(ccp(272, 80));
	}
	Guardian->setDelegate(this);

	addChild(Guardian, -Guardian->getPositionY());
	Guardian->setLV(6);
	Guardian->setHPbar();
	Guardian->setShadows();
	Guardian->setCharNO(7);

	Guardian->idle();
	CCDictionary *callValue = CCDictionary::create();
	callValue->setObject(CCString::create("smk"), 1);
	Guardian->setSkillEffect(Guardian, callValue);

	Guardian->doAI();

	_CharacterArray->addObject(Guardian);
	_hudLayer->addMapIcon();
	_isGuardian = true;
}

void GameLayer::initHeros()
{

	initTileMap();
	initEffects();

	_CharacterArray = CCArray::create();
	addSprites("Element/hpBar/hpBar.plist");
	CCObject *pObject = nullptr;
	int i = 0;

	CCTMXObjectGroup *group = currentMap->objectGroupNamed("object");
	CCArray *objectArray = group->getObjects();

	_isOugis2Game = true;
	//4v4

	if (Cheats > 10)
	{

		CCARRAY_FOREACH(Heros, pObject)
		{

			CCDictionary *tempdict = (CCDictionary *)pObject;

			CCString *player = CCString::create(tempdict->valueForKey("character")->getCString());
			CCString *role = CCString::create(tempdict->valueForKey("role")->getCString());
			CCString *group = CCString::create(tempdict->valueForKey("group")->getCString());

			if (i == 0)
			{
				spawnPoint = ccp(432, 80);
			}
			else if (i == 7)
			{
				spawnPoint = ccp(2608, 80);
			}
			else
			{
				CCObject *mapObject = objectArray->objectAtIndex(i - 1);
				CCDictionary *mapdict = (CCDictionary *)mapObject;
				int x = ((CCString *)mapdict->objectForKey("x"))->intValue();
				int y = ((CCString *)mapdict->objectForKey("y"))->intValue();
				spawnPoint = ccp(x, y);
			}

			if (i == 0)
			{
				currentPlayer = (CharacterBase *)Provider::create(player, role, group);
				currentPlayer->setDelegate(this);
				currentPlayer->setPosition(spawnPoint);
				currentPlayer->setSpawnPoint(spawnPoint);
				addChild(currentPlayer, -currentPlayer->getPositionY());
				currentPlayer->setHPbar();
				currentPlayer->setShadows();
				currentPlayer->idle();
				currentPlayer->setCharNO(i + 1);
				currentPlayer->schedule(schedule_selector(CharacterBase::setRestore2), 1.0f);
				_CharacterArray->addObject(currentPlayer);
			}
			else
			{

				Hero *Com = Provider::create(player, role, group);
				Com->setDelegate(this);
				Com->setPosition(spawnPoint);
				Com->setSpawnPoint(spawnPoint);
				addChild(Com, -Com->getPositionY());
				Com->setHPbar();
				Com->setShadows();
				Com->idle();
				Com->setCharNO(i + 1);
				Com->schedule(schedule_selector(CharacterBase::setRestore2), 1.0f);
				_CharacterArray->addObject(Com);
			}
			i++;
		}
	}
	else
	{
		CCARRAY_FOREACH(Heros, pObject)
		{

			CCDictionary *tempdict = (CCDictionary *)pObject;

			CCString *player = CCString::create(tempdict->valueForKey("character")->getCString());
			CCString *role = CCString::create(tempdict->valueForKey("role")->getCString());
			CCString *group = CCString::create(tempdict->valueForKey("group")->getCString());

			int mapPos = i;
			if (strcmp(group->getCString(), Akatsuki) == 0)
			{
				if (mapPos <= MapPosCount)
				{
					mapPos += 3;
				}
			}
			else
			{
				if (mapPos > MapPosCount)
				{
					mapPos -= 3;
				}
			}

			CCObject *mapObject = objectArray->objectAtIndex(mapPos);
			CCDictionary *mapdict = (CCDictionary *)mapObject;
			int x = ((CCString *)mapdict->objectForKey("x"))->intValue();
			int y = ((CCString *)mapdict->objectForKey("y"))->intValue();
			spawnPoint = ccp(x, y);

			if (i == 0)
			{

				if (strcmp(group->getCString(), Akatsuki) == 0)
				{
					team = 0;
				}

				currentPlayer = (CharacterBase *)Provider::create(player, role, group);
				currentPlayer->setDelegate(this);
				currentPlayer->setPosition(spawnPoint);
				currentPlayer->setSpawnPoint(spawnPoint);
				addChild(currentPlayer, -currentPlayer->getPositionY());
				currentPlayer->setHPbar();
				currentPlayer->setShadows();
				currentPlayer->idle();
				if (team <= 0)
				{
					currentPlayer->_isFlipped = true;
					currentPlayer->setFlipX(true);
				}
				currentPlayer->setCharNO(i + 1);
				currentPlayer->schedule(schedule_selector(CharacterBase::setRestore2), 1.0f);
				_CharacterArray->addObject(currentPlayer);
			}
			else
			{

				Hero *Com = Provider::create(player, role, group);
				Com->setDelegate(this);
				Com->setPosition(spawnPoint);
				Com->setSpawnPoint(spawnPoint);
				addChild(Com, -Com->getPositionY());
				Com->setHPbar();
				Com->setShadows();
				Com->idle();
				if (strcmp(Com->getGroup()->getCString(), Akatsuki) == 0)
				{
					Com->_isFlipped = true;
					Com->setFlipX(true);
				}

				Com->setCharNO(i + 1);
				Com->schedule(schedule_selector(CharacterBase::setRestore2), 1.0f);
				_CharacterArray->addObject(Com);
			}
			i++;
		}
	}

	_CharacterArray->retain();

	initTower();

	schedule(schedule_selector(GameLayer::updateViewPoint), 0.00f);

	//NOTE: Set all characters speed to zero. (Control movement before game real start)
	CCARRAY_FOREACH(_CharacterArray, pObject)
	{
		CharacterBase *tempHero = (CharacterBase *)pObject;
		tempHero->setWalkSpeed(0);
	}

	scheduleOnce(schedule_selector(GameLayer::onKaichang), 0.5f);
}

void GameLayer::onKaichang(float dt)
{
	getHudLayer()->onKaichang();

	if (_isHardCoreGame)
	{
		SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/battle_start1.ogg");
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/battle_start.ogg");
	}

	scheduleOnce(schedule_selector(GameLayer::onGameStart), 0.75f);
}

void GameLayer::onGameStart(float dt)
{
	_isStarted = true;

	getHudLayer()->Kaichang->removeFromParent();
	getHudLayer()->Kaichang = nullptr;
	getHudLayer()->initHeroInterface();
	schedule(schedule_selector(GameLayer::updateGameTime), 1.0f);
	schedule(schedule_selector(GameLayer::checkBackgroundMusic), 2.0f);
	schedule(schedule_selector(GameLayer::addFlog), 15.0f);
	initFlogs();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CCDirector::sharedDirector()->getOpenGLView()->setAccelerometerKeyHook((cocos2d::CCEGLView::LPFN_ACCELEROMETER_KEYHOOK)(&GameLayer::LPFN_ACCELEROMETER_KEYHOOK));
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	glfwSetKeyCallback(_window, keyEventHandle);
#endif

	CCObject *pObject;
	CCARRAY_FOREACH(_CharacterArray, pObject)
	{
		CharacterBase *tempChar = (CharacterBase *)pObject;

		//NOTE: Resume movement speed
		tempChar->setWalkSpeed(tempChar->_originSpeed);

		if (strcmp(tempChar->getRole()->getCString(), "Com") == 0)
			tempChar->doAI();
	}
}

void GameLayer::initFlogs()
{

	CCString *KonohaFlogName = CCString::create("KotetsuFlog");
	CCString *AkatsukiFlogName = CCString::create("FemalePainFlog");

	kName = "KotetsuFlog";
	aName = "FemalePainFlog";

	_KonohaFlogArray = CCArray::create();
	_AkatsukiFlogArray = CCArray::create();

	addSprites("Element/hpBar/flogBar.plist");

	int i;
	Flog *aiFlog;
	float mainPosY;
	for (i = 0; i < 6; i++)
	{
		aiFlog = Flog::create();
		aiFlog->setDelegate(this);
		aiFlog->setID(KonohaFlogName, CCString::create("Flog"), CCString::create(Konoha));
		if (i < 3)
		{
			mainPosY = (5.5 - i / 1.5) * 32;
		}
		else
		{
			mainPosY = (3.5 - i / 1.5) * 32;
		}
		aiFlog->_mainPosY = mainPosY;
		aiFlog->setPosition(ccp(13 * 32, aiFlog->_mainPosY));
		aiFlog->setHPbar();
		addChild(aiFlog, -int(aiFlog->getPositionY()));
		aiFlog->idle();
		aiFlog->doAI();
		_KonohaFlogArray->addObject(aiFlog);
	}

	_KonohaFlogArray->retain();

	for (i = 0; i < 6; i++)
	{
		aiFlog = Flog::create();
		aiFlog->setDelegate(this);
		aiFlog->setID(AkatsukiFlogName, CCString::create("Flog"), CCString::create(Akatsuki));
		if (i < 3)
		{
			mainPosY = (5.5 - i / 1.5) * 32;
		}
		else
		{
			mainPosY = (3.5 - i / 1.5) * 32;
		}
		aiFlog->_mainPosY = mainPosY;
		aiFlog->setPosition(ccp(83 * 32, aiFlog->_mainPosY));
		aiFlog->setHPbar();
		aiFlog->idle();
		aiFlog->doAI();
		_AkatsukiFlogArray->addObject(aiFlog);
		addChild(aiFlog, -aiFlog->getPositionY());
	}

	_AkatsukiFlogArray->retain();
}
void GameLayer::addFlog(float dt)
{

	CCString *KonohaFlogName = CCString::create(kName);
	CCString *AkatsukiFlogName = CCString::create(aName);

	int i;
	Flog *aiFlog;
	float mainPosY;
	for (i = 0; i < 6; i++)
	{
		aiFlog = Flog::create();
		aiFlog->setDelegate(this);
		aiFlog->setID(KonohaFlogName, CCString::create("Flog"), CCString::create(Konoha));
		if (i < 3)
		{
			mainPosY = (5.5 - i / 1.5) * 32;
		}
		else
		{
			mainPosY = (3.5 - i / 1.5) * 32;
		}
		aiFlog->_mainPosY = mainPosY;
		aiFlog->setPosition(ccp(13 * 32, aiFlog->_mainPosY));
		aiFlog->setHPbar();
		aiFlog->idle();
		aiFlog->doAI();
		_KonohaFlogArray->addObject(aiFlog);
		addChild(aiFlog, -int(aiFlog->getPositionY()));
	}

	for (i = 0; i < 6; i++)
	{
		aiFlog = Flog::create();
		aiFlog->setDelegate(this);
		aiFlog->setID(AkatsukiFlogName, CCString::create("Flog"), CCString::create(Akatsuki));
		if (i < 3)
		{
			mainPosY = (5.5 - i / 1.5) * 32;
		}
		else
		{
			mainPosY = (3.5 - i / 1.5) * 32;
		}
		aiFlog->_mainPosY = mainPosY;
		aiFlog->setPosition(ccp(83 * 32, aiFlog->_mainPosY));
		aiFlog->setHPbar();
		aiFlog->idle();
		aiFlog->doAI();
		_AkatsukiFlogArray->addObject(aiFlog);
		addChild(aiFlog, -aiFlog->getPositionY());
	}
}

void GameLayer::initTower()
{

	if (randomMap == 0)
	{
		addSprites("Element/Tower/Tower.plist");
	}
	else if (randomMap == 1)
	{
		addSprites("Element/Tower/Tower2.plist");
	}
	else if (randomMap == 2)
	{
		addSprites("Element/Tower/Tower3.plist");
	}
	else if (randomMap == 3)
	{
		addSprites("Element/Tower/Tower4.plist");
	}
	else if (randomMap == 4)
	{
		addSprites("Element/Tower/Tower5.plist");
	}

	CCTMXObjectGroup *metaGroup = currentMap->objectGroupNamed("meta");
	CCArray *metaArray = metaGroup->getObjects();

	_TowerArray = CCArray::create();

	CCObject *pObject;
	int i = 0;

	CCARRAY_FOREACH(metaArray, pObject)
	{

		CCDictionary *dict = (CCDictionary *)pObject;

		int metaX = ((CCString *)dict->objectForKey("x"))->intValue();
		int metaY = ((CCString *)dict->objectForKey("y"))->intValue();

		int metaWidth = ((CCString *)dict->objectForKey("width"))->intValue();
		int metaHeight = ((CCString *)dict->objectForKey("height"))->intValue();

		CCString *name = (CCString *)dict->objectForKey("name");

		Tower *tower = Tower::create();
		tower->setDelegate(this);
		char towerName[7] = "abcdef";
		strncpy(towerName, name->getCString(), 6);
		if (strcmp(towerName, Konoha) == 0)
		{
			tower->setID(name, CCString::create("Tower"), CCString::create(Konoha));
		}
		else
		{
			tower->setID(name, CCString::create("Tower"), CCString::create(Akatsuki));
			tower->setFlipX(true);
			tower->_isFlipped = true;
		}
		float posX = metaX + metaWidth / 2;
		float posY = metaY + metaHeight / 2;
		tower->setPosition(ccp(posX, posY));
		tower->setSpawnPoint(ccp(posX, posY));
		tower->setCharNO(i + 1);

		if (i == 1 || i == 4)
		{
			//4v4
			if (Cheats > 10)
			{
				tower->setMaxHP(CCString::createWithFormat("%d", 80000));
			}
			else
			{
				tower->setMaxHP(CCString::createWithFormat("%d", 50000));
			}
			tower->setHP(CCString::create(tower->getMaxHP()->getCString()));
		}
		tower->setHPbar();
		tower->_hpBar->setVisible(false);
		tower->idle();
		addChild(tower, -tower->getPositionY());

		_TowerArray->addObject(tower);
		_TowerArray->retain();

		i++;
	}
}

void GameLayer::initEffects()
{

	addSprites("Effects/SkillEffect.plist");
	skillEffectBatch = CCSpriteBatchNode::create("Effects/SkillEffect.png");
	addChild(skillEffectBatch, currentSkillTag);

	addSprites("Effects/DamageEffect.plist");
	damageEffectBatch = CCSpriteBatchNode::create("Effects/DamageEffect.png");
	addChild(damageEffectBatch, currentDamageTag);

	addSprites("Effects/Shadows.plist");
	shadowBatch = CCSpriteBatchNode::create("Effects/Shadows.png");
	addChild(shadowBatch, currentShadowTag);
}

void GameLayer::updateGameTime(float dt)
{
	_second += 1;
	if (_second == 60)
	{
		_minute += 1;
		_second = 0;
	}
	CCString *tempTime = CCString::createWithFormat("%02d:%02d", _minute, _second);
	_hudLayer->gameClock->setString(tempTime->getCString());

	int newValue = to_int(getTotalTM()->getCString()) + 1;
	setTotalTM(CCString::createWithFormat("%d", newValue));
}

void GameLayer::updateViewPoint(float dt)
{

	if (!currentPlayer)
		return;
	CCPoint playerPoint;
	if (ougisChar)
	{
		playerPoint = ougisChar->getPosition();
	}
	else if (controlChar)
	{
		playerPoint = controlChar->getPosition();
	}
	else
	{
		playerPoint = currentPlayer->getPosition();
	}

	int x = MAX(playerPoint.x, winSize.width / 2);
	int y = MAX(playerPoint.y, winSize.width / 2);
	x = MIN(x, (currentMap->getMapSize().width * currentMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (currentMap->getMapSize().height * currentMap->getTileSize().height) - winSize.height / 2);

	CCPoint actualPoint = ccp(x, y);
	CCPoint centerPoint = ccp(winSize.width / 2, y);
	CCPoint viewPoint = ccpSub(centerPoint, actualPoint);

	setPosition(viewPoint);
	// CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
}

void GameLayer::setTowerState(unsigned int charNO)
{
	_hudLayer->setTowerState(charNO);
};

void GameLayer::setHPLose(float percent)
{
	_hudLayer->setHPLose(percent);
}
void GameLayer::setCKRLose(bool isCRK2)
{
	_hudLayer->setCKRLose(isCRK2);
}

void GameLayer::setReport(const char *name1, const char *name2, CCString *killNum)
{
	_hudLayer->setReport(name1, name2, killNum);
}

void GameLayer::resetStatusBar()
{
	_hudLayer->status_hpbar->setRotation(0);
}

void GameLayer::setCoin(const char *value)
{
	_hudLayer->setCoin(value);
}
void GameLayer::removeOugisMark(int type)
{
	if (type == 1)
	{
		if (_hudLayer->skill4Button)
		{
			if (_hudLayer->skill4Button->lockLabel1)
			{
				_hudLayer->skill4Button->lockLabel1->removeFromParent();
				_hudLayer->skill4Button->lockLabel1 = nullptr;
			}
		}
	}
	else
	{

		if (_hudLayer->skill5Button)
		{
			if (_hudLayer->skill5Button->lockLabel1)
			{
				_hudLayer->skill5Button->lockLabel1->removeFromParent();
				_hudLayer->skill5Button->lockLabel1 = nullptr;
			}
		}
	}
}

void GameLayer::checkTower()
{

	int konohaTowerCount = 0;
	int akatsukiTowerCount = 0;
	CCObject *pObject;

	CCARRAY_FOREACH(_TowerArray, pObject)
	{
		Tower *tower = (Tower *)pObject;
		if (strcmp(tower->getGroup()->getCString(), Konoha) == 0)
		{
			konohaTowerCount++;
		}
		else
		{
			akatsukiTowerCount++;
		}
	}

	if (konohaTowerCount == 2)
	{
		aName = "PainFlog";
		kEXPBound = 50;
	}
	else if (konohaTowerCount == 1)
	{
		aName = "ObitoFlog";
		kEXPBound = 100;
	}

	if (akatsukiTowerCount == 2)
	{
		kName = "IzumoFlog";
		aEXPBound = 50;
	}
	else if (akatsukiTowerCount == 1)
	{
		kName = "KakashiFlog";
		aEXPBound = 100;
	}

	if (postTime >= 9)
	{
		currentPlayer = nullptr;
	}

	CCARRAY_FOREACH(_CharacterArray, pObject)
	{
		CharacterBase *tmpHero = (CharacterBase *)pObject;

		if (strcmp(tmpHero->getRole()->getCString(), "Com") != 0)
		{
			continue;
		}

		if (strcmp(tmpHero->getGroup()->getCString(), Konoha) == 0)
		{
			tmpHero->battleCondiction = konohaTowerCount - akatsukiTowerCount;
			if (konohaTowerCount == 1)
			{
				tmpHero->isBaseDanger = true;
			}
		}
		else
		{

			tmpHero->battleCondiction = akatsukiTowerCount - konohaTowerCount;
			if (_isHardCoreGame)
			{
				if (akatsukiTowerCount == 1)
				{
					tmpHero->isBaseDanger = true;
				}
			}
		}
	}

	if (konohaTowerCount == 0 || akatsukiTowerCount == 0)
	{
		if (team > 0)
		{
			if (konohaTowerCount == 0)
			{
				onGameOver(false);
			}
			else
			{
				onGameOver(true);
			}
		}
		else
		{
			if (akatsukiTowerCount == 0)
			{
				onGameOver(false);
			}
			else
			{
				onGameOver(true);
			}
		}
	}
}

void GameLayer::clearDoubleClick()
{
	if (_hudLayer->skill1Button->getDoubleSkill() &&
		_hudLayer->skill1Button->_clickNum >= 1)
	{
		_hudLayer->skill1Button->setFreezeAction(nullptr);
		_hudLayer->skill1Button->beganAnimation();
	}
}

void GameLayer::JoyStickRelease()
{

	if (currentPlayer->getActionState() == State::WALK)
	{
		currentPlayer->idle();
	}
}

void GameLayer::JoyStickUpdate(CCPoint direction)
{
	if (!ougisChar)
	{
		//CCLOG("x:%f,y:%f",direction.x,direction.y);
		currentPlayer->walk(direction);
	}
}

void GameLayer::attackButtonClick(abType type)
{
	if (type == NAttack)
	{
		_isAttackButtonRelease = false;
	}

	if (type == Item1)
	{
		currentPlayer->setItem(type);
	}
	else
	{
		currentPlayer->attack(type);
	}
}
void GameLayer::gearButtonClick(gearType type)
{

	currentPlayer->useGear(type);
}

void GameLayer::attackButtonRelease()
{
	_isAttackButtonRelease = true;
}

void GameLayer::onPause()
{
	if (_isPause)
		return;

	_isPause = true;
	CCRenderTexture *snapshoot = CCRenderTexture::create(winSize.width, winSize.height);
	CCScene *f = CCDirector::sharedDirector()->getRunningScene();
	CCObject *pObject = f->getChildren()->objectAtIndex(0);
	BGLayer *bg = (BGLayer *)pObject;
	snapshoot->begin();
	bg->visit();

	visit();
	snapshoot->end();

	CCScene *pscene = CCScene::create();
	PauseLayer *layer = PauseLayer::create(snapshoot);

	layer->setDelegate(this);
	pscene->addChild(layer);
	CCDirector::sharedDirector()->pushScene(pscene);
}

void GameLayer::onGear()
{
	if (_isGear)
		return;

	_isGear = true;

	if (_isHardCoreGame)
	{

		CCRenderTexture *snapshoot = CCRenderTexture::create(winSize.width, winSize.height);
		CCScene *f = CCDirector::sharedDirector()->getRunningScene();
		CCObject *pObject = f->getChildren()->objectAtIndex(0);
		BGLayer *bg = (BGLayer *)pObject;
		snapshoot->begin();
		bg->visit();

		visit();
		snapshoot->end();

		CCScene *pscene = CCScene::create();
		GearLayer *layer = GearLayer::create(snapshoot);
		_gLayer->_gearLayer = layer;

		layer->setDelegate(this);
		layer->updatePlayerGear();
		pscene->addChild(layer);
		CCDirector::sharedDirector()->pushScene(pscene);
	}
}

void GameLayer::onGameOver(bool isWin)
{
	// Stop all keyboard events
	_gLayer = nullptr;

	if (_isPause)
	{
		_isPause = false;
		CCDirector::sharedDirector()->popScene();
	}
	if (_isGear)
	{
		_isGear = false;
		CCDirector::sharedDirector()->popScene();
	}

	CCRenderTexture *snapshoot = CCRenderTexture::create(winSize.width, winSize.height);
	CCScene *f = CCDirector::sharedDirector()->getRunningScene();
	CCObject *pObject = f->getChildren()->objectAtIndex(0);
	BGLayer *bg = (BGLayer *)pObject;
	snapshoot->begin();
	bg->visit();
	visit();
	snapshoot->end();

	CCScene *pscene = CCScene::create();
	GameOver *layer = GameOver::create(snapshoot);
	layer->setWin(isWin);
	layer->setDelegate(this);
	pscene->addChild(layer);
	CCDirector::sharedDirector()->pushScene(pscene);
}

void GameLayer::onLeft()
{

	CCArray *childArray = getChildren();
	CCObject *pObject;

	CCARRAY_FOREACH(childArray, pObject)
	{
		CharacterBase *ac = (CharacterBase *)pObject;
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(ac, "acceptAttack");
	}

	if (randomMap == 0)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Tower/Tower.plist");
	}
	else if (randomMap == 1)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Tower/Tower2.plist");
	}
	else if (randomMap == 2)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Tower/Tower3.plist");
	}
	else if (randomMap == 3)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Tower/Tower4.plist");
	}
	else if (randomMap == 4)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Tower/Tower5.plist");
	}

	const char *path;
	CCARRAY_FOREACH(_CharacterArray, pObject)
	{
		CharacterBase *player = (CharacterBase *)pObject;

		if (strcmp(player->getRole()->getCString(), K_TAG_CLONE) == 0 ||
			strcmp(player->getRole()->getCString(), "Summon") == 0)
		{
			continue;
		}

		path = CCString::createWithFormat("Element/%s/%s.plist", player->getCharacter()->getCString(), player->getCharacter()->getCString())->getCString();
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(path);

		if (strcmp(player->getRole()->getCString(), "Com") == 0 ||
			strcmp(player->getRole()->getCString(), "Player") == 0)
		{
			KTools::prepareFileOGG(player->getCharacter()->getCString(), true);
		}

		if (strcmp(player->getCharacter()->getCString(), "Jiraiya") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/SageJiraiya/SageJiraiya.plist");
			KTools::prepareFileOGG("SageJiraiya", true);
		}
		else if (strcmp(player->getCharacter()->getCString(), "Kankuro") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Karasu/Karasu.plist");
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Sanshouuo/Sanshouuo.plist");
		}
		else if (strcmp(player->getCharacter()->getCString(), "Kakuzu") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/MaskFudon/MaskFudon.plist");
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/MaskRaidon/MaskRaidon.plist");
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/MaskKadon/MaskKadon.plist");
		}
		else if (strcmp(player->getCharacter()->getCString(), "Naruto") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/SageNaruto/SageNaruto.plist");
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/RikudoNaruto/RikudoNaruto.plist");
			KTools::prepareFileOGG("SageNaruto", true);
			KTools::prepareFileOGG("RikudoNaruto", true);
		}
		else if (strcmp(player->getCharacter()->getCString(), "RockLee") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Lee/Lee.plist");
		}
		else if (strcmp(player->getCharacter()->getCString(), "Lee") == 0)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/RockLee/RockLee.plist");
		}
		else if (strcmp(player->getCharacter()->getCString(), "Sasuke") == 0)
		{
			KTools::prepareFileOGG("ImmortalSasuke", true);
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/ImmortalSasuke/ImmortalSasuke.plist");
		}
		player->removeFromParentAndCleanup(true);
	}

	if (_isHardCoreGame)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Roshi/Roshi.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Element/Han/Han.plist");
		KTools::prepareFileOGG(Guardian_Roshi, true);
		KTools::prepareFileOGG(Guardian_Han, true);
	}

	KTools::prepareFileOGG("Effect", true);
	KTools::prepareFileOGG("Ougis", true);

	_CharacterArray->removeAllObjects();
	_CharacterArray = nullptr;

	_TowerArray->removeAllObjects();
	_TowerArray = nullptr;
	_KonohaFlogArray->removeAllObjects();
	_KonohaFlogArray = nullptr;
	_AkatsukiFlogArray->removeAllObjects();
	_AkatsukiFlogArray = nullptr;

	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("UI.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("Map.plist");

	SimpleAudioEngine::sharedEngine()->end();

	lua_call_func("onGameOver");
}

void GameLayer::checkBackgroundMusic(float dt)
{
	if (CCUserDefault::sharedUserDefault()->getBoolForKey("isBGM") != false)
	{

		if (!SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
		{
			if (!_isHardCoreGame)
			{
				SimpleAudioEngine::sharedEngine()->playBackgroundMusic(BATTLE_MUSIC);
			}
			else
			{
				if (_playNum == 0)
				{
					SimpleAudioEngine::sharedEngine()->playBackgroundMusic(CCString::createWithFormat("Audio/Music/Battle%d.ogg", int(2 + randomMap * 3))->getCString(), false);
					_playNum++;
				}
				else if (_playNum == 1)
				{
					SimpleAudioEngine::sharedEngine()->playBackgroundMusic(CCString::createWithFormat("Audio/Music/Battle%d.ogg", int(3 + randomMap * 3))->getCString(), false);
					_playNum++;
				}
				else if (_playNum == 2)
				{
					SimpleAudioEngine::sharedEngine()->playBackgroundMusic(CCString::createWithFormat("Audio/Music/Battle%d.ogg", int(1 + randomMap * 3))->getCString(), false);
					_playNum = 0;
				}
			}
		}
	}
}

void GameLayer::setOugis(CCNode *sender)
{

	if (!_hudLayer->ougisLayer)
	{
		CCArray *childArray = getChildren();
		ougisChar = sender;
		CharacterBase *Sender = (CharacterBase *)sender;
		CCObject *pObject;
		CCARRAY_FOREACH(childArray, pObject)
		{

			CCNode *object = (CCNode *)pObject;

			object->pauseSchedulerAndActions();
		}
		pauseSchedulerAndActions();

		updateViewPoint(0.01);

		blend = CCLayerColor::create(ccc4(0, 0, 0, 200), winSize.width, winSize.height);
		blend->setPosition(ccp(-getPositionX(), 0));
		addChild(blend, 1000);
		sender->setZOrder(2000);

		if (CCUserDefault::sharedUserDefault()->getBoolForKey("isVoice") != false)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(CCString::createWithFormat("Audio/Ougis/%s_ougis.ogg", Sender->getCharacter()->getCString())->getCString());
		}

		_hudLayer->setOugis(Sender->getCharacter(), Sender->getGroup());
	}
}

void GameLayer::removeOugis()
{
	ougisChar->setZOrder(-ougisChar->getPositionY());
	CCArray *childArray = getChildren();
	CCObject *pObject;
	CCARRAY_FOREACH(childArray, pObject)
	{
		CCNode *object = (CCNode *)pObject;
		object->resumeSchedulerAndActions();
	}
	resumeSchedulerAndActions();

	blend->removeFromParent();
	ougisChar = nullptr;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define isPressed(__KEY__) _isPressed(__KEY__)
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#include "glfw3.h"
#define isPressed(__KEY__) glfwGetKey(_window, __KEY__)
#endif

/**
 * Use __W __S __D __A control when to force move
 */
#define MOVE(__W, __S, __D, __A, name, keyState)                                                    \
	if (keyState)                                                                                   \
		_gLayer->_lastPressedMovementKey = name;                                                    \
	else if (_gLayer->_lastPressedMovementKey == name)                                              \
		_gLayer->_lastPressedMovementKey = -100;                                                    \
	int horizontal_##name;                                                                          \
	int vertical_##name;                                                                            \
	if (__W)                                                                                        \
	{                                                                                               \
		vertical_##name = (isPressed(KEY_W) ? 1 : -1);                                              \
	}                                                                                               \
	else if (__S)                                                                                   \
	{                                                                                               \
		vertical_##name = (isPressed(KEY_S) ? -1 : 1);                                              \
	}                                                                                               \
	else                                                                                            \
	{                                                                                               \
		vertical_##name = (isPressed(KEY_W) ? 1 : -1) + (isPressed(KEY_S) ? -1 : 1);                \
		vertical_##name = abs(vertical_##name) > 1 ? vertical_##name / 2 : vertical_##name;         \
	}                                                                                               \
	if (__D)                                                                                        \
	{                                                                                               \
		horizontal_##name = (isPressed(KEY_D) ? 1 : -1);                                            \
	}                                                                                               \
	else if (__A)                                                                                   \
	{                                                                                               \
		horizontal_##name = (isPressed(KEY_A) ? -1 : 1);                                            \
	}                                                                                               \
	else                                                                                            \
	{                                                                                               \
		horizontal_##name = (isPressed(KEY_D) ? 1 : -1) + (isPressed(KEY_A) ? -1 : 1);              \
		horizontal_##name = abs(horizontal_##name) > 1 ? horizontal_##name / 2 : horizontal_##name; \
	}                                                                                               \
	if (horizontal_##name != 0 || vertical_##name != 0)                                             \
	{                                                                                               \
		if (!_gLayer->ougisChar)                                                                    \
			_gLayer->currentPlayer->walk(ccp(horizontal_##name, vertical_##name));                  \
	}                                                                                               \
	else if (_gLayer->currentPlayer->getActionState() == State::WALK)                               \
	{                                                                                               \
		_gLayer->_lastPressedMovementKey = -100;                                                    \
		_gLayer->currentPlayer->idle();                                                             \
	}                                                                                               \
	break;

#define ON_GEAR_BY(__ID__, __KEY_STATE__)                                                                                          \
	if (_gLayer->_isGear && __KEY_STATE__)                                                                                         \
	{                                                                                                                              \
		GearButton *gear_btn_##__ID__ = (GearButton *)_gLayer->_gearLayer->_screwLayer->getGearArray()->objectAtIndex(__ID__ - 1); \
		gear_btn_##__ID__->click();                                                                                                \
	}                                                                                                                              \
	break;

bool GameLayer::checkHasAnyMovement()
{
	if (_gLayer)
	{
		if (_gLayer->_lastPressedMovementKey != -100)
		{
			keyEventHandle(_window, _gLayer->_lastPressedMovementKey, 0, 1, 0);
			return true;
		}
	}
	return false;
}

/** NOTE: Impl key listener */
void GameLayer::keyEventHandle(GLFWwindow *window, int key, int scancode, int keyState, int mods)
{
	if (_gLayer == nullptr)
		return;

	switch (key)
	{
	case KEY_W:
	case KEY_UP:
		MOVE(keyState, 0, 0, 0, KEY_W, keyState);
	case KEY_S:
	case KEY_DOWN:
		MOVE(0, keyState, 0, 0, KEY_S, keyState);
	case KEY_A:
	case KEY_LEFT:
		MOVE(0, 0, keyState, 0, KEY_A, keyState);
	case KEY_D:
	case KEY_RIGHT:
		MOVE(0, 0, 0, keyState, KEY_D, keyState);
	case KEY_J:
		if (keyState)
			_gLayer->_hudLayer->nAttackButton->click();
		else
			_gLayer->_isAttackButtonRelease = true;
		break;
	case KEY_L: // Ramen button
		if (keyState)
			_gLayer->_hudLayer->item1Button->click();
		break;
	case KEY_H: // Ougis 2 buttons
		if (keyState)
			_gLayer->_hudLayer->skill5Button->click();
		break;
	case KEY_K:
		if (keyState)
			_gLayer->_hudLayer->skill4Button->click();
		break;
	case KEY_U: // skill 1
		if (keyState)
			_gLayer->_hudLayer->skill1Button->click();
		break;
	case KEY_I: // skill 2
		if (keyState)
			_gLayer->_hudLayer->skill2Button->click();
		break;
	case KEY_O: // skill 3
		if (keyState)
			_gLayer->_hudLayer->skill3Button->click();
		break;
		// Gear buttons
	case KEY_1:
	case KEY_KP_1:
		ON_GEAR_BY(1, keyState);
	case KEY_2:
	case KEY_KP_2:
		ON_GEAR_BY(2, keyState);
	case KEY_3:
	case KEY_KP_3:
		ON_GEAR_BY(3, keyState);
	case KEY_4:
	case KEY_KP_4:
		ON_GEAR_BY(4, keyState);
	case KEY_5:
	case KEY_KP_5:
		ON_GEAR_BY(5, keyState);
	case KEY_6:
	case KEY_KP_6:
		ON_GEAR_BY(6, keyState);
	case KEY_7:
	case KEY_KP_7:
		ON_GEAR_BY(7, keyState);
	case KEY_8:
	case KEY_KP_8:
		ON_GEAR_BY(8, keyState);
	case KEY_9:
	case KEY_KP_9:
		ON_GEAR_BY(9, keyState);
	case KEY_0:
	case KEY_KP_0:
		break;
	/* Item buttons */
	// Item 1 & Purchase
	case KEY_B:
		if (keyState)
		{
			if (_gLayer->_isGear)
				_gLayer->_gearLayer->confirmPurchase();
			else
				_gLayer->_hudLayer->getItem3Button()->click();
		}
		break;
	// Item 2
	case KEY_N:
		if (keyState)
			_gLayer->_hudLayer->getItem4Button()->click();
		break;
	// Item 3
	case KEY_M:
		if (keyState)
			_gLayer->_hudLayer->getItem2Button()->click();
		break;
	case KEY_ESCAPE:
	case KEY_ENTER:
		if (keyState && _gLayer->_isStarted == true)
		{
			if (_gLayer->_isPause)
			{
				CCDirector::sharedDirector()->popScene();
				_gLayer->_isPause = false;
			}
			else if (_gLayer->_isGear)
			{
				CCDirector::sharedDirector()->popScene();
				_gLayer->_isGear = false;
			}
			else
			{
				_gLayer->onPause(); // enter pause menu
			}
		}
		break;
	case KEY_SPACE:
		if (_gLayer->_isStarted && keyState && !_gLayer->_isPause && !_gLayer->_isGear)
			_gLayer->onGear(); // enter gear shop
		break;
	case KEY_F11:
		// if (keyState)
		// {
		// 	// SET_FULL_SCREEN_MODE(_window, _isFullScreen);
		// 	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		// 	if (nullptr == monitor)
		// 	{
		// 		return;
		// 	}
		// 	const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);
		// 	int width, height;
		// 	glfwGetWindowSize(_window, &width, &height);
		// 	if (_isFullScreen)
		// 	{
		// 		glfwSetWindowMonitor(_window, nullptr, videoMode->width / 2, videoMode->height / 2, WIDTH, HEIGHT, videoMode->refreshRate);
		// 		glfwSetWindowPos(_window,
		// 						 (videoMode->width - WIDTH) / 2,
		// 						 (videoMode->height - HEIGHT) * 0.35f);
		// 	}
		// 	else
		// 	{
		// 		glfwSetWindowMonitor(_window, nullptr, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
		// 		CCDirector::sharedDirector()->getOpenGLView()->updateFrameSize(videoMode->width,videoMode->height);
		// 	}
		// 	_isFullScreen = !_isFullScreen;
		// }
		break;
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

void GameLayer::LPFN_ACCELEROMETER_KEYHOOK(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		keyEventHandle(nullptr, wParam, 0, 1, 0);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		keyEventHandle(nullptr, wParam, 0, 0, 0);
		break;
	}
}

#endif

#endif
