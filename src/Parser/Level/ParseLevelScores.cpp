#include "ParseLevelScores.h"
#include "Game/Level/LevelScoreManager.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;

	void parseLevelScores(LevelScoreManager& scoreManager, const Value& elem)
	{
		LevelScore score;
		std::vector<LevelScore> scores;
		for (const auto& val : elem)
		{
			score.name = getStringKey(val, "name");
			score.level = getIntKey(val, "level");
			score.score = getInt64Key(val, "score");
			score.jewels = getIntKey(val, "jewels");
			score.time = getTimeUKey(val, "time");
			scores.push_back(score);
		}
		scoreManager.set(scores);
	}
}
