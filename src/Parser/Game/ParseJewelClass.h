#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseJewelClass(Game& game, const rapidjson::Value& elem);
}
