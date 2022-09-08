#include "ParseLevelActions.h"
#include "Game/Actions/ActLevel.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser2::Actions
{
	using namespace Parser;
	using namespace rapidjson;

	std::shared_ptr<Action> parseLevelAddHighScore(const Value& elem)
	{
		return std::make_shared<ActLevelAddHighScore>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "scoreName"),
			getIntKey(elem, "scoreLevel"),
			getInt64Key(elem, "score"),
			getIntKey(elem, "jewels"),
			getTimeKey(elem, "time"));
	}

	std::shared_ptr<Action> parseLevelLoadGame(const Value& elem)
	{
		return std::make_shared<ActLevelLoadGame>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "file"));
	}

	std::shared_ptr<Action> parseLevelMoveJewels(const Value& elem)
	{
		return std::make_shared<ActLevelMoveJewels>(
			getStringViewKey(elem, "level"),
			getVector2iKey<PairInt16>(elem, "offset"),
			getBoolKey(elem, "addDropPoints", true));
	}

	std::shared_ptr<Action> parseLevelNewGame(const Value& elem)
	{
		return std::make_shared<ActLevelNewGame>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "type"),
			getTimeKey(elem, "timeLimit"),
			(int16_t)getIntKey(elem, "players"),
			(int16_t)getIntKey(elem, "height"),
			getIntKey(elem, "initialLevel"));
	}

	std::shared_ptr<Action> parseLevelPause(const Value& elem)
	{
		return std::make_shared<ActLevelPause>(
			getStringViewKey(elem, "level"),
			getBoolKey(elem, "pause", true));
	}

	std::shared_ptr<Action> parseLevelRotateJewels(const Value& elem)
	{
		return std::make_shared<ActLevelRotateJewels>(
			getStringViewKey(elem, "level"),
			getBoolKey(elem, "reverse"));
	}

	std::shared_ptr<Action> parseLevelSaveGame(const Value& elem)
	{
		return std::make_shared<ActLevelSaveGame>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "file"));
	}

	std::shared_ptr<Action> parseLevelSetShader(const Value& elem)
	{
		return std::make_shared<ActLevelSetShader>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "shader"));
	}
}
