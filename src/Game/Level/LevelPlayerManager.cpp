#include "LevelPlayerManager.h"
#include <algorithm>
#include "LevelOptionsManager.h"

void LevelPlayerManager::Init(int16_t numPlayers, int32_t initialLevel, const LevelOptionsManager& optionsManager)
{
	players.clear();
	playerIdx = 0;

	for (int16_t i = 0; i < numPlayers; i++)
	{
		players.push_back({});
		players[i].levelStart = initialLevel;
		players[i].options = optionsManager.get(players[i].getLevel());
	}
}

void LevelPlayerManager::Init(const std::vector<LevelPlayer>& players_, uint32_t playerIdx_, const LevelOptionsManager& optionsManager)
{
	if (players_.empty() == true)
	{
		Init(1, 0, optionsManager);
		return;
	}

	players.clear();

	auto numPlayers = std::clamp(players_.size(), (size_t)1, players.max_size());

	for (size_t i = 0; i < numPlayers; i++)
	{
		players.push_back(players_[i]);
		players[i].options = optionsManager.get(players[i].getLevel());
	}

	playerIdx = playerIdx_ < numPlayers ? playerIdx_ : 0;
}

bool LevelPlayerManager::getProperty(uint32_t playerIdx_, const std::string_view prop, Variable& var) const
{
	if (playerIdx_ < players.size())
	{
		return players[playerIdx_].getProperty(prop, var);
	}
	return false;
}
