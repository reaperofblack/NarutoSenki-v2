#pragma once
#include "Characters.h"

class AI_Tenten : public Hero
{
	void perform();
};

void AI_Tenten::perform()
{

	_mainTarget = NULL;
	this->findEnemy2("Hero");
	if (atoi(this->getCoin()->getCString()) >= 500 && !_isControled && _delegate->_isHardCoreGame)
	{
		if (this->getGearArray()->count() == 0)
		{
			this->setGear(gear00);
		}
		else if (this->getGearArray()->count() == 1)
		{
			this->setGear(gear05);
		}
		else if (this->getGearArray()->count() == 2)
		{
			this->setGear(gear02);
		}
	}

	if (this->checkRetri())
	{
		if (_mainTarget != NULL)
		{
			if (this->stepBack2())
			{
				return;
			}
		}
		else
		{
			if (_isCanGear00)
			{
				this->useGear(gear00);
			}
			if (this->stepBack())
			{
				return;
			}
		}
	}

	if (isBaseDanger && this->checkBase() && !_isControled)
	{
		bool needBack = false;
		if (strcmp("Akatsuki", this->getGroup()->getCString()) == 0)
		{
			if (this->getPositionX() < 85 * 32)
			{
				needBack = true;
			}
		}
		else
		{

			if (this->getPositionX() > 11 * 32)
			{
				needBack = true;
			}
		}

		if (needBack)
		{
			if (_isCanGear00)
			{
				this->useGear(gear00);
			}
			if (this->stepBack2())
			{
				return;
			}
		}
	}

	if (_mainTarget && strcmp(_mainTarget->getRole()->getCString(), "Flog") != 0)
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
		if (_actionState == ACTION_STATE_IDLE || _actionState == ACTION_STATE_WALK || _actionState == ACTION_STATE_ATTACK)
		{

			if (_isCanSkill1)
			{
				this->changeSide(sp);
				this->attack(SKILL1);
				return;
			}
			else if (_isCanOugis2 && !_isControled && _delegate->_isOugis2Game && _mainTarget->getGP() < 5000 && !_skillChangeBuffValue)
			{
				if (abs(sp.x) > 48 || abs(sp.y) > 16)
				{
					if (_isCanGear00)
					{
						this->useGear(gear00);
					}
					moveDirection = ccpNormalize(sp);
					this->walk(moveDirection);
					return;
				}
				this->changeSide(sp);

				this->attack(OUGIS2);
				return;
			}
			else if (_isCanOugis1 && !_isControled && !_mainTarget->_isBati && _mainTarget->getGP() < 5000 && !_skillChangeBuffValue)
			{

				if (abs(sp.x) > 48 || abs(sp.y) > 16)
				{
					if (_isCanGear00)
					{
						this->useGear(gear00);
					}
					moveDirection = ccpNormalize(sp);
					this->walk(moveDirection);
					return;
				}
				this->changeSide(sp);
				this->attack(OUGIS1);

				return;
			}
			else if (enemyCombatPoint > friendCombatPoint && abs(enemyCombatPoint - friendCombatPoint) > 3000 && !_isHealling && !_skillChangeBuffValue && !_isControled)
			{
				if (abs(sp.x) < 160)
				{
					this->stepBack2();
					return;
				}
				else
				{

					this->idle();
					return;
				}
			}
			else if (abs(sp.x) < 128)
			{

				if ((abs(sp.x) > 96 || abs(sp.y) > 16) && !_skillChangeBuffValue)
				{
					moveDirection = ccpNormalize(sp);
					this->walk(moveDirection);
					return;
				}

				if (_isCanSkill2 && _mainTarget->getGP() < 5000)
				{
					this->changeSide(sp);
					this->attack(SKILL2);
				}
				else if (_isCanSkill3 && _mainTarget->getGP() < 5000)
				{
					this->changeSide(sp);
					this->attack(SKILL3);
				}
				else
				{

					if ((abs(sp.x) > 32 || abs(sp.y) > 32) && !_skillChangeBuffValue)
					{
						moveDirection = ccpNormalize(sp);
						this->walk(moveDirection);
						return;
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
	}
	_mainTarget = NULL;
	if (!this->findEnemy2("Flog"))
	{
		this->findEnemy2("Tower");
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

		if (abs(sp.x) > 32 || abs(sp.y) > 32)
		{
			moveDirection = ccpNormalize(sp);
			this->walk(moveDirection);
			return;
		}

		if (_actionState == ACTION_STATE_IDLE || _actionState == ACTION_STATE_WALK || _actionState == ACTION_STATE_ATTACK)
		{
			if (strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0 && _isCanSkill1)
			{
				this->changeSide(sp);
				this->attack(SKILL1);
			}
			else if (strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0 && _isCanSkill3)
			{
				this->changeSide(sp);
				this->attack(SKILL3);
			}
			else if (strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0 && _isCanSkill2 && isBaseDanger)
			{
				this->changeSide(sp);
				this->attack(SKILL2);
			}
			else
			{
				this->changeSide(sp);
				this->attack(NAttack);
			}
		}
		return;
	}

	if (_isHealling && this->getHpPercent() < 1)
	{
		if (_actionState == ACTION_STATE_IDLE || _actionState == ACTION_STATE_WALK || _actionState == ACTION_STATE_ATTACK)
		{
			this->idle();
		}
	}
	else
	{
		if (_isCanGear00)
		{
			this->useGear(gear00);
		}
		this->stepOn();
	}
}
