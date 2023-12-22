#include "ParseLevelSave.h"
#include "Game/Level/Level.h"
#include "Game/Level/LevelPlayer.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	LevelPlayer parseLevelSavePlayer(const Value& elem)
	{
		LevelPlayer plr;

		plr.levelStart = getIntKey(elem, "levelStart");
		plr.levelUp = getIntKey(elem, "levelUp");
		plr.score = getInt64Key(elem, "score");
		plr.scoreRound = getIntKey(elem, "scoreRound");
		plr.jewels = getIntKey(elem, "jewels");
		plr.jewelsDestroyed = getIntKey(elem, "jewelsDestroyed");
		plr.jewelsPoints = getIntKey(elem, "jewelsPoints");
		plr.magicJewelsUsed = getIntKey(elem, "magicJewelsUsed");

		return plr;
	}

	std::shared_ptr<Jewel> parseLevelSaveJewel(const Level& level, const Value& elem)
	{
		auto jewelClass = level.getJewelClass(getStringKey(elem, "jewel"));
		if (jewelClass != nullptr)
		{
			return std::make_shared<Jewel>(
				jewelClass,
				getVector2fKey<PairFloat>(elem, "position"));
		}
		return nullptr;
	}

	LevelSaveObject parseLevelSaveObj(const Level& level, const Value& elem)
	{
		LevelSaveObject levelSave;

		levelSave.state = (LevelState)getIntKey(elem, "state");
		levelSave.flashGame = getBoolKey(elem, "flash");
		levelSave.timer = getTimeUKey(elem, "timer");
		levelSave.timeLimit = getTimeUKey(elem, "timeLimit");
		levelSave.chain = getUIntKey(elem, "chain");

		levelSave.playerIdx = getUIntKey(elem, "playerIndex");

		if (isValidArray(elem, "players") == true)
		{
			for (const auto& val : elem["players"sv])
			{
				levelSave.players.push_back(parseLevelSavePlayer(val));
			}
		}

		if (isValidArray(elem, "board") == true)
		{
			for (const auto& val : elem["board"sv])
			{
				auto jewel = parseLevelSaveJewel(level, val);
				if (jewel == nullptr)
				{
					continue;
				}
				if (getBoolKey(val, "flash") == true)
				{
					jewel->Flash(true);
					levelSave.hasFlash = true;
				}
				levelSave.boardJewels.push_back(jewel);
			}
		}

		if (isValidArray(elem, "jewels") == true)
		{
			for (const auto& val : elem["jewels"sv])
			{
				auto jewel = parseLevelSaveJewel(level, val);
				if (jewel == nullptr)
				{
					continue;
				}
				levelSave.currentJewels.push_back(jewel);
			}
		}

		return levelSave;
	}
}
