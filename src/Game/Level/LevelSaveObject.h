#pragma once

#include "LevelPlayer.h"
#include "LevelStateObject.h"

struct LevelSaveObject : public LevelStateObject
{
	std::vector<std::shared_ptr<Jewel>> boardJewels;
	std::vector<LevelPlayer> players;
	uint32_t playerIdx{ 0 };
	bool hasFlash{ false };
};
