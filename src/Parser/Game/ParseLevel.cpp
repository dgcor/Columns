#include "ParseLevel.h"
#include "Game.h"
#include "Game/Level.h"
#include "GameUtils.h"
#include "Json/JsonUtils.h"
#include "Parser/ParseAction.h"
#include "Parser/Utils/ParseUtils.h"
#include "Utils/Utils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevelOptions(Level& level, const Value& elem)
	{
		LevelOptions options;
		std::vector<LevelOptions> allOptions;
		for (const auto& val : elem)
		{
			options.level = getIntKey(val, "level", options.level + 1);
			options.levelMax = getIntKey(val, "levelMax", options.levelMax);
			options.scoreMin = getInt64Key(val, "scoreMin", options.scoreMin);
			options.scoreMax = getInt64Key(val, "scoreMax", options.scoreMax);
			options.jewelsMax = getIntKey(val, "jewelsMax", options.jewelsMax);
			options.dropSteps = getUIntKey(val, "dropSteps", options.dropSteps);
			options.dropPoints = (int16_t)getIntKey(val, "dropPoints", options.dropPoints);
			options.magicJewels += (int16_t)getIntKey(val, "magicJewels");
			options.magicHeight = (int16_t)getIntKey(val, "magicHeight", options.magicHeight);
			options.hints = getBoolKey(val, "hints", options.hints);
			options.dropSpeed = getTimeKey(val, "dropSpeed", options.dropSpeed);
			options.timeoutSpeed = getTimeKey(val, "timeoutSpeed", options.timeoutSpeed);
			options.updateScoreSpeed = getTimeKey(val, "updateScoreSpeed", options.updateScoreSpeed);

			if (isValidArray(val, "jewels") == true)
			{
				options.jewels.clear();
				for (const auto& val2 : val["jewels"sv])
				{
					auto id = getStringVal(val2);
					if (id.empty() == true)
					{
						continue;
					}
					options.jewels.push_back(id);
				}
			}
			allOptions.push_back(options);
		}
		level.setOptions(allOptions);
	}

	void parseLevelHighScores(Level& level, const Value& elem)
	{
		LevelScore score;
		std::vector<LevelScore> scores;
		for (const auto& val : elem)
		{
			score.name = getStringKey(val, "name");
			score.level = getIntKey(val, "level");
			score.score = getInt64Key(val, "score");
			score.jewels = getIntKey(val, "jewels");
			score.time = getTimeKey(val, "time");
			scores.push_back(score);
		}
		level.setHighScores(scores);
	}

	void parseLevel(Game& game, const Value& elem)
	{
		auto id = getStringViewKey(elem, "id");
		auto level = game.Resources().getLevel(id);
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
			auto mapSize = getVector2iKey<PairInt16>(elem, "mapSize");
			auto tileSize = (float)getUIntKey(elem, "tileSize");

			auto levelPtr = std::make_shared<Level>(game, tiles, select, highlight, mapSize, tileSize);

			game.Resources().addDrawable(id, levelPtr, true, getStringViewKey(elem, "resource"));
			level = levelPtr.get();
			game.Resources().setCurrentLevel(level);
			level->Id(id);
			level->setShader(game.Shaders().Level);

			auto anchor = getAnchorKey(elem, "anchor");
			level->setAnchor(anchor);
			auto size = level->Size();
			auto pos = getPositionKey(elem, "position", size, game.RefSize());
			if (getBoolKey(elem, "relativeCoords", true) == true &&
				game.RefSize() != game.DrawRegionSize())
			{
				GameUtils::setAnchorPosSize(anchor, pos, size, game.RefSize(), game.DrawRegionSize());
			}
			level->Position(pos);
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
			parseLevelOptions(*level, elem["levels"sv]);
		}
		if (isValidArray(elem, "highScores") == true)
		{
			parseLevelHighScores(*level, elem["highScores"sv]);
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
