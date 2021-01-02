#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseImageContainer(Game& game, const rapidjson::Value& elem);
}
