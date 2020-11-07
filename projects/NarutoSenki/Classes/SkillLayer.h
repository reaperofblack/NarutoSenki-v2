#pragma once
#include "Defines.h"

// declare menuButton
enum sbtnType
{
	Menu,
	Unlock1,
	Unlock2,
	skill1,
	skill2,
	skill3,
	Ougis1,
	Ougis2
};

class SelectLayer;

class SkillLayer : public CCLayer
{
public:
	SkillLayer();
	~SkillLayer();

	bool init();

	void initInterface();

	CREATE_FUNC(SkillLayer);
	CCSprite *_skillExplain;

	bool isPosting;
	CCSprite *refreshBtn;

	CCLabelBMFont *coinLabel;

	CCLayer *buyLayer;
	sbtnType _buyType;
	CCSprite *_heroFull;
	CCMenu *changemenu;
	bool _autoMove;

	CCSprite *fengSprite;
	CCSprite *fengSprite2;
	CCSprite *skillGroup;

	CCSprite *skillSprite;

	void update(float dt);

	const char *selectHero;

	void setSkillExplain(sbtnType type);
	CCProgressTimer *bonds;

	CC_SYNTHESIZE(SelectLayer *, _delegate, Delegate);

private:
	void onCancel(CCObject *sender);

	void updateSkillGroup();
	void onChangeBtn(CCObject *sender);
	void onResume(CCObject *sender);
	void onSkillRequestCompleted(CCHttpClient *client, CCHttpResponse *response);
};

class SelectButton : public CCSprite, public CCTouchDelegate
{
public:
	SelectButton();
	~SelectButton();

	CC_SYNTHESIZE_RETAIN(CCString *, _charName, CharName);

	bool _isAviable;
	virtual bool init(const char *szImage);
	static SelectButton *create(const char *szImage);
	CCRect getRect();
	void click();
	int _clickTime;
	CC_SYNTHESIZE(sbtnType, _btnType, BtnType);

	bool _isCanBuy;
	bool _isCanBuy2;
	bool _isUnlock;

	CC_SYNTHESIZE(SelectLayer *, _selectLayer, Delegate1);
	CC_SYNTHESIZE(SkillLayer *, _skillLayer, Delegate2);

	void spriteToGrey();

protected:
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);

	inline bool containsTouchLocation(CCTouch *touch);
};
