#pragma once
#include "Hero.hpp"

class Mask : public Hero
{
	void perform()
	{
		if (strcmp(getCharacter()->getCString(), "MaskRaidon") == 0)
		{
			if (!findEnemy(ROLE_FLOG, 0))
			{
				if (!findEnemy("Hero", 0))
				{
					if (!findEnemy("Tower", 0))
					{
						_mainTarget = nullptr;
					}
				}
			}
		}
		else
		{
			if (!findEnemy("Hero", 0))
			{
				if (!findEnemy(ROLE_FLOG, 0))
				{
					if (!findEnemy("Tower", 0))
					{
						_mainTarget = nullptr;
					}
				}
			}
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

			if (strcmp(_mainTarget->getRole()->getCString(), "Tower") == 0)
			{
				if (abs(sp.x) > 32 || abs(sp.y) > 32)
				{
					moveDirection = ccpNormalize(sp);
					walk(moveDirection);
				}
				else
				{
					if (getActionState() == State::IDLE || getActionState() == State::WALK || getActionState() == State::NATTACK)
					{
						changeSide(sp);
						attack(NAttack);
					}
				}
				return;
			}
			else
			{
				if (abs(sp.x) > 96 || abs(sp.y) > 16)
				{
					moveDirection = ccpNormalize(sp);
					walk(moveDirection);
					return;
				}
				else if ((abs(sp.x) > 48 || abs(sp.y) > 16) && !_isCanSkill1)
				{
					moveDirection = ccpNormalize(sp);
					walk(moveDirection);
					return;
				}
				else if (getActionState() == State::IDLE || getActionState() == State::WALK || getActionState() == State::NATTACK)
				{
					if (_isCanSkill1 && _mainTarget->getGP() < 5000)
					{
						changeSide(sp);
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
		}

		stepOn();
	}
};
