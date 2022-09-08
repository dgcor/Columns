#pragma once

#include <cstdint>

enum class LevelState : int32_t
{
	GameOver,
	NextRound,
	AddJewels,
	ClearJewels,
	ClearingJewels,
	ClearedJewels
};
