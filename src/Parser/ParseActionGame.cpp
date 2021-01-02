#include "ParseActionGame.h"
#include "Actions/ActLevel.h"
#include "Utils/ParseUtils.h"
#include "Utils/Utils.h"

namespace Parser
{
	using namespace rapidjson;

	std::shared_ptr<Action> parseActionGameElem(Game& game,
		uint16_t nameHash16, const Value& elem)
	{
		switch (nameHash16)
		{
		case str2int16("level.addHighScore"):
		{
			return std::make_shared<ActLevelAddHighScore>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "scoreName"),
				getIntKey(elem, "scoreLevel"),
				getInt64Key(elem, "score"),
				getIntKey(elem, "jewels"),
				getTimeKey(elem, "time"));
		}
		case str2int16("level.moveJewels"):
		{
			return std::make_shared<ActLevelMoveJewels>(
				getStringViewKey(elem, "level"),
				getVector2iKey<PairInt16>(elem, "offset"),
				getBoolKey(elem, "addDropPoints", true));
		}
		case str2int16("level.newGame"):
		{
			return std::make_shared<ActLevelNewGame>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "type"),
				getTimeKey(elem, "timeLimit"),
				(int16_t)getIntKey(elem, "players"),
				(int16_t)getIntKey(elem, "height"),
				getIntKey(elem, "initialLevel"));
		}
		case str2int16("level.pause"):
		{
			return std::make_shared<ActLevelPause>(
				getStringViewKey(elem, "level"),
				getBoolKey(elem, "pause", true));
		}
		case str2int16("level.rotateJewels"):
		{
			return std::make_shared<ActLevelRotateJewels>(
				getStringViewKey(elem, "level"),
				getBoolKey(elem, "reverse"));
		}
		case str2int16("level.save"):
		{
			return std::make_shared<ActLevelSave>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "file"),
				getVariablesMapKey(elem, "properties"));
		}
		case str2int16("level.setShader"):
		{
			return std::make_shared<ActLevelSetShader>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "shader"));
		}
		default:
			return nullptr;
		}
	}
}
