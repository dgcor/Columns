#include "LevelSave.h"
#include "Game/Utils/FileUtils.h"
#include "Json/JsonParser.h"
#include "Json/JsonUtils.h"
#include "Json/SaveUtils.h"
#include "Level.h"
#include "LevelSaveObject.h"
#include "Parser/Level/ParseLevelSave.h"

using namespace rapidjson;
using namespace SaveUtils;

bool LevelSave::load(Level& level, const std::string_view filePath)
{
	Document doc;
	if (JsonUtils::loadFile(filePath, doc) == true)
	{
		return load(level, Parser::parseLevelSaveObj(level, doc));
	}
	return false;
}

bool LevelSave::load(Level& level, const LevelSaveObject& levelState)
{
	level.gameState = {};

	level.gameState.state = levelState.state;

	if (levelState.flashGame == true &&
		levelState.hasFlash == false)
	{
		level.gameState.state = LevelState::GameOver;
	}

	level.gameState.flashGame = levelState.flashGame;
	level.gameState.timer = levelState.timer;
	level.gameState.timeLimit = levelState.timeLimit;

	level.gameState.chain = levelState.chain;

	level.playerManager.Init(levelState.players, levelState.playerIdx, level.optionsManager);

	for (const auto& jewel : levelState.boardJewels)
	{
		level.addJewel(jewel);
	}

	if (levelState.currentJewels.size() > 0)
	{
		level.gameState.currentJewels.clear();
	}

	for (const auto& jewel : levelState.currentJewels)
	{
		level.gameState.currentJewels.push_back(jewel);
	}

	return true;
}

void LevelSave::save(const Level& level, const std::string_view filePath)
{
	StringBuffer buffer(0, std::numeric_limits<uint16_t>::max());
	PrettyWriter<StringBuffer> writer(buffer);
	writer.SetIndent(' ', 2);

	serialize(level, &writer);

	FileUtils::saveText(filePath, { buffer.GetString(), buffer.GetSize() });
}

void LevelSave::serialize(const Level& level, void* serializeObj)
{
	auto& writer = *((PrettyWriter<StringBuffer>*)serializeObj);

	// root
	writer.StartObject();

	writeInt(writer, "state", (int32_t)level.gameState.state);

	writeBool(writer, "flash", level.gameState.flashGame);
	writeUInt(writer, "timer", level.gameState.timer.asMilliseconds());
	writeUInt(writer, "timeLimit", level.gameState.timeLimit.asMilliseconds());

	writeInt(writer, "chain", level.gameState.chain);

	if (level.playerManager.size() > 1)
	{
		writeInt(writer, "playerIndex", level.playerManager.PlayerIdx());
	}

	// players
	writeKeyStringView(writer, "players");
	writer.StartArray();

	for (const auto& player : level.playerManager)
	{
		// player
		writer.StartObject();

		writeInt(writer, "levelStart", player.levelStart);
		writeInt(writer, "levelUp", player.levelUp);
		writeInt64(writer, "score", player.score);
		writeInt(writer, "scoreRound", player.scoreRound);
		writeInt(writer, "jewels", player.jewels);
		writeInt(writer, "jewelsDestroyed", player.jewelsDestroyed);
		writeInt(writer, "jewelsPoints", player.jewelsPoints);
		writeInt(writer, "magicJewelsUsed", player.magicJewelsUsed);

		// player
		writer.EndObject();
	}

	// players
	writer.EndArray();

	// board
	writeKeyStringView(writer, "board");
	writer.StartArray();

	for (const auto& cell : level.board)
	{
		if (cell.jewel == nullptr)
		{
			continue;
		}
		// jewel
		writer.StartObject();

		writeString(writer, "jewel", cell.jewel->Class()->Id());
		writeVector2f(writer, "position", cell.jewel->BoardPosition());
		if (cell.jewel->Flash() == true)
		{
			writeBool(writer, "flash", true);
		}

		// jewel
		writer.EndObject();
	}

	// board
	writer.EndArray();

	// jewels
	writeKeyStringView(writer, "jewels");
	writer.StartArray();

	for (const auto& jewel : level.CurrentJewels())
	{
		// jewel
		writer.StartObject();

		writeString(writer, "jewel", jewel->Class()->Id());
		writeVector2f(writer, "position", jewel->BoardPosition());

		// jewel
		writer.EndObject();
	}

	// jewels
	writer.EndArray();

	// root
	writer.EndObject();
}
