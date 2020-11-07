#pragma once
#include "Characters.h"

class AI_Akamaru : public Hero
{
	void perform()
	{

		if (!findEnemy("Hero", winSize.width / 2 - 32, true))
		{
			if (!findEnemy("Flog", winSize.width / 2 - 32, true))
			{
				if (!findEnemy("Tower", winSize.width / 2 - 32, true))
				{
					_mainTarget = NULL;
				}
			}
		}

		CCPoint moveDirection;

		if (abs(ccpSub(_master->getPosition(), getPosition()).x) > winSize.width / 2 - 48)
		{
			if (getActionState() == State::IDLE || getActionState() == State::WALK || getActionState() == State::ATTACK)
			{
				moveDirection = ccpNormalize(ccpSub(_master->getPosition(), getPosition()));
				walk(moveDirection);
				return;
			}
		}

		if (_mainTarget)
		{

			CCPoint sp;
			if (_mainTarget->_originY)
			{
				sp = ccpSub(ccp(_mainTarget->getPositionX(), _mainTarget->_originY), getPosition());
			}
			else
			{
				sp = ccpSub(_mainTarget->getPosition(), getPosition());
			}

			if (strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0)
			{

				if (abs(sp.x) > 48 || abs(sp.y) > 32)
				{
					moveDirection = ccpNormalize(sp);
					walk(moveDirection);
				}
				else
				{
					if (getActionState() == State::IDLE || getActionState() == State::WALK || _actionState == State::ATTACK)
					{
						changeSide(sp);
						attack(NAttack);
					}
				}
				return;
			}
			else
			{

				if (abs(sp.x) > 48 || abs(sp.y) > 32)
				{

					moveDirection = ccpNormalize(sp);
					walk(moveDirection);
					return;
				}
				else
				{
					if (getActionState() == State::IDLE || getActionState() == State::WALK || getActionState() == State::ATTACK)
					{

						if (_master->_isCanSkill2 && _mainTarget->getGP() < 5000 && (_master->_isControled || _master->_isAI == true) &&
							(_master->getActionState() == State::IDLE || _master->getActionState() == State::WALK || _master->getActionState() == State::ATTACK))
						{
							changeSide(sp);
							if (strcmp(_master->getRole()->getCString(), "Player") != 0)
							{
								_master->attack(SKILL2);
							}
							else if (strcmp(_master->getRole()->getCString(), "Player") == 0)
							{
								_master->attack(SKILL2);
							}
						}
						else
						{
							changeSide(sp);
							attack(NAttack);
						}
					}
				}

				return;
			}
		}

		if (abs(ccpSub(_master->getPosition(), getPosition()).x) > winSize.width / 2 - 64)
		{

			CCPoint moveDirection = ccpNormalize(ccpSub(_master->getPosition(), getPosition()));
			walk(moveDirection);
			return;
		}
		else
		{

			stepOn();
		}
	}
};
