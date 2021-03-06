#pragma once
#include "Hero.hpp"
#include "Kuchiyose/Karasu.hpp"
#include "Kuchiyose/Sanshouuo.hpp"
#include "Kuchiyose/Saso.hpp"

class Kankuro : public Hero
{
	void perform()
	{
		_mainTarget = nullptr;
		findEnemy2("Hero");

		if (_isCanGear06)
		{
			if ((getActionState() == State::FLOAT ||
				 getActionState() == State::AIRHURT ||
				 getActionState() == State::HURT ||
				 getActionState() == State::KNOCKDOWN) &&
				getHpPercent() < 0.5 && !_isArmored && !_isInvincible)
			{
				useGear(gear06);
			}
		}
		if (to_int(getCoin()->getCString()) >= 500 && !_isControlled && _delegate->_isHardCoreGame)
		{
			if (getGearArray()->count() == 0)
			{
				setGear(gear06);
			}
			else if (getGearArray()->count() == 1)
			{
				setGear(gear04);
			}
			else if (getGearArray()->count() == 2)
			{
				setGear(gear08);
			}
		}

		if (checkRetri())
		{
			if (_mainTarget != nullptr)
			{
				if (stepBack2())
					return;
			}
			else
			{
				if (stepBack())
					return;
			}
		}

		if (isBaseDanger && checkBase() && !_isControlled)
		{
			bool needBack = false;
			if (strcmp(Akatsuki, getGroup()->getCString()) == 0)
			{
				if (getPositionX() < 85 * 32)
					needBack = true;
			}
			else
			{
				if (getPositionX() > 11 * 32)
					needBack = true;
			}
			if (needBack)
			{
				if (stepBack2())
					return;
			}
		}

		bool isFound1 = false;
		bool isFound2 = false;
		bool isFound3 = false;

		if (getMonsterArray() && getMonsterArray()->count() > 0)
		{
			CCObject *pObject;
			CCARRAY_FOREACH(getMonsterArray(), pObject)
			{
				Monster *mo = (Monster *)pObject;
				if (strcmp(mo->getCharacter()->getCString(), "Saso") == 0)
				{
					isFound3 = true;
				}
				if (strcmp(mo->getCharacter()->getCString(), "Sanshouuo") == 0)
				{
					isFound2 = true;
				}
				if (strcmp(mo->getCharacter()->getCString(), "Karasu") == 0)
				{
					isFound1 = true;
				}
			}
		}

		if (_mainTarget && strcmp(_mainTarget->getRole()->getCString(), ROLE_FLOG) != 0)
		{
			CCPoint moveDirection;
			CCPoint sp;

			if (_mainTarget->_originY)
			{
				sp = ccpSub(ccp(_mainTarget->getPositionX(), _mainTarget->_originY), getPosition());
			}
			else
			{
				sp = ccpSub(_mainTarget->getPosition(), getPosition());
			}
			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::NATTACK)
			{
				if (_isCanSkill3)
				{
					changeSide(sp);
					attack(SKILL3);
					return;
				}
				else if (_isCanSkill1 && !isFound1 && !_isControlled)
				{
					attack(SKILL1);
					return;
				}
				else if (_isCanOugis1 && !_isControlled && !isFound2)
				{
					changeSide(sp);
					attack(OUGIS1);
					return;
				}
				else if (_isCanOugis2 && !_isControlled && _delegate->_isOugis2Game && !isFound3)
				{
					changeSide(sp);
					attack(OUGIS2);
					return;
				}
				else if (enemyCombatPoint > friendCombatPoint && abs(enemyCombatPoint - friendCombatPoint) > 5000 && !_isHealling && !_isControlled)
				{
					if (abs(sp.x) < 160)
					{
						stepBack2();
						return;
					}
					else
					{
						idle();
						return;
					}
				}
				else if (abs(sp.x) < 128)
				{
					if (abs(sp.x) > 32 || abs(sp.y) > 32)
					{
						moveDirection = ccpNormalize(sp);
						walk(moveDirection);
						return;
					}
					else
					{
						changeSide(sp);
						attack(NAttack);
					}

					return;
				}
			}
		}
		_mainTarget = nullptr;
		if (!findEnemy2(ROLE_FLOG))
		{
			findEnemy2("Tower");
		}

		if (_mainTarget)
		{
			CCPoint moveDirection;
			CCPoint sp;

			if (_mainTarget->_originY)
			{
				sp = ccpSub(ccp(_mainTarget->getPositionX(), _mainTarget->_originY), getPosition());
			}
			else
			{
				sp = ccpSub(_mainTarget->getPosition(), getPosition());
			}

			if (abs(sp.x) > 32 || abs(sp.y) > 32)
			{
				moveDirection = ccpNormalize(sp);
				walk(moveDirection);
				return;
			}

			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::NATTACK)
			{
				if (_isCanOugis1 && !_isControlled && !isFound2)
				{
					changeSide(sp);
					attack(OUGIS1);
				}
				else if (_isCanOugis2 && !_isControlled && _delegate->_isOugis2Game && !isFound3)
				{
					changeSide(sp);
					attack(OUGIS2);
				}
				else if (_isCanSkill1 && !isFound1)
				{
					attack(SKILL1);
				}
				else
				{
					changeSide(sp);
					attack(NAttack);
				}
			}
			return;
		}

		if (_isHealling && getHpPercent() < 1)
		{
			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::NATTACK)
			{
				idle();
			}
		}
		else
		{
			stepOn();
		}
	}

	void changeAction()
	{
		if (is_player)
		{
			_delegate->getHudLayer()->skill1Button->setLock();
			_delegate->getHudLayer()->skill2Button->unLock();
		}
	}

	void setActionResume()
	{
		if (is_player)
		{
			_delegate->getHudLayer()->skill1Button->unLock();
			_delegate->getHudLayer()->skill2Button->setLock();
		}
		_skillChangeBuffValue = 0;
	}

	Hero *createClone(int32_t cloneTime)
	{
		Hero *clone = nullptr;

		if (!_monsterArray)
		{
			_monsterArray = CCArray::create();
			_monsterArray->retain();
		}

		if (cloneTime == 0)
		{
			clone = create<Karasu>(CCString::create("Karasu"), CCString::create(ROLE_KUGUTSU), getGroup());
		}
		else if (cloneTime == 1)
		{
			clone = create<Sanshouuo>(CCString::create("Sanshouuo"), CCString::create(ROLE_KUGUTSU), getGroup());
			if (is_player)
			{
				if (_delegate->getHudLayer()->skill4Button)
				{
					_delegate->getHudLayer()->skill4Button->setLock();
				}
			}
		}
		else if (cloneTime == 2)
		{
			clone = create<Saso>(CCString::create("Saso"), CCString::create(ROLE_KUGUTSU), getGroup());
			if (is_player)
			{
				if (_delegate->getHudLayer()->skill5Button)
				{
					_delegate->getHudLayer()->skill5Button->setLock();
				}
			}
		}

		clone->_isArmored = true;
		_monsterArray->addObject(clone);
		return clone;
	}
};
