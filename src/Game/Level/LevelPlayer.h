#pragma once

#include "Game/Variable.h"
#include "LevelOptions.h"

struct LevelPlayer
{
	int32_t levelStart{ 0 };
	int32_t levelUp{ 0 };
	int64_t score{ 0 };
	int32_t scoreRound{ 0 };
	int32_t jewels{ 0 };
	int32_t jewelsDestroyed{ 0 };
	int32_t jewelsPoints{ 0 };
	int32_t magicJewelsUsed{ 0 };
	LevelOptions options;

	int32_t getLevel() const noexcept { return levelStart + levelUp; }

	bool getProperty(const std::string_view prop, Variable& var) const;
};
