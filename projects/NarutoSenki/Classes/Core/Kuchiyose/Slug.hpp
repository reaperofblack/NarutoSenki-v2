#pragma once
#include "Hero.hpp"

class Slug : public Hero
{
	void perform()
	{
		if (!findEnemy(ROLE_FLOG, 0))
		{
			if (!findEnemy("Tower", 0))
			{
				_mainTarget = nullptr;
			}
		}

		if (_mainTarget)
		{
			CCPoint moveDirection;
			CCPoint sp = ccpSub(_mainTarget->getPosition(), getPosition());

			if (strcmp(_mainTarget->getRole()->getCString(), "Tower") == 0 ||
				strcmp(_mainTarget->getRole()->getCString(), ROLE_FLOG) == 0)
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
						if (_isCanSkill1)
						{
							attack(SKILL1);
							scheduleOnce(schedule_selector(CharacterBase::enableSkill1), _sattackcoldDown1);
						}
						else if (_isCanSkill2)
						{
							attack(SKILL2);
							scheduleOnce(schedule_selector(CharacterBase::enableSkill2), _sattackcoldDown2);
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

		stepOn();
	}
};
