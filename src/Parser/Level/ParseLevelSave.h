#pragma once

#include "Game/Level/LevelSaveObject.h"
#include "Json/JsonParser.h"

class Level;

namespace Parser
{
	LevelSaveObject parseLevelSaveObj(const Level& level, const rapidjson::Value& elem);
}
