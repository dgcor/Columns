#include "ParseLevel.h"
#include "Game/Level/LevelOptionsManager.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevelOptions(LevelOptionsManager& optionsManager, const Value& elem)
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
		optionsManager.set(allOptions);
	}
}
