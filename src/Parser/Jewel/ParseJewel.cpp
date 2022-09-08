#include "ParseJewel.h"
#include "Game/Game.h"
#include "Game/Jewel/Jewel.h"
#include "Game/Level/Level.h"
#include "Game/Utils/GameUtils.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseJewel(Game& game, const Value& elem)
	{
		if (isValidString(elem, "class") == false)
		{
			return;
		}

		auto level = game.Resources().getLevel<Level>(getStringViewKey(elem, "level"));
		if (level == nullptr)
		{
			return;
		}

		auto class_ = level->getJewelClass(elem["class"sv].GetStringView());
		if (class_ == nullptr)
		{
			return;
		}

		auto boardPos = getVector2NumberKey<PairFloat, int16_t>(elem, "boardPosition");
		if (level->Board().isCoordValid(boardPos) == false ||
			level->Board().hasJewel(boardPos) == true)
		{
			return;
		}

		auto jewel = std::make_shared<Jewel>(class_, boardPos);

		jewel->Points((int16_t)getIntKey(elem, "points", class_->Points()));

		level->addJewel(jewel);
	}
}
