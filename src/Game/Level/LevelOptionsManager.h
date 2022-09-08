#pragma once

#include "LevelOptions.h"

class LevelOptionsManager
{
private:
	std::vector<LevelOptions> options;

public:
	LevelOptions get(int32_t level) const;

	void set(std::vector<LevelOptions>& options_);
};
