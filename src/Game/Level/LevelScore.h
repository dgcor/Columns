#pragma once

#include <cstdint>
#include <SFML/System/Time.hpp>
#include <string>

struct LevelScore
{
	std::string name;
	int32_t level{ 0 };
	int64_t score{ 0 };
	int32_t jewels{ 0 };
	sf::Time time;
};
