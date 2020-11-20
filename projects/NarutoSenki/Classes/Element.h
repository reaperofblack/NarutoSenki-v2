#pragma once
#include "CharacterBase.h"
#include "HPBar.h"

class HeroElement : public CharacterBase
{
public:
	HeroElement();
	~HeroElement();

	bool init();
	void initAction();
	void setHPbar();
	void setShadows();
	void changeHPbar();
	void checkRefCount(float dt);

	CCSprite *rebornSprite;
	CCLabelBMFont *rebornLabel;
	void reborn(float dt);
	void dealloc();

protected:
	void countDown(float dt);
};

class Monster : public CharacterBase
{
public:
	Monster();
	~Monster();

	CREATE_FUNC(Monster);
	bool init();
	void initAction();
	void setID(CCString *character, CCString *role, CCString *group);
	void setHPbar();
	void changeHPbar();
	void setDirectMove(unsigned int length, float delay, bool isReverse);
	void setEaseIn(unsigned int length, float delay);
	void setDirectMoveBy(unsigned int length, float delay);

protected:
	void dealloc();
	void dealloc2();
	void setAI(float dt);
	void setResume();
};

#include "Core/Tower/Tower.hpp"

#include "Core/Warrior/Flog.hpp"

#include "Core/Bullet/Bullet.hpp"
