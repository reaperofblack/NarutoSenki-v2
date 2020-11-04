#pragma once
#include "Characters.h"

class AI_Minato : public Hero
{
	void perform()
	{

		_mainTarget = NULL;
		this->findEnemy2("Hero");

		if (_isCanGear06)
		{
			if ((this->getActionState() == State::FLOAT ||
				 this->getActionState() == State::AIRHURT ||
				 this->getActionState() == State::HURT ||
				 this->getActionState() == State::KOCKDOWN) &&
				this->getHpPercent() < 0.5 && !_isBati && !_isWudi)
			{
				this->useGear(gear06);
			}
		}
		if (atoi(this->getCoin()->getCString()) >= 500 && !_isControled && _delegate->_isHardCoreGame)
		{
			if (this->getGearArray()->count() == 0)
			{
				this->setGear(gear06);
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
		if (this->getHpPercent() < 0.3f)
		{

			if (_isCanSkill1)
			{

				CCObject *pObject;
				bool isMark = false;
				if (this->getMonsterArray() && this->getMonsterArray()->count() > 0)
				{
					CCARRAY_FOREACH(this->getMonsterArray(), pObject)
					{
						Monster *mo = (Monster *)pObject;
						if (strcmp(mo->getCharacter()->getCString(), "HiraishinMark") == 0)
						{
							if (strcmp(this->getGroup()->getCString(), Konoha) == 0 && mo->getPositionX() < this->getPositionX())
							{
								isMark = true;
							}
							else if (strcmp(this->getGroup()->getCString(), Akatsuki) == 0 && mo->getPositionX() > this->getPositionX())
							{
								isMark = true;
							}
						}
					}
				}
				if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
				{
					if (isMark)
					{
						this->attack(SKILL1);
						return;
					}
				}
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
				if (this->stepBack())
				{
					return;
				}
			}
		}

		if (isBaseDanger && this->checkBase() && !_isControled)
		{
			bool needBack = false;
			if (strcmp(Akatsuki, this->getGroup()->getCString()) == 0)
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
			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{

				CCObject *pObject;
				bool isMark = false;
				if (this->getMonsterArray() && this->getMonsterArray()->count() > 0)
				{
					CCARRAY_FOREACH(this->getMonsterArray(), pObject)
					{
						Monster *mo = (Monster *)pObject;
						if (strcmp(mo->getCharacter()->getCString(), "HiraishinMark") == 0)
						{
							isMark = true;
						}
					}
				}

				if (_isCanSkill1 && !isMark)
				{
					this->changeSide(sp);
					this->attack(SKILL1);
				}
				else if (_isCanOugis2 && !_isControled && _delegate->_isOugis2Game && _mainTarget->getGP() < 5000)
				{
					if (abs(sp.x) > 64 || abs(sp.y) > 16)
					{
						moveDirection = ccpNormalize(sp);
						this->walk(moveDirection);
						return;
					}
					this->changeSide(sp);

					this->attack(OUGIS2);

					return;
				}
				else if (_isCanOugis1 && !_isControled && _mainTarget->getGP() < 5000)
				{
					if (abs(sp.x) > 64 || abs(sp.y) > 16)
					{
						moveDirection = ccpNormalize(sp);
						this->walk(moveDirection);
						return;
					}
					this->changeSide(sp);
					this->attack(OUGIS1);
					return;
				}
				else if (_isCanSkill2 && _mainTarget->getGP() < 5000)
				{
					if (abs(sp.y) > 16)
					{
						moveDirection = ccpNormalize(sp);
						this->walk(moveDirection);
						return;
					}
					this->changeSide(sp);
					this->attack(SKILL2);
					return;
				}
				else if (enemyCombatPoint > friendCombatPoint && abs(enemyCombatPoint - friendCombatPoint) > 3000 && !_isHealling && !_isControled && !_skillChangeBuffValue)
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
				else if (_isCanSkill3)
				{
					this->changeSide(sp);
					this->attack(SKILL3);
					return;
				}
				else
				{
					if (abs(sp.x) > 352 || abs(sp.y) > 128)
					{
						moveDirection = ccpNormalize(sp);
						this->walk(moveDirection);
						return;
					}
					else if ((abs(sp.x) > 32 || abs(sp.y) > 32) && atoi(this->getnAttackValue()->getCString()) < 260)
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

			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{
				if (_isCanSkill3 && strcmp(_mainTarget->getRole()->getCString(), "Flog") == 0)
				{
					this->changeSide(sp);
					this->attack(SKILL3);
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
			if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
			{
				this->idle();
			}
		}
		else
		{
			if (_isCanSkill1)
			{

				CCObject *pObject;
				bool isMark = false;
				if (this->getMonsterArray() && this->getMonsterArray()->count() > 0)
				{
					CCARRAY_FOREACH(this->getMonsterArray(), pObject)
					{
						Monster *mo = (Monster *)pObject;
						if (strcmp(mo->getCharacter()->getCString(), "HiraishinMark") == 0)
						{

							if (strcmp(this->getGroup()->getCString(), Konoha) == 0 && mo->getPositionX() > this->getPositionX())
							{
								isMark = true;
							}
							else if (strcmp(this->getGroup()->getCString(), Akatsuki) == 0 && mo->getPositionX() < this->getPositionX())
							{
								isMark = true;
							}
						}
					}
				}
				if (_actionState == State::IDLE || _actionState == State::WALK || _actionState == State::ATTACK)
				{
					if (isMark)
					{
						this->attack(SKILL1);
						return;
					}
				}
			}

			this->stepOn();
		}
	}
};
