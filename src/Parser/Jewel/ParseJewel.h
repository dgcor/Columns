#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseJewel(Game& game, const rapidjson::Value& elem);
}
