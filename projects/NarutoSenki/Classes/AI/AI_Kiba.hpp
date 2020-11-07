#pragma once
#include "Characters.h"

class AI_Kiba : public Hero
{
	void perform()
	{

		_mainTarget = NULL;
		findEnemy2("Hero");
		if (atoi(getCoin()->getCString()) >= 500 && !_isControled && _delegate->_isHardCoreGame)
		{
			if (getGearArray()->count() == 0)
			{
				setGear(gear00);
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
			if (_mainTarget != NULL)
			{
				if (stepBack2())
				{
					return;
				}
			}
			else
			{
				if (_isCanGear00)
				{
					useGear(gear00);
				}
				if (stepBack())
				{
					return;
				}
			}
		}

		if (_mainTarget && (battleCondiction >= 0 || _isCanOugis1 || _isCanOugis2))
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

			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{

				if (_isCanOugis2 && !_isControled && _delegate->_isOugis2Game && !_powerUPBuffValue && _mainTarget->getGP() < 5000 && !_mainTarget->_isBati && _mainTarget->getActionState() != State::KOCKDOWN && !_mainTarget->_isSticking)
				{
					if (abs(sp.x) > 32 || abs(sp.y) > 32)
					{
						if (_isCanGear00)
						{
							useGear(gear00);
						}
						moveDirection = ccpNormalize(sp);
						walk(moveDirection);
						return;
					}

					changeSide(sp);

					attack(OUGIS2);
					return;
				}
				else if (_isCanOugis1 && !_isControled && _mainTarget->getGP() < 5000 && !_powerUPBuffValue)
				{
					changeSide(sp);
					attack(OUGIS1);
					return;
				}
				else if (_isCanSkill1 && _isBati)
				{
					attack(SKILL1);
					return;
				}
				else if (enemyCombatPoint > friendCombatPoint && abs(enemyCombatPoint - friendCombatPoint) > 3000 && !_isHealling && !_isControled)
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
						if (_isCanGear00)
						{
							useGear(gear00);
						}
						moveDirection = ccpNormalize(sp);
						walk(moveDirection);
						return;
					}

					if (_isCanSkill3 && !_powerUPBuffValue && !_isBati && getHpPercent() > 0.5)
					{
						changeSide(sp);
						attack(SKILL3);
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
		if (battleCondiction >= 0)
		{
			_mainTarget = NULL;
			if (!findEnemy2("Flog"))
			{
				findEnemy2("Tower");
			}
		}
		else
		{
			_mainTarget = NULL;
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

			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{

				if (_isCanSkill1 && _isBati)
				{
					attack(SKILL1);
				}
				else if (strcmp(_mainTarget->getRole()->getCString(), "Tower") == 0 && _isCanSkill3 && !_powerUPBuffValue && !_isBati)
				{
					changeSide(sp);
					attack(SKILL3);
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
			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{
				idle();
			}
		}
		else
		{
			if (_isCanGear00)
			{
				useGear(gear00);
			}
			stepOn();
		}
	}
};
