#include "ParseAction2.h"
#include "Parser/Actions/ParseLevelActions.h"
#include "Utils/StringHash.h"

namespace Parser2
{
	using namespace rapidjson;

	std::shared_ptr<Action> parseActionElem(Game& game, uint16_t nameHash16, const Value& elem)
	{
		switch (nameHash16)
		{
		case str2int16("level.addHighScore"):
		{
			return Actions::parseLevelAddHighScore(elem);
		}
		case str2int16("level.loadGame"):
		{
			return Actions::parseLevelLoadGame(elem);
		}
		case str2int16("level.moveJewels"):
		{
			return Actions::parseLevelMoveJewels(elem);
		}
		case str2int16("level.newGame"):
		{
			return Actions::parseLevelNewGame(elem);
		}
		case str2int16("level.pause"):
		{
			return Actions::parseLevelPause(elem);
		}
		case str2int16("level.rotateJewels"):
		{
			return Actions::parseLevelRotateJewels(elem);
		}
		case str2int16("level.saveGame"):
		{
			return Actions::parseLevelSaveGame(elem);
		}
		case str2int16("level.setShader"):
		{
			return Actions::parseLevelSetShader(elem);
		}
		default:
			return nullptr;
		}
	}
}
