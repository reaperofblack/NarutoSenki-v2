#include "MyUtils/Cocos2dxHelper.hpp"
#include "StartMenu.h"
#include <time.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "../../../cocos2dx/platform/android/jni/JniHelper.h"
#endif

SelectLayer::SelectLayer()
{
	_selectList = nullptr;
	_com1Select = nullptr;
	_com2Select = nullptr;

	isStart = false;
	_isRandomChar = false;

	_playerSelect = nullptr;
}

SelectLayer::~SelectLayer()
{
}

void SelectLayer::onGameStart()
{
	if (!isStart)
	{
		isStart = true;
		SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");

		CCDictionary *dic = CCDictionary::create();

		CCString *tmpChar;

		if (_playerSelect)
		{
			tmpChar = CCString::create(_playerSelect);
		}
		else
		{
			srand((int)time(0));
			int num2 = _selectList->count();
			int index = random(num2);
			tmpChar = (CCString *)_selectList->objectAtIndex(index);
			do
			{
				srand((int)time(0));
				int num = _selectList->count();
				int index = random(num);
				tmpChar = (CCString *)_selectList->objectAtIndex(index);
			} while (strcmp(tmpChar->getCString(), "Pain") == 0 ||
					 strcmp(tmpChar->getCString(), "Orochimaru") == 0 ||
					 strcmp(tmpChar->getCString(), "None") == 0);

			_playerSelect = tmpChar->getCString();
			_isRandomChar = true;
		}

		srand((int)time(0));
		int zhenying = 1;
		zhenying = random(2);

		const char *groupName;
		if (zhenying > 0)
		{
			groupName = Konoha;
		}
		else
		{
			groupName = Akatsuki;
		}

		CCString *tmpRole = CCString::create("Player");
		CCString *tmpGroup = CCString::create(groupName);

		dic->setObject(tmpChar, "character");
		dic->setObject(tmpRole, "role");
		dic->setObject(tmpGroup, "group");

		CCArray *tempHeros = CCArray::createWithObject(dic);

		CCArray *realHero = CCArray::create();

		for (int i = 0; i < heroNum; i++)
		{

			if (strcmp(_playerSelect, heroList[i]) == 0)
			{
				continue;
			}

			if (_com1Select)
			{
				if (strcmp(_com1Select, heroList[i]) == 0)
				{
					continue;
				}
			}
			if (_com2Select)
			{
				if (strcmp(_com2Select, heroList[i]) == 0)
				{
					continue;
				}
			}

			CCString *hero = CCString::create(heroList[i]);
			realHero->addObject(hero);
		}

		for (int i = 0; i < ComCount; i++)
		{
			CCString *hero;
			if (i < KonohaCount)
			{

				if (i == 0 && _com1Select)
				{
					hero = CCString::create(_com1Select);
				}
				else if (i == 1 && _com2Select)
				{
					hero = CCString::create(_com2Select);
				}
				else
				{
					int length = realHero->count();
					srand((int)time(0));
					int index = random(length);
					if (index == (int)realHero->count())
					{
						index = realHero->count() - 1;
					}

					CCObject *tempObject = realHero->objectAtIndex(index);
					hero = (CCString *)tempObject;
					realHero->removeObjectAtIndex(index);
				}

				dic = CCDictionary::create();
				tmpChar = CCString::create(hero->getCString());
				tmpRole = CCString::create("Com");
				if (zhenying > 0)
				{
					tmpGroup = CCString::create(Konoha);
				}
				else
				{
					tmpGroup = CCString::create(Akatsuki);
				}
				dic->setObject(tmpChar, "character");
				dic->setObject(tmpRole, "role");
				dic->setObject(tmpGroup, "group");

				tempHeros->addObject(dic);
			}
			else
			{
				int length = realHero->count();
				srand((int)time(0));
				int index = random(length);
				if (index == (int)realHero->count())
				{
					index = realHero->count() - 1;
				}

				CCObject *tempObject = realHero->objectAtIndex(index);
				CCString *hero = (CCString *)tempObject;

				dic = CCDictionary::create();
				tmpChar = CCString::create(hero->getCString());
				tmpRole = CCString::create("Com");
				if (zhenying > 0)
				{
					tmpGroup = CCString::create(Akatsuki);
				}
				else
				{
					tmpGroup = CCString::create(Konoha);
				}
				dic->setObject(tmpChar, "character");
				dic->setObject(tmpRole, "role");
				dic->setObject(tmpGroup, "group");

				tempHeros->addObject(dic);
				realHero->removeObjectAtIndex(index);
			}
		}

		CCScene *loadScene = CCScene::create();
		LoadLayer *loadLayer = LoadLayer::create();
		loadLayer->tempHeros = tempHeros;
		loadScene->addChild(loadLayer);

		loadLayer->_isRandomChar = _isRandomChar;
		loadLayer->preloadAudio();
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f, loadScene));
	}
}

void SelectLayer::keyBackClicked(){
	lua_call_func("backToStartMenu")}

SelectLayer *SelectLayer::create()
{

	SelectLayer *sl = new SelectLayer();
	if (sl && sl->init())
	{
		sl->autorelease();
		return sl;
	}
	else
	{
		delete sl;
		return nullptr;
	}
}
