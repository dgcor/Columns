#pragma once

#include "Json/JsonParser.h"

class LevelScoreManager;

namespace Parser
{
	void parseLevelScores(LevelScoreManager& scoreManager, const rapidjson::Value& elem);
}
