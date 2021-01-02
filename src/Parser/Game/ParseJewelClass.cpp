#include "ParseJewelClass.h"
#include "Game.h"
#include "Game/JewelClass.h"
#include "Game/Level.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseJewelClass(Game& game, const Value& elem)
	{
		auto level = game.Resources().getLevel(getStringViewKey(elem, "level"));
		if (level == nullptr)
		{
			return;
		}
		if (isValidString(elem, "id") == false)
		{
			return;
		}

		auto id = elem["id"sv].GetStringView();

		if (isValidId(id) == false ||
			level->getJewelClass(id) != nullptr)
		{
			return;
		}

		auto texturePack = game.Resources().getTexturePack(getStringViewKey(elem, "texturePack"));
		auto texturePackDestroy = game.Resources().getTexturePack(getStringViewKey(elem, "texturePackDestroy"));
		if (texturePack == nullptr ||
			texturePackDestroy == nullptr)
		{
			return;
		}

		auto jewelClass = std::make_shared<JewelClass>(texturePack, texturePackDestroy);

		jewelClass->Id(id);
		jewelClass->Type(getStringViewKey(elem, "type"));
		jewelClass->Points((int16_t)getIntKey(elem, "points"));

		level->addJewelClass(id, jewelClass);
	}
}
