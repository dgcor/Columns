#pragma once

#include "Game/Jewel/Jewels.h"
#include "LevelState.h"

struct LevelStateObject
{
	LevelState state{ LevelState::GameOver };

	bool flashGame{ false };
	sf::Time timer;
	sf::Time timeLimit;

	int32_t chain{ 0 };

	Jewels currentJewels;
	Jewels nextJewels;

	bool isTimeTrial() const noexcept { return timeLimit > sf::Time::Zero; }

	bool isTimeOver() const noexcept { return isTimeTrial() && timer >= timeLimit; }

	sf::Time getTimeLeft() const noexcept
	{
		if (isTimeTrial() == true && timer < timeLimit)
		{
			return timeLimit - timer;
		}
		return sf::Time::Zero;
	}
};
