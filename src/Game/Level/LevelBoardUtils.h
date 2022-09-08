#pragma once

#include <functional>
#include "Utils/PairXY.h"
#include <vector>

class Jewel;
class Level;

class LevelBoardUtils
{
public:
	static std::vector<PairFloat> getHintPositions(const Level& level);

	static void checkBoard(const Level& level, int16_t startX, int16_t startY,
		int16_t stopX, int16_t stopY, const std::function<bool(Jewel&)> func = {});
};
