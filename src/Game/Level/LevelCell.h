#pragma once

#include "Game/Jewel/Jewel.h"
#include <memory>

struct LevelCell
{
	Image tile;
	std::shared_ptr<Jewel> jewel;
};
