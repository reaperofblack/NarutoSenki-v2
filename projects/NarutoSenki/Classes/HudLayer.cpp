#include "HudLayer.h"
#include "MyUtils/CCShake.h"

MiniIcon::MiniIcon()
{
}

MiniIcon::~MiniIcon()
{

	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateMap");
}

bool MiniIcon::init(const char *szImage, bool isNotification)
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!CCSprite::init());
		initWithSpriteFrameName(szImage);

		if (isNotification)
		{
			setAnchorPoint(ccp(0.5f, 0));
			CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(MiniIcon::updateMap), "updateMap", nullptr);
		}
		else
		{
			setAnchorPoint(ccp(0.5f, 0.5f));
		}

		bRet = true;
	} while (0);

	return bRet;
}

void MiniIcon::updateMap(CCObject *sender)
{
	CharacterBase *poster = (CharacterBase *)sender;

	if (getCharNO() == poster->getCharNO())
	{
		if (poster->getActionState() == State::DEAD || poster->_isVisable == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
			updatePosition(poster->getPosition());
		}
	}
}

void MiniIcon::updateState()
{
	CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("tower_icon3.png");
	setDisplayFrame(frame);
}

void MiniIcon::updatePosition(CCPoint location)
{
	int x = floor(location.x / 32);
	int y = floor(location.y / 32) * 5;

	setPosition(ccp(x, y));
	_delegate->reorderChild(this, 500);
}

MiniIcon *MiniIcon::create(const char *szImage, bool isNotification)
{
	MiniIcon *mi = new MiniIcon();
	if (mi && mi->init(szImage, isNotification))
	{
		mi->autorelease();
		return mi;
	}
	else
	{
		delete mi;
		return nullptr;
	}
}

HudLayer::HudLayer()
{
	_joyStick = nullptr;

	_reportListArray = nullptr;
	_buffCount = 0;
	_towerIconArray = nullptr;
	reportSPCSprite = nullptr;

	_isAllButtonLocked = false;

	isOpenSQL = false;
	skill4Button = nullptr;
	skill5Button = nullptr;
	ougisLayer = nullptr;

	gear1Button = nullptr;
	gear2Button = nullptr;
	gear3Button = nullptr;
	gearMenu = nullptr;

	item1Button = nullptr;
	item2Button = nullptr;
	item3Button = nullptr;
	item4Button = nullptr;
	Kaichang = nullptr;

	bcdLabel1 = nullptr;
	bcdLabel2 = nullptr;
}

HudLayer::~HudLayer()
{
}

void HudLayer::onEnter()
{
	if (_delegate->_isExiting)
	{
		return;
	}
	CCLayer::onEnter();
}

void HudLayer::onExit()
{
	CCLayer::onExit();
	if (_delegate->_isExiting)
	{

		CC_SAFE_RELEASE(_reportListArray);
		CC_SAFE_RELEASE(_towerIconArray);
	}
}

bool HudLayer::init()
{
	bool bRet = false;
	do
	{

		CC_BREAK_IF(!CCLayer::init());

		texUI = CCTextureCache::sharedTextureCache()->addImage("UI.png");
		uiBatch = CCSpriteBatchNode::createWithTexture(texUI);

		addChild(uiBatch);

		bRet = true;

	} while (0);

	return bRet;
}

void HudLayer::setDelegate(GameLayer *layer)
{
	_delegate = layer;
}

void HudLayer::JoyStickRelease()
{
	_delegate->JoyStickRelease();
}

void HudLayer::JoyStickUpdate(CCPoint direction)
{
	_delegate->JoyStickUpdate(direction);
}

void HudLayer::initGearButton(const char *charName)
{
	if (gearMenu)
	{
		gearMenu->removeFromParentAndCleanup(true);
	}

	CCMenuItem *gear_button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_avator.png", charName)->getCString()), nullptr, nullptr, this, menu_selector(HudLayer::gearButtonClick));
	gear_button->setAnchorPoint(ccp(0, 0));
	gearMenu = CCMenu::create(gear_button, nullptr);
	gearMenu->setPosition(ccp(0, winSize.height - gear_button->getContentSize().height));
	gearMenu->setTouchPriority(10);
	addChild(gearMenu, 100);
}

void HudLayer::onKaichang()
{

	CCArray *tempArray = CCArray::create();

	int i = 1;

	while (i < ComCount)
	{
		CCString *path = CCString::createWithFormat("gameStart_00%d.png", i);
		CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(path->getCString());
		tempArray->addObject(frame);
		i += 1;
	}

	CCAnimation *tempAnimation = CCAnimation::createWithSpriteFrames(tempArray, 0.1f);
	CCAction *tempAction = CCAnimate::create(tempAnimation);
	CCArray *list = CCArray::create();
	list->addObject(tempAction);
	CCAction *seq = CCSequence::create(list);

	Kaichang = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("gameStart_001.png")->getCString());

	Kaichang->setAnchorPoint(ccp(0.5f, 0.5f));
	Kaichang->setPosition(ccp(winSize.width / 2 + 32, winSize.height / 2));
	addChild(Kaichang, 5000);
	Kaichang->runAction(seq);
}

void HudLayer::initHeroInterface()
{

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	//init the joyStick
	_joyStick = JoyStick::create();
	_joyStick->setPosition(ccp(28, 16));
	_joyStick->setDelegate(this);
	uiBatch->addChild(_joyStick);
#endif

	//produce status bar
	status_bar = CCSprite::createWithSpriteFrameName("status_bar_bg.png");
	status_bar->setAnchorPoint(ccp(0, 0));
	status_bar->setPosition(ccp(0, winSize.height - status_bar->getContentSize().height));
	uiBatch->addChild(status_bar);

	status_hpbar = CCSprite::createWithSpriteFrameName("status_hpbar.png");
	status_hpbar->setPosition(ccp(53, winSize.height - 54));
	addChild(status_hpbar, 40);

	status_hpMark = CCSprite::createWithSpriteFrameName("status_hpMark.png");
	status_hpMark->setAnchorPoint(ccp(0, 0));
	status_hpMark->setPosition(ccp(54, winSize.height - 105));
	addChild(status_hpMark, 45);

	CCSprite *tmpSprite = CCSprite::createWithSpriteFrameName("status_ckrbar.png");
	status_expbar = CCProgressTimer::create(tmpSprite);
	status_expbar->setType(kCCProgressTimerTypeRadial);
	status_expbar->setPercentage(0);
	status_expbar->setReverseDirection(true);
	status_expbar->setPosition(ccp(54, winSize.height - 54));
	addChild(status_expbar, 50);
	CCMenuItem *menu_button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("minimap_bg.png"), nullptr, nullptr, this, menu_selector(HudLayer::pauseButtonClick));
	menu_button->setAnchorPoint(ccp(1, 1));
	pauseNenu = CCMenu::create(menu_button, nullptr);
	pauseNenu->setPosition(ccp(winSize.width, winSize.height));
	addChild(pauseNenu);

	CCSprite *killIcon = CCSprite::createWithSpriteFrameName("kill_icon.png");
	killIcon->setAnchorPoint(ccp(0, 1));
	killIcon->setPosition(ccp(winSize.width - 114, winSize.height - 46));
	addChild(killIcon, 5000);

	killLabel = CCLabelBMFont::create("0", "Fonts/1.fnt");
	killLabel->setScale(0.26f);
	killLabel->setAnchorPoint(ccp(0, 1));
	killLabel->setPosition(ccp(killIcon->getPositionX() + killIcon->getContentSize().width / 2 + 5, winSize.height - 56));
	addChild(killLabel, 5001);

	CCSprite *deadIcon = CCSprite::createWithSpriteFrameName("dead_icon.png");
	deadIcon->setAnchorPoint(ccp(0, 1));
	deadIcon->setPosition(ccp(winSize.width - 114 + 26, winSize.height - 47));
	addChild(deadIcon, 5000);

	deadLabel = CCLabelBMFont::create("0", "Fonts/1.fnt");
	deadLabel->setScale(0.26f);
	deadLabel->setAnchorPoint(ccp(0, 1));
	deadLabel->setPosition(ccp(deadIcon->getPositionX() + deadIcon->getContentSize().width / 2 + 5, winSize.height - 56));
	addChild(deadLabel, 5001);

	KonoLabel = CCLabelBMFont::create("0", "Fonts/blue.fnt");
	KonoLabel->setScale(0.35f);

	AkaLabel = CCLabelBMFont::create("0", "Fonts/red.fnt");
	AkaLabel->setScale(0.35f);

	if (_delegate->team > 0)
	{
		KonoLabel->setAnchorPoint(ccp(1, 1));
		AkaLabel->setAnchorPoint(ccp(0, 1));
		KonoLabel->setPosition(ccp(winSize.width - 42, winSize.height - 46));
		AkaLabel->setPosition(ccp(winSize.width - 32, winSize.height - 46));
	}
	else
	{
		AkaLabel->setAnchorPoint(ccp(1, 1));
		KonoLabel->setAnchorPoint(ccp(0, 1));
		AkaLabel->setPosition(ccp(winSize.width - 42, winSize.height - 46));
		KonoLabel->setPosition(ccp(winSize.width - 32, winSize.height - 46));
	}

	addChild(KonoLabel, 5000);
	addChild(AkaLabel, 5000);
	CCLabelBMFont *hengLabel = CCLabelBMFont::create(":", "Fonts/1.fnt");
	hengLabel->setScale(0.5f);
	hengLabel->setAnchorPoint(ccp(0.5f, 1));
	hengLabel->setPosition(ccp(winSize.width - 37, KonoLabel->getPositionY() + 2));
	addChild(hengLabel, 5000);

	initGearButton(_delegate->currentPlayer->getCharacter()->getCString());

	hpLabel = CCLabelBMFont::create(_delegate->currentPlayer->getHP()->getCString(), "Fonts/1.fnt");
	hpLabel->setScale(0.35f);
	hpLabel->setPosition(ccp(0, winSize.height - 54));
	hpLabel->setAnchorPoint(ccp(0, 0));

	addChild(hpLabel, 5000);

	int exp = _delegate->currentPlayer->getEXP() - ((_delegate->currentPlayer->getLV() - 1) * 500) / 500;
	expLabel = CCLabelBMFont::create(CCString::createWithFormat("%i%%", exp)->getCString(), "Fonts/1.fnt");
	expLabel->setScale(0.35f);
	expLabel->setPosition(ccp(94, winSize.height - 54));
	expLabel->setAnchorPoint(ccp(0.5f, 0));

	addChild(expLabel, 5000);

	gameClock = CCLabelBMFont::create("00:00", "Fonts/1.fnt");
	gameClock->setAnchorPoint(ccp(0.5f, 0));
	gameClock->setScale(0.4f);
	gameClock->setPosition(ccp(25, 0));
	addChild(gameClock, 5000);

	coinLabel = CCLabelBMFont::create("50", "Fonts/arial.fnt");
	coinLabel->setAnchorPoint(ccp(0, 0));
	coinLabel->setPosition(ccp(121, winSize.height - 61));

	addChild(coinLabel, 5000);

	//init NATTACK Button
	nAttackButton = ActionButton::create("n_attack.png");
	nAttackButton->setDelegate(this);
	nAttackButton->setABType(NAttack);
	uiBatch->addChild(nAttackButton);

	//init SKIll Button

	skill1Button = ActionButton::create(CCString::createWithFormat("%s_skill1.png", _delegate->currentPlayer->getCharacter()->getCString())->getCString());
	skill1Button->setCD(CCString::createWithFormat("%d", _delegate->currentPlayer->_sattackcoldDown1 * 1000));
	skill1Button->setDelegate(this);
	skill1Button->setABType(SKILL1);
	if (_delegate->currentPlayer->_sattack1isDouble)
	{
		skill1Button->setDoubleSkill(true);
	}
	uiBatch->addChild(skill1Button);

	skill2Button = ActionButton::create(CCString::createWithFormat("%s_skill2.png", _delegate->currentPlayer->getCharacter()->getCString())->getCString());
	skill2Button->setCD(CCString::createWithFormat("%d", _delegate->currentPlayer->_sattackcoldDown2 * 1000));
	skill2Button->setDelegate(this);
	skill2Button->setABType(SKILL2);
	if (_delegate->currentPlayer->_sattack2isDouble)
	{
		skill2Button->setDoubleSkill(true);
	}
	if (strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Kankuro") == 0 ||
		strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Chiyo") == 0 ||
		strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Kiba") == 0)
	{
		skill2Button->setLock();
	}
	uiBatch->addChild(skill2Button);

	skill3Button = ActionButton::create(CCString::createWithFormat("%s_skill3.png", _delegate->currentPlayer->getCharacter()->getCString())->getCString());
	skill3Button->setCD(CCString::createWithFormat("%d", _delegate->currentPlayer->_sattackcoldDown3 * 1000));
	skill3Button->setDelegate(this);
	skill3Button->setABType(SKILL3);
	if (_delegate->currentPlayer->_sattack3isDouble)
	{
		skill3Button->setDoubleSkill(true);
	}
	if (strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Kiba") == 0 ||
		strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Chiyo") == 0 ||
		strcmp(_delegate->currentPlayer->getCharacter()->getCString(), "Lee") == 0)
	{
		skill3Button->setLock();
	}

	uiBatch->addChild(skill3Button);

	skill4Button = ActionButton::create(CCString::createWithFormat("%s_skill4.png", _delegate->currentPlayer->getCharacter()->getCString())->getCString());
	skill4Button->setDelegate(this);
	skill4Button->setABType(OUGIS1);
	uiBatch->addChild(skill4Button);

	skill5Button = ActionButton::create(CCString::createWithFormat("%s_skill5.png", _delegate->currentPlayer->getCharacter()->getCString())->getCString());
	skill5Button->setDelegate(this);
	skill5Button->setABType(OUGIS2);
	uiBatch->addChild(skill5Button);

	//init Item
	item1Button = ActionButton::create("item1.png");
	if (_delegate->_isHardCoreGame)
	{
		item1Button->setCD(CCString::createWithFormat("%d", 5000));
	}
	else
	{
		item1Button->setCD(CCString::createWithFormat("%d", 3000));
	}

	item1Button->setDelegate(this);
	item1Button->setABType(Item1);
	item1Button->_cost = "50";
	uiBatch->addChild(item1Button);

	//init Item
	item2Button = ActionButton::create("item2.png");
	item2Button->setCD(CCString::createWithFormat("%d", 15000));
	item2Button->setDelegate(this);
	item2Button->setABType(GearItem);
	item2Button->_gearType = None;
	item2Button->_isLock = true;
	uiBatch->addChild(item2Button);
	item2Button->setVisible(false);

	item3Button = ActionButton::create("item3.png");
	item3Button->setCD(CCString::createWithFormat("%d", 15000));
	item3Button->setDelegate(this);
	item3Button->setABType(GearItem);
	item3Button->_gearType = None;
	item3Button->_isLock = true;
	uiBatch->addChild(item3Button);
	item3Button->setVisible(false);

	item4Button = ActionButton::create("item4.png");
	item4Button->setCD(CCString::createWithFormat("%d", 15000));
	item4Button->setDelegate(this);
	item4Button->setABType(GearItem);
	item4Button->_gearType = None;
	item4Button->_isLock = true;
	uiBatch->addChild(item4Button);
	item4Button->setVisible(false);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	nAttackButton->setPosition(ccp(winSize.width - 60, 8));
	skill1Button->setPosition(ccp(winSize.width - skill1Button->getContentSize().width - 64, 2));
	skill2Button->setPosition(ccp(winSize.width - 95, 50));
	skill3Button->setPosition(ccp(winSize.width - 44, 8 + nAttackButton->getContentSize().height + 8));
	skill4Button->setPosition(ccp(skill1Button->getPositionX() - skill4Button->getContentSize().width - 8, 2));
	skill5Button->setPosition(ccp(skill4Button->getPositionX() - skill5Button->getContentSize().width - 8, 2));

	item1Button->setPosition(ccp(8, skill3Button->getPositionY() + skill3Button->getContentSize().height));
	item2Button->setPosition(ccp(winSize.width - 44, skill3Button->getPositionY() + skill3Button->getContentSize().height + 8));
	if (skill5Button)
		item3Button->setPosition(ccp(skill5Button->getPositionX() - skill5Button->getContentSize().width - 8, 2));
	else if (skill4Button)
		item3Button->setPosition(ccp(skill4Button->getPositionX() - skill4Button->getContentSize().width - 8, 2));
	else
		item3Button->setPosition(ccp(skill1Button->getPositionX() - skill1Button->getContentSize().width - 8, 2));
	item4Button->setPosition(ccp(skill2Button->getPositionX() - skill2Button->getContentSize().width - 8, skill2Button->getPositionY()));
#else
	float width = skill1Button->getContentSize().width;

	nAttackButton->setPosition(ccp(winSize.width + 100, -100));

	skill1Button->setPosition(ccp(winSize.width / 2 - (width + 8) * 2, 2));
	skill1Button->setScale(DESKTOP_UI_SCALE);
	skill2Button->setPosition(ccp(winSize.width / 2 - width - 8, 2));
	skill2Button->setScale(DESKTOP_UI_SCALE);
	skill3Button->setPosition(ccp(winSize.width / 2, 2));
	skill3Button->setScale(DESKTOP_UI_SCALE);
	skill4Button->setPosition(ccp(winSize.width / 2 + width + 8, 2));
	skill4Button->setScale(DESKTOP_UI_SCALE);
	skill5Button->setPosition(ccp(winSize.width / 2 + (width + 8) * 2, 2));
	skill5Button->setScale(DESKTOP_UI_SCALE);

	item1Button->setScale(DESKTOP_UI_SCALE);
	item2Button->setScale(DESKTOP_UI_SCALE);
	item3Button->setScale(DESKTOP_UI_SCALE);
	item4Button->setScale(DESKTOP_UI_SCALE);

	// Ramen	Item
	item1Button->setPosition(ccp(8, 8 + nAttackButton->getContentSize().height + skill1Button->getContentSize().height));
	// First 	Item (Speed Up & Stealth)
	item3Button->setPosition(ccp(winSize.width - skill1Button->getContentSize().width - 64, 2));
	// Second 	Item (変わり身の術)
	item4Button->setPosition(ccp(winSize.width - 95, 50));
	// Third 	Item (Trap)
	item2Button->setPosition(ccp(winSize.width - 44, 8 + nAttackButton->getContentSize().height + 8));
#endif

	// Set mask
	skill1Button->setMarkSprite("skill_freeze.png");
	skill2Button->setMarkSprite("skill_freeze.png");
	skill3Button->setMarkSprite("skill_freeze.png");
	skill4Button->setOugisMark();
	skill5Button->setOugisMark();

	item1Button->setMarkSprite("skill_freeze.png");
	item2Button->setMarkSprite("skill_freeze.png");
	item3Button->setMarkSprite("skill_freeze.png");
	item4Button->setMarkSprite("skill_freeze.png");

	//init gear
	updateGears();
	miniLayer = CCLayer::create();
	miniLayer->setAnchorPoint(ccp(0, 0));
	miniLayer->setPosition(ccp(winSize.width - 112, winSize.height - 38));
	CCObject *pObject;
	CCARRAY_FOREACH(_delegate->_TowerArray, pObject)
	{
		CharacterBase *player = (CharacterBase *)pObject;
		const char *path = "";
		MiniIcon *mi;

		if (strcmp(player->getGroup()->getCString(), _delegate->currentPlayer->getGroup()->getCString()) == 0)
		{
			path = "tower_icon1.png";
		}
		else
		{
			path = "tower_icon2.png";
		}
		mi = MiniIcon::create(path, false);
		mi->_delegate = miniLayer;
		miniLayer->addChild(mi);
		mi->updatePosition(player->getSpawnPoint());
		mi->setCharNO(player->getCharNO());

		if (!_towerIconArray)
		{
			_towerIconArray = CCArray::create();
			_towerIconArray->retain();
		}
		_towerIconArray->addObject(mi);
	}

	CCARRAY_FOREACH(_delegate->_CharacterArray, pObject)
	{
		CharacterBase *player = (CharacterBase *)pObject;
		if (strcmp(player->getRole()->getCString(), "Player") == 0 ||
			strcmp(player->getRole()->getCString(), "Com") == 0)
		{
			MiniIcon *mi;
			const char *path = "";
			if (strcmp(player->getRole()->getCString(), "Player") == 0)
			{
				path = "player_icon.png";
			}
			else if (strcmp(player->getRole()->getCString(), "Com") == 0)
			{
				if (strcmp(player->getCharacter()->getCString(), Guardian_Han) == 0 ||
					strcmp(player->getCharacter()->getCString(), Guardian_Roshi) == 0)
				{
					path = "guardian_icon.png";
				}
				else
				{
					if (strcmp(player->getGroup()->getCString(), _delegate->currentPlayer->getGroup()->getCString()) == 0)
					{
						path = "com_icon.png";
					}
					else
					{
						path = "enemy_icon.png";
					}
				}
			}

			mi = MiniIcon::create(path, true);
			mi->_delegate = miniLayer;
			miniLayer->addChild(mi);
			mi->updatePosition(player->getSpawnPoint());
			mi->setCharNO(player->getCharNO());
		}
	}

	addChild(miniLayer, 500);
}

void HudLayer::addMapIcon()
{

	CCObject *pObject;
	CCARRAY_FOREACH(_delegate->_CharacterArray, pObject)
	{
		CharacterBase *player = (CharacterBase *)pObject;
		if (strcmp(player->getCharacter()->getCString(), Guardian_Han) == 0 ||
			strcmp(player->getCharacter()->getCString(), Guardian_Roshi) == 0)
		{
			MiniIcon *mi;
			const char *path = "guardian_icon.png";
			mi = MiniIcon::create(path, true);
			mi->_delegate = miniLayer;
			miniLayer->addChild(mi);
			mi->updatePosition(player->getSpawnPoint());
			mi->setCharNO(player->getCharNO());
		}
	}
}

void HudLayer::updateGears()
{

	if (!gear1Button && !gear2Button && !gear3Button)
	{

		gear1Button = ActionButton::create("gearbg.png");
		gear1Button->setPosition(ccp(0, winSize.height - 92));
		gear1Button->setDelegate(this);
		gear1Button->setCD(CCString::createWithFormat("%d", 15000));
		gear1Button->setABType(GearBtn);

		gear1Button->setMarkSprite("gear_freeze.png");
		gear1Button->_gearType = None;
		addChild(gear1Button, 200);

		gear2Button = ActionButton::create("gearbg.png");
		gear2Button->setPosition(ccp(35, winSize.height - 112));
		gear2Button->setDelegate(this);
		gear2Button->setCD(CCString::createWithFormat("%d", 15000));
		gear2Button->_gearType = None;
		gear2Button->setABType(GearBtn);
		gear2Button->setMarkSprite("gear_freeze.png");

		addChild(gear2Button, 200);

		gear3Button = ActionButton::create("gearbg.png");
		gear3Button->setPosition(ccp(70, winSize.height - 92));
		gear3Button->setCD(CCString::createWithFormat("%d", 15000));
		gear3Button->setDelegate(this);
		gear3Button->_gearType = None;

		gear3Button->setABType(GearBtn);
		gear3Button->setMarkSprite("gear_freeze.png");
		addChild(gear3Button, 200);

		return;
	}

	if (_delegate->currentPlayer->getGearArray() && _delegate->currentPlayer->getGearArray()->count() > 0)
	{
		CCObject *pObject;
		int i = 0;
		CCARRAY_FOREACH(_delegate->currentPlayer->getGearArray(), pObject)
		{
			CCString *tmpGear = (CCString *)pObject;
			if (tmpGear->intValue() != gear1Button->_gearType && i == 0)
			{
				gear1Button->setGearType(tmpGear->intValue());
			}
			else if (tmpGear->intValue() != gear2Button->_gearType && i == 1)
			{
				gear2Button->setGearType(tmpGear->intValue());
			}
			else if (tmpGear->intValue() != gear3Button->_gearType && i == 2)
			{
				gear3Button->setGearType(tmpGear->intValue());
			}

			if (tmpGear->intValue() == 6 && getItem2Button()->_gearType == None)
			{
				getItem2Button()->_gearType = gearType(tmpGear->intValue());
				getItem2Button()->setVisible(true);
				getItem2Button()->_isLock = false;
			}
			else if (tmpGear->intValue() == 0 && getItem3Button()->_gearType == None)
			{

				getItem3Button()->_gearType = gearType(tmpGear->intValue());
				getItem3Button()->_isLock = false;
				getItem3Button()->setVisible(true);
			}
			else if (tmpGear->intValue() == 3 && getItem4Button()->_gearType == None)
			{

				getItem4Button()->setVisible(true);
				getItem4Button()->_gearType = gearType(tmpGear->intValue());
				getItem4Button()->_isLock = false;
			}

			i++;
		}
	}
}

void HudLayer::setHPLose(float percent)
{
	CCRotateTo *ra = CCRotateTo::create(0.2f, -((1 - percent) * 180), -((1 - percent) * 180));
	status_hpbar->runAction(ra);

	int hp = to_int(_delegate->currentPlayer->getHP()->getCString());
	hpLabel->setString(CCString::createWithFormat("%d", int(hp))->getCString());
}

void HudLayer::setCKRLose(bool isCRK2)
{

	if (!isCRK2)
	{

		if (skill4Button)
		{

			if (skill4Button->lockLabel1)
			{
				skill4Button->ougismarkSprite->setVisible(false);
				skill4Button->_isMarkVisable = false;
				CCActionInterval *su = CCScaleBy::create(0.2f, 1.1f);
				CCFiniteTimeAction *call = CCCallFunc::create(skill4Button, callfunc_selector(ActionButton::setProgressMark));
				CCArray *list = CCArray::create();
				list->addObject(su);
				list->addObject(su->reverse());
				list->addObject(call);
				CCAction *seq = CCSequence::create(list);
				skill4Button->runAction(seq);
			}
			else
			{
				skill4Button->updateProgressMark();
			}
		}
	}
	else
	{

		if (skill5Button)
		{

			if (skill5Button->lockLabel1)
			{
				skill5Button->ougismarkSprite->setVisible(false);
				skill5Button->_isMarkVisable = false;
				CCActionInterval *su = CCScaleBy::create(0.2f, 1.1f);
				CCFiniteTimeAction *call = CCCallFunc::create(skill5Button, callfunc_selector(ActionButton::setProgressMark));
				CCArray *list = CCArray::create();
				list->addObject(su);
				list->addObject(su->reverse());
				list->addObject(call);
				CCAction *seq = CCSequence::create(list);
				skill5Button->runAction(seq);
			}
			else
			{
				skill5Button->updateProgressMark();
			}
		}
	}
}

void HudLayer::setEXPLose(float percent)
{

	int exp = _delegate->currentPlayer->getEXP();
	int lvExp = (_delegate->currentPlayer->getLV() - 1) * 500;
	float Percent = (exp - lvExp) / 500.0 * 100;
	if (Percent > 100)
	{
		Percent = 100;
	}
	//CCRotateTo* ra=CCRotateTo::create(0.2f,((1-Percent/100)*90),((1-Percent/100)*90));
	//status_expbar->runAction(ra);
	status_expbar->setPercentage((1 + Percent / 100) * 50);

	if (exp >= 2500)
	{
		status_expbar->setPercentage(100);
		expLabel->setString("Max");
	}
	else
	{
		expLabel->setString(CCString::createWithFormat("%d%%", int(Percent))->getCString());
	}
}

void HudLayer::setTowerState(unsigned int charNO)
{
	CCObject *pObject;
	CCARRAY_FOREACH(_towerIconArray, pObject)
	{
		MiniIcon *mi = (MiniIcon *)pObject;
		if (mi->getCharNO() == charNO)
		{
			mi->updateState();
		}
	}
}

void HudLayer::setCoin(const char *value)
{

	const char *cl = coinLabel->getString();
	int tempCoin = to_int(cl);
	tempCoin += to_int(value);
	CCString *realCoin = CCString::createWithFormat("%d", tempCoin);
	coinLabel->setString(realCoin->getCString());
}

bool HudLayer::offCoin(const char *value)
{

	const char *cl = coinLabel->getString();
	int tempCoin = to_int(cl);
	if (tempCoin - to_int(value) >= 0)
	{
		tempCoin -= to_int(value);
		CCString *realCoin = CCString::createWithFormat("%d", tempCoin);
		coinLabel->setString(realCoin->getCString());
		_delegate->currentPlayer->minusCoin(to_int(value));
		return true;
	}
	else
	{
		return false;
	}
}

void HudLayer::setReport(const char *name1, const char *name2, CCString *killNum)
{
	float length;

	if (!_reportListArray)
	{
		_reportListArray = CCArray::create();
		_reportListArray->retain();
	}
	else if (_reportListArray->count() > 0)
	{
		CCDictionary *dic = CCDictionary::create();
		dic->setObject(CCString::create(name1), "name1");
		dic->setObject(CCString::create(name2), "name2");
		dic->setObject(CCString::createWithFormat("%d", 1), "num");
		dic->setObject(killNum, "kills");
		_reportListArray->addObject(dic);
		return;
	}

	reportSprite = createReport(name1, name2, length);
	reportSprite->setPosition(ccp(winSize.width / 2 - length / 2, winSize.height - 80));
	addChild(reportSprite, 500);

	CCActionInterval *su = CCScaleBy::create(0.2f, 1.0f);
	CCFiniteTimeAction *call = CCCallFunc::create(this, callfunc_selector(HudLayer::setReportCache));
	CCDelayTime *delay = CCDelayTime::create(2.0f);
	CCAction *seq = CCSequence::create(su, su->reverse(), delay, call, nullptr);
	reportSprite->runAction(seq);

	CCDictionary *dic = CCDictionary::create();
	dic->setObject(CCString::create(name1), "name1");
	dic->setObject(CCString::create(name2), "name2");
	dic->setObject(CCString::createWithFormat("%d", 1), "num");
	dic->setObject(killNum, "kills");
	dic->setObject(CCString::create("True"), "isDisplay");
	_reportListArray->addObject(dic);
}

void HudLayer::setBuffDisplay(const char *buffName, float buffStayTime)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	float scale = 0.35f;
#else
	float scale = 0.40f;
#endif

	CCSprite *buffSprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_%s.png", _delegate->currentPlayer->getCharacter()->getCString(), buffName)->getCString());
	if (buffSprite == nullptr)
		return;
	buffSprite->setAnchorPoint(ccp(0, 0));
	buffSprite->setPosition(ccp(8, item1Button->getPositionY() + 76 - 28 * _buffCount));
	if (buffStayTime != 0)
	{
		if (_buffCount == 0)
		{

			bcdLabel1 = CCLabelBMFont::create(CCString::createWithFormat("%d", (int)buffStayTime)->getCString(), "Fonts/1.fnt");
			bcdLabel1->setScale(scale);
			bcdLabel1->setPosition(ccp(5, 5));
			bcdLabel1->setAnchorPoint(ccp(0.5, 0.5));
			buffSprite->addChild(bcdLabel1);
			schedule(schedule_selector(HudLayer::updateBuffDisplay), 1.0f);
		}
		else
		{
			bcdLabel2 = CCLabelBMFont::create(CCString::createWithFormat("%d", (int)buffStayTime)->getCString(), "Fonts/1.fnt");
			bcdLabel2->setScale(scale);
			bcdLabel2->setPosition(ccp(5, 5));
			bcdLabel2->setAnchorPoint(ccp(0.5, 0.5));
			buffSprite->addChild(bcdLabel2);
			schedule(schedule_selector(HudLayer::updateBuffDisplay2), 1.0f);
		}
	}

	CCFiniteTimeAction *call = CCCallFuncN::create(buffSprite, callfuncN_selector(HudLayer::clearBuffDisplay));
	CCDelayTime *delay = CCDelayTime::create(buffStayTime - 3);

	CCArray *seqArray = CCArray::create();
	seqArray->addObject(delay);
	seqArray->addObject(call);

	CCAction *seq = CCSequence::create(seqArray);
	buffSprite->runAction(seq);
	buffSprite->setTag(0);

	buffSprite->retain();
	addChild(buffSprite, 500);
	_buffCount++;
}

void HudLayer::updateBuffDisplay(float dt)
{

	if (bcdLabel1)
	{
		if (to_int(bcdLabel1->getString()) > 0)
		{
			bcdLabel1->setString(CCString::createWithFormat("%d", to_int(bcdLabel1->getString()) - 1)->getCString());
		}
		else
		{
			bcdLabel1->removeFromParentAndCleanup(true);
			bcdLabel1 = nullptr;
			_buffCount--;
		}
	}
}

void HudLayer::updateBuffDisplay2(float dt)
{

	if (bcdLabel2)
	{
		if (to_int(bcdLabel2->getString()) > 0)
		{
			bcdLabel2->setString(CCString::createWithFormat("%d", to_int(bcdLabel2->getString()) - 1)->getCString());
		}
		else
		{
			bcdLabel2->removeFromParentAndCleanup(true);
			bcdLabel2 = nullptr;
			_buffCount--;
		}
	}
}

void HudLayer::clearBuffDisplay(CCNode *sender)
{
	CCSprite *bs = (CCSprite *)sender;
	if (bs->getTag() == 0)
	{
		CCFiniteTimeAction *call = CCCallFuncN::create(bs, callfuncN_selector(HudLayer::clearBuffDisplay));
		CCDelayTime *delay = CCDelayTime::create(4.0f);
		bs->setTag(1);
		CCFadeOut *fade = CCFadeOut::create(0.5f);
		CCAction *fadeseq = CCRepeatForever::create(CCSequence::createWithTwoActions(fade, fade->reverse()));
		CCAction *seq = CCSequence::createWithTwoActions(delay, call);
		bs->runAction(seq);
		bs->runAction(fadeseq);
	}
	else
	{
		bs->removeFromParentAndCleanup(true);
	}
}

void HudLayer::clearSPCReport()
{
	if (reportSPCSprite)
	{
		reportSPCSprite->stopAllActions();
		reportSPCSprite->removeFromParent();
		reportSPCSprite = nullptr;
	}
}

void HudLayer::stopSPCReport()
{
	if (reportSPCSprite)
	{
		reportSPCSprite->stopAllActions();
		unschedule(schedule_selector(HudLayer::updateSPCReprot));
		reportSPCSprite->setPosition(ccp(winSize.width / 2, winSize.height - 105));
		reportSPCSprite->setScale(1.0f);
		CCDelayTime *delay = CCDelayTime::create(1.6f);
		CCFiniteTimeAction *call = CCCallFunc::create(this, callfunc_selector(HudLayer::clearSPCReport));
		reportSPCSprite->runAction(CCSequence::createWithTwoActions(delay, call));
	}
}

void HudLayer::updateSPCReprot(float dt)
{
	if (reportSPCSprite)
	{
		reportSPCSprite->setPosition(ccp(winSize.width / 2, winSize.height - 105));
	}
}

void HudLayer::setReportCache()
{
	reportSprite->removeFromParent();
	CCObject *pObject;

	CCARRAY_FOREACH(_reportListArray, pObject)
	{
		CCDictionary *tempdict = (CCDictionary *)pObject;
		CCString *isDisplay = CCString::create(tempdict->valueForKey("isDisplay")->getCString());

		if (strcmp(isDisplay->getCString(), "True") != 0)
		{

			CCString *name1 = CCString::create(tempdict->valueForKey("name1")->getCString());
			CCString *name2 = CCString::create(tempdict->valueForKey("name2")->getCString());
			CCString *num = CCString::create(tempdict->valueForKey("num")->getCString());
			CCString *killnum = CCString::create(tempdict->valueForKey("kills")->getCString());

			float length;
			reportSprite = createReport(name1->getCString(), name2->getCString(), length);
			reportSprite->setPosition(ccp(winSize.width / 2 - length / 2, winSize.height - 80));
			addChild(reportSprite, 500);

			CCActionInterval *su = CCScaleBy::create(0.2f, 1.1f);
			CCFiniteTimeAction *call = CCCallFunc::create(this, callfunc_selector(HudLayer::setReportCache));
			CCDelayTime *delay = CCDelayTime::create(2.0f);
			CCAction *seq = CCSequence::create(su, su->reverse(), delay, call, nullptr);

			reportSprite->runAction(seq);

			if (strcmp(name2->getCString(), "Tower") != 0)
			{
				CCObject *pObject2;
				int num2 = 0;
				CCARRAY_FOREACH(_reportListArray, pObject2)
				{
					CCDictionary *tempdict2 = (CCDictionary *)pObject2;
					CCString *tempName = CCString::create(tempdict2->valueForKey("name1")->getCString());
					CCString *tempName2 = CCString::create(tempdict2->valueForKey("name2")->getCString());
					CCString *isDisplay2 = CCString::create(tempdict2->valueForKey("isDisplay")->getCString());

					if (strcmp(tempName->getCString(), name1->getCString()) == 0 &&
						strcmp(tempName2->getCString(), "Tower") != 0 &&
						strcmp(isDisplay2->getCString(), "True") == 0)
					{
						num2 += 1;
					}
				}

				// ɱ
				if (num2 == 1)
				{

					if (reportSPCSprite)
					{
						reportSPCSprite->stopAllActions();
						reportSPCSprite->removeFromParent();
					}
					reportSPCSprite = createSPCReport(killnum->getCString(), num2);
					reportSPCSprite->setPosition(ccp(winSize.width / 2, winSize.height - 115));
					addChild(reportSPCSprite, 500);
					CCFadeIn *fn = CCFadeIn::create(0.8f);
					CCMoveBy *mv = CCMoveBy::create(0.8f, ccp(0, 10));
					CCSpawn *sp = CCSpawn::createWithTwoActions(fn, mv);
					CCDelayTime *delay2 = CCDelayTime::create(1.6f);
					CCFiniteTimeAction *call2 = CCCallFunc::create(this, callfunc_selector(HudLayer::clearSPCReport));
					CCArray *list = CCArray::create();
					list->addObject(sp);
					list->addObject(delay2);
					list->addObject(call2);
					CCAction *seq2 = CCSequence::create(list);
					reportSPCSprite->runAction(seq2);
				}
				else if (num2 >= 2)
				{

					if (reportSPCSprite)
					{
						reportSPCSprite->stopAllActions();
						reportSPCSprite->removeFromParent();
					}

					reportSPCSprite = createSPCReport(killnum->getCString(), num2);
					reportSPCSprite->setScale(2.0);
					reportSPCSprite->setPosition(ccp(winSize.width / 2, winSize.height - 105));
					addChild(reportSPCSprite, 500);
					CCScaleTo *st = CCScaleTo::create(0.8f, 1.0f, 1.0f);
					CCFiniteTimeAction *call2 = CCCallFunc::create(this, callfunc_selector(HudLayer::stopSPCReport));
					CCAction *seq2 = CCSequence::createWithTwoActions(st, call2);
					reportSPCSprite->runAction(seq2);

					CCFiniteTimeAction *sk = CCRepeatForever::create(CCShake::create(0.2f, 4));
					reportSPCSprite->runAction(sk);

					schedule(schedule_selector(HudLayer::updateSPCReprot), 0.2f);
				}
			}

			tempdict->setObject(CCString::create("True"), "isDisplay");
			return;
		}
	}

	if (_reportListArray)
	{

		_reportListArray->removeAllObjects();
		_reportListArray = nullptr;
	}
}

CCSprite *HudLayer::createSPCReport(const char *killNum, int num)
{

	bool isBrocast = false;
	if (CCUserDefault::sharedUserDefault()->getBoolForKey("isVoice"))
	{
		isBrocast = true;
	}

	if (num == 1)
	{

		if (to_int(killNum) < 10)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("DoubleKill_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill2_1.ogg");
		}
		else if (to_int(killNum) < 20)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("KillingSpree_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill2_2.ogg");
		}
		else if (to_int(killNum) < 30)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("Unstoppable_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill2_3.ogg");
		}
		else if (to_int(killNum) >= 30)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("Godlike_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill2_4.ogg");
		}
	}
	else if (num == 2)
	{

		if (to_int(killNum) < 10)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("TripeKill_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill3_1.ogg");
		}
		else if (to_int(killNum) < 20)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("Domination_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill3_2.ogg");
		}
		else if (to_int(killNum) < 30)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("Rampage_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill3_3.ogg");
		}
		else if (to_int(killNum) >= 30)
		{
			reportSPCSprite = CCSprite::createWithSpriteFrameName("Holyshit_rpf.png");
			if (isBrocast)
				SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill3_4.ogg");
		}
	}
	else
	{
		reportSPCSprite = CCSprite::createWithSpriteFrameName("MonsterKill_rpf.png");
		if (isBrocast)
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/kill4.ogg");
	}

	return reportSPCSprite;
}

CCSprite *HudLayer::createReport(const char *name1, const char *name2, float &length)
{

	CCSprite *reportSprite = CCSprite::create();

	CCSprite *slain_p = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_rp.png", name1)->getCString());
	slain_p->setAnchorPoint(ccp(0, 0.5f));
	slain_p->setPosition(ccp(0, 0));
	reportSprite->addChild(slain_p);

	CCSprite *slain_pf = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_rpf.png", name1)->getCString());
	slain_pf->setPosition(ccp(slain_p->getContentSize().width, 0));
	slain_pf->setAnchorPoint(ccp(0, 0.5f));
	reportSprite->addChild(slain_pf);

	CCSprite *slain;
	if (strcmp(name2, "Tower") != 0)
	{
		slain = CCSprite::createWithSpriteFrameName("slain.png");

		reportSprite->addChild(slain);
	}
	else
	{
		slain = CCSprite::createWithSpriteFrameName("destroy.png");
		reportSprite->addChild(slain);
	}
	slain->setAnchorPoint(ccp(0, 0.5f));
	slain->setPosition(ccp(slain_pf->getPositionX() + slain_pf->getContentSize().width, 0));

	CCSprite *death_pf = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_rpf.png", name2)->getCString());
	death_pf->setAnchorPoint(ccp(0, 0.5f));
	death_pf->setPosition(ccp(slain->getPositionX() + slain->getContentSize().width, 0));
	reportSprite->addChild(death_pf);
	CCSprite *death_p;
	if (strcmp(name2, "Tower") != 0)
	{

		death_p = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_rp.png", name2)->getCString());
		death_p->setAnchorPoint(ccp(0, 0.5f));
		death_p->setPosition(ccp(death_pf->getPositionX() + death_pf->getContentSize().width, 0));
		reportSprite->addChild(death_p);
	}

	if (strcmp(name2, "Tower") != 0)
	{
		length = slain_p->getContentSize().width + slain_pf->getContentSize().width + slain->getContentSize().width + death_pf->getContentSize().width + death_p->getContentSize().width;
	}
	else
	{
		length = slain_p->getContentSize().width + slain_pf->getContentSize().width + slain->getContentSize().width + death_pf->getContentSize().width;
	}

	return reportSprite;
}

void HudLayer::attackButtonClick(abType type)
{
	_delegate->attackButtonClick(type);
}

void HudLayer::gearButtonClick(gearType type)
{
	_delegate->gearButtonClick(type);
}

void HudLayer::attackButtonRelease()
{
	_delegate->attackButtonRelease();
}

void HudLayer::pauseButtonClick(CCObject *sender)
{
	_delegate->onPause();
}

void HudLayer::gearButtonClick(CCObject *sender)
{
	if (!_isAllButtonLocked)
	{
		_delegate->onGear();
	}
}

bool HudLayer::getSkillFinish()
{

	if (!_delegate->getSkillFinish())
	{
		return false;
	}

	return true;
}

bool HudLayer::getOugisEnable(bool isCKR2)
{
	if (!isCKR2)
	{
		float ckr = atof(_delegate->currentPlayer->getCKR()->getCString());
		if (ckr >= 15000)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{

		float ckr = atof(_delegate->currentPlayer->getCKR2()->getCString());
		if (ckr >= 25000)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void HudLayer::costCKR(int value, bool isCKR2)
{
	if (!isCKR2)
	{
		float ckr = atof(_delegate->currentPlayer->getCKR()->getCString());

		if (ckr - value > 0)
		{
			ckr -= value;
		}
		else
		{
			ckr = 0;
		}

		_delegate->currentPlayer->setCKR(CCString::createWithFormat("%f", ckr));
		setCKRLose(false);
	}
	else
	{

		float ckr = atof(_delegate->currentPlayer->getCKR2()->getCString());

		if (ckr - value > 0)
		{
			ckr -= value;
		}
		else
		{
			ckr = 0;
		}

		_delegate->currentPlayer->setCKR2(CCString::createWithFormat("%f", ckr));
		setCKRLose(true);
	}
}

void HudLayer::setOugis(CCString *character, CCString *group)
{

	if (!ougisLayer)
	{
		CCObject *pObject;
		CCArray *childArray = getChildren();
		CCARRAY_FOREACH(childArray, pObject)
		{
			CCNode *object = (CCNode *)pObject;
			object->pauseSchedulerAndActions();
		}

		ougisLayer = CCLayer::create();
		int startPosY = 0;
		int startPosX = 0;
		int endPosX = 0;
		CCSprite *CutBg;
		const char *cutPath1;
		const char *cutPath2;
		if (_delegate->team > 0)
		{
			cutPath1 = "CutBg.png";
			cutPath2 = "CutBg2.png";
		}
		else
		{
			cutPath1 = "CutBg2.png";
			cutPath2 = "CutBg.png";
		}

		if (strcmp(group->getCString(), Konoha) == 0)
		{
			startPosY = 0;
			startPosX = -48;
			endPosX = 0;
			CutBg = CCSprite::createWithSpriteFrameName(cutPath1);
			CutBg->setAnchorPoint(ccp(0, 0));
		}
		else
		{
			startPosY = winSize.height / 2;
			startPosX = winSize.width + 48;
			endPosX = winSize.width;
			CutBg = CCSprite::createWithSpriteFrameName(cutPath2);
			CutBg->setFlipX(true);
			CutBg->setAnchorPoint(ccp(1, 0.5f));
		}

		CutBg->setPosition(ccp(startPosX, startPosY));
		CutBg->runAction(CCMoveTo::create(0.3f, ccp(endPosX, startPosY)));
		ougisLayer->addChild(CutBg);

		CCArray *animeFrames = CCArray::create();
		for (int i = 1; i <= 8; i++)
		{
			CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("CutLine_%02d.png", i)->getCString());
			animeFrames->addObject(frame);
		}

		CCAnimation *tempAnimation = CCAnimation::createWithSpriteFrames(animeFrames, 0.1f);
		CCAction *tempAction = CCAnimate::create(tempAnimation);

		CCSprite *CutLine = CCSprite::createWithSpriteFrameName("CutLine_01.png");
		if (strcmp(group->getCString(), Konoha) == 0)
		{
			CutLine->setAnchorPoint(ccp(0, 0));
			CutLine->setPosition(ccp(-48, 0));
			CutLine->runAction(tempAction);
			CutLine->runAction(CCMoveTo::create(0.3f, ccp(0, 0)));
		}
		else
		{
			CutLine->setFlipX(true);
			CutLine->setAnchorPoint(ccp(1, 0.5f));
			CutLine->setPosition(ccp(winSize.width + 48, winSize.height / 2));
			CutLine->runAction(tempAction);
			CutLine->runAction(CCMoveTo::create(0.3f, ccp(winSize.width, winSize.height / 2)));
		}

		ougisLayer->addChild(CutLine);

		CCSprite *CutLineUP = CCSprite::createWithSpriteFrameName("CutLineUP.png");
		if (strcmp(group->getCString(), Konoha) == 0)
		{
			CutLineUP->setAnchorPoint(ccp(0, 0));
			CutLineUP->setPosition(ccp(-48, 2));
			CutLineUP->runAction(CCMoveTo::create(0.3f, ccp(-1, 1)));
		}
		else
		{
			CutLineUP->setFlipX(true);
			CutLineUP->setAnchorPoint(ccp(1, 0.5f));
			CutLineUP->setPosition(ccp(winSize.width + 48, winSize.height / 2 + 2));
			CutLineUP->runAction(CCMoveTo::create(0.3f, ccp(winSize.width + 1, winSize.height / 2 + 1)));
		}

		ougisLayer->addChild(CutLineUP);

		CCSprite *CutIn = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_CutIn.png", character->getCString())->getCString());
		if (strcmp(group->getCString(), Konoha) == 0)
		{
			CutIn->setAnchorPoint(ccp(0, 0));
			CutIn->setPosition(ccp(-200, 10));
			CutIn->runAction(CCMoveTo::create(0.3f, ccp(0, 10)));
		}
		else
		{
			CutIn->setAnchorPoint(ccp(1, 0));
			CutIn->setPosition(ccp(winSize.width + 200, winSize.height / 2 + 10 - CutBg->getContentSize().height / 2));
			CutIn->runAction(CCMoveTo::create(0.3f, ccp(winSize.width, winSize.height / 2 + 10 - CutBg->getContentSize().height / 2)));
		}
		ougisLayer->addChild(CutIn);

		CCSprite *CutLineDown = CCSprite::createWithSpriteFrameName("CutLineDown.png");
		if (strcmp(group->getCString(), Konoha) == 0)
		{
			CutLineDown->setAnchorPoint(ccp(0, 0));
			CutLineDown->setPosition(ccp(-48, 0));
			CutLineDown->runAction(CCMoveTo::create(0.3f, ccp(0, 0)));
		}
		else
		{
			CutLineDown->setFlipX(true);
			CutLineDown->setAnchorPoint(ccp(1, 0));
			CutLineDown->setPosition(ccp(winSize.width + 48, winSize.height / 2 - CutBg->getContentSize().height / 2));
			CutLineDown->runAction(CCMoveTo::create(0.3f, ccp(winSize.width, winSize.height / 2 - CutBg->getContentSize().height / 2)));
		}

		ougisLayer->addChild(CutLineDown);

		ougisLayer->setPosition(ccp(0, 0));
		addChild(ougisLayer, -100);

		CCDelayTime *delay = CCDelayTime::create(1.0f);
		CCFiniteTimeAction *call = CCCallFuncN::create(this, callfuncN_selector(HudLayer::removeOugis));
		ougisLayer->runAction(CCSequence::createWithTwoActions(delay, call));
	}
}

void HudLayer::removeOugis(CCNode *sender)
{
	ougisLayer->removeFromParent();
	ougisLayer = nullptr;

	CCObject *pObject;
	CCArray *childArray = getChildren();
	CCARRAY_FOREACH(childArray, pObject)
	{
		CCNode *object = (CCNode *)pObject;
		object->resumeSchedulerAndActions();
	}

	_delegate->removeOugis();
}

void HudLayer::updateSkillButtons()
{
	auto player = _delegate->currentPlayer;
	if (!player)
		return;
	std::string charName = player->getCharacter()->getCString();
	if (charName.length() == 0)
		return;

	auto cache = CCSpriteFrameCache::sharedSpriteFrameCache();

#define updateButtonInfo(index)                                                                                       \
	if (skill##index##Button)                                                                                         \
	{                                                                                                                 \
		skill##index##Button->setDisplayFrame(cache->spriteFrameByName((charName + "_skill" #index ".png").c_str())); \
		skill##index##Button->setCD(CCString::createWithFormat("%d", player->_sattackcoldDown1 * 1000));              \
		skill##index##Button->_isColdChanged = true;                                                                  \
	}

	updateButtonInfo(1);
	updateButtonInfo(2);
	updateButtonInfo(3);
	updateButtonInfo(4);
	updateButtonInfo(5);
}
