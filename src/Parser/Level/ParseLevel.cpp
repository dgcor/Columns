#include "ParseLevel.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "ParseLevelOptions.h"
#include "ParseLevelScores.h"
#include "Parser/Drawables/ParseDrawable.h"
#include "Parser/ParseAction.h"
#include "Parser/Utils/ParseUtils.h"
#include "Utils/StringHash.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevel(Game& game, const Value& elem)
	{
		auto id = getStringViewKey(elem, "id");
		auto level = game.Resources().getLevel<Level>(id);
		if (level == nullptr)
		{
			if (isValidId(id) == false)
			{
				return;
			}

			auto tiles = game.Resources().getTexturePack(getStringViewKey(elem, "tilesTexturePack"));
			bool initLevel = getBoolKey(elem, "init");
			if (initLevel == false &&
				tiles == nullptr)
			{
				return;
			}
			auto select = game.Resources().getTexturePack(getStringViewKey(elem, "selectTexturePack"));
			auto highlight = game.Resources().getTexturePack(getStringViewKey(elem, "highlightTexturePack"));
			auto boardSize = getVector2iKey<PairInt16>(elem, "boardSize");
			auto tileSize = (float)getUIntKey(elem, "tileSize");

			level = std::make_shared<Level>(game, tiles, select, highlight, boardSize, tileSize);

			game.Resources().addDrawable(id, level, true, getStringViewKey(elem, "resource"));
			game.Resources().setCurrentLevel(level);
			level->Id(id);
			level->setShader(game.Shaders().Level);

			parseDrawableProperties(game, elem, *level);
		}

		if (elem.HasMember("name"sv) == true)
		{
			level->Name(getStringViewVal(elem["name"sv]));
		}
		if (elem.HasMember("pause"sv) == true)
		{
			level->Pause(getBoolVal(elem["pause"sv]));
		}
		if (elem.HasMember("visible"sv) == true)
		{
			level->Visible(getBoolVal(elem["visible"sv]));
		}
		if (elem.HasMember("startPosition"sv) == true)
		{
			level->setPlayer1StartPosition((int16_t)getIntVal(elem["startPosition"sv]));
		}
		if (elem.HasMember("startPosition2"sv) == true)
		{
			level->setPlayer2StartPosition((int16_t)getIntVal(elem["startPosition2"sv]));
		}
		if (elem.HasMember("flashPosition"sv) == true)
		{
			level->setFlashPosition((int16_t)getIntVal(elem["flashPosition"sv]));
		}
		if (elem.HasMember("scoreFormula"sv) == true)
		{
			level->setScoreFormula(getStringViewVal(elem["scoreFormula"sv]));
		}
		if (elem.HasMember("levelUpFormula"sv) == true)
		{
			level->setLevelUpFormula(getStringViewVal(elem["levelUpFormula"sv]));
		}
		if (elem.HasMember("magicJewel"sv) == true)
		{
			level->setMagicJewel(getStringViewVal(elem["magicJewel"sv]));
		}
		if (isValidArray(elem, "levels") == true)
		{
			parseLevelOptions(level->Options(), elem["levels"sv]);
		}
		if (isValidArray(elem, "highScores") == true)
		{
			parseLevelScores(level->HighScores(), elem["highScores"sv]);
		}

		if (elem.HasMember("onLevelUp"sv))
		{
			level->setAction(
				str2int16("levelUp"),
				getActionVal(game, elem["onLevelUp"sv])
			);
		}
		if (elem.HasMember("onRotate"sv))
		{
			level->setAction(
				str2int16("rotate"),
				getActionVal(game, elem["onRotate"sv])
			);
		}
		if (elem.HasMember("onTouchBase"sv))
		{
			level->setAction(
				str2int16("touchBase"),
				getActionVal(game, elem["onTouchBase"sv])
			);
		}
		if (elem.HasMember("onDestroy"sv))
		{
			level->setAction(
				str2int16("destroy"),
				getActionVal(game, elem["onDestroy"sv])
			);
		}
		if (elem.HasMember("onGameOver"sv))
		{
			level->setAction(
				str2int16("gameOver"),
				getActionVal(game, elem["onGameOver"sv])
			);
		}
	}
}
