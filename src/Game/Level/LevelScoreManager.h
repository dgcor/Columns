#pragma once

#include "Game/Variable.h"
#include "LevelScore.h"
#include <string_view>
#include <vector>

class LevelScoreManager
{
private:
	static constexpr size_t MaxSize = 10;

	std::vector<LevelScore> levelScores;

	void updateScores();

public:
	void add(const LevelScore& levelScore);

	void set(std::vector<LevelScore>& levelScores_);

	auto size() const noexcept { return levelScores.size(); }

	bool getProperty(const std::string_view prop, Variable& var) const;
};
