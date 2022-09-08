#pragma once

#include "Json/JsonParser.h"

class LevelOptionsManager;

namespace Parser
{
	void parseLevelOptions(LevelOptionsManager& optionsManager, const rapidjson::Value& elem);
}
