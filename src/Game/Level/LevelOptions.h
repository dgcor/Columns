#pragma once

#include <SFML/System/Time.hpp>
#include <string>
#include "Utils/UnorderedStringMap.h"
#include <vector>

struct LevelOptions
{
	int32_t level{ 0 };
	int32_t levelMax{ 199 };
	int64_t scoreMin{ 0 };
	int64_t scoreMax{ 99999999 };
	int32_t jewelsMax{ 9999 };
	uint32_t dropSteps{ 2 };
	int16_t dropPoints{ 1 };
	int16_t magicJewels{ 0 };
	int16_t magicHeight{ 0 };
	bool hints{ false };
	sf::Time dropSpeed{ sf::milliseconds(250) };
	sf::Time timeoutSpeed{ sf::milliseconds(320) };
	sf::Time updateScoreSpeed{ sf::milliseconds(400) };
	std::vector<std::string> jewels;
};
