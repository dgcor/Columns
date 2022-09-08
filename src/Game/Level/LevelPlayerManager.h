#pragma once

#include "Game/Variable.h"
#include "LevelPlayer.h"
#include "Utils/FixedArray.h"

class LevelOptionsManager;

class LevelPlayerManager
{
private:
	FixedArray<LevelPlayer, 2> players;
	uint32_t playerIdx{ 0 };
	std::array<int16_t, 2> startPositions;

public:
	constexpr auto begin() noexcept { return players.begin(); }
	constexpr auto end() noexcept { return players.end(); }
	constexpr auto begin() const noexcept { return players.begin(); }
	constexpr auto end() const noexcept { return players.end(); }
	constexpr auto cbegin() const noexcept { return players.cbegin(); }
	constexpr auto cend() const noexcept { return players.cend(); }
	constexpr auto rbegin() noexcept { return players.rbegin(); }
	constexpr auto rend() noexcept { return players.rend(); }
	constexpr auto rbegin() const noexcept { return players.rbegin(); }
	constexpr auto rend() const noexcept { return players.rend(); }
	constexpr auto crbegin() const noexcept { return players.crbegin(); }
	constexpr auto crend() const noexcept { return players.crend(); }

	void Init(int16_t numPlayers, int32_t initialLevel, const LevelOptionsManager& optionsManager);

	void Init(const std::vector<LevelPlayer>& players_, uint32_t playerIdx_, const LevelOptionsManager& optionsManager);

	auto& get(uint32_t playerIdx_) noexcept { return players[playerIdx_]; }
	auto& get(uint32_t playerIdx_) const noexcept { return players[playerIdx_]; }

	auto& Plr() noexcept { return players[playerIdx]; }
	auto& Plr() const noexcept { return players[playerIdx]; }

	auto getStartPosition(uint32_t playerIdx_) const noexcept { return startPositions[playerIdx_]; }

	auto StartPosition() const noexcept { return startPositions[playerIdx]; }

	auto PlayerIdx() const noexcept { return playerIdx; }

	constexpr auto size() const noexcept { return players.size(); }

	void nextPlayer() noexcept { playerIdx = (playerIdx + 1) % players.size(); }

	void setStartPosition(uint32_t playerIdx_, int16_t startPos_) { startPositions[playerIdx_] = startPos_; }

	bool getProperty(uint32_t playerIdx_, const std::string_view prop, Variable& var) const;
};
