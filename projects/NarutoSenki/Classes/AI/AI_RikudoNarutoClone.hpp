#pragma once
#include "Characters.h"

class AI_RikudoNarutoClone : public Hero
{
	void perform();
};

void AI_RikudoNarutoClone::perform()
{

	if (!this->findEnemy("Hero", 0))
	{
		if (!this->findEnemy("Flog", 0))
		{
			if (!this->findEnemy("Tower", 0))
			{
				_mainTarget = NULL;
			}
		}
	}

	if (_mainTarget)
	{
		CCPoint moveDirection;
		CCPoint sp;
		if (_mainTarget->_originY)
		{
			sp = ccpSub(ccp(_mainTarget->getPositionX(), _mainTarget->_originY), this->getPosition());
		}
		else
		{
			sp = ccpSub(_mainTarget->getPosition(), this->getPosition());
		}

		if (strcmp(_mainTarget->getRole()->getCString(), "Tower") == 0 ||
			strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0)
		{

			if (abs(sp.x) > 32 || abs(sp.y) > 32)
			{

				moveDirection = ccpNormalize(sp);
				this->walk(moveDirection);
			}
			else
			{
				if (this->getActionState() == ACTION_STATE_IDLE || this->getActionState() == ACTION_STATE_WALK || this->getActionState() == ACTION_STATE_ATTACK)
				{

					this->changeSide(sp);
					this->attack(NAttack);
				}
			}
			return;
		}
		else
		{

			if (abs(sp.x) > 128 || abs(sp.y) > 16)
			{

				moveDirection = ccpNormalize(sp);
				this->walk(moveDirection);
				return;
			}
			else if ((abs(sp.x) > 48 || abs(sp.y) > 8) && !_isCanSkill1)
			{

				moveDirection = ccpNormalize(sp);
				this->walk(moveDirection);
				return;
			}
			else if (this->getActionState() == ACTION_STATE_IDLE || this->getActionState() == ACTION_STATE_WALK || this->getActionState() == ACTION_STATE_ATTACK)
			{

				if (_isCanSkill1 && _mainTarget->getGP() < 5000)
				{
					this->changeSide(sp);
					this->attack(SKILL1);
				}
				else
				{
					this->changeSide(sp);
					this->attack(NAttack);
				}
			}
			return;
		}
	}

	this->stepOn();
}
