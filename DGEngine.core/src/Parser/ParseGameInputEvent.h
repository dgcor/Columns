#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseGameInputEvent(Game& game, const rapidjson::Value& elem);
}
