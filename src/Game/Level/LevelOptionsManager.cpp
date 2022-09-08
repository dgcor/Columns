#include "LevelOptionsManager.h"
#include <algorithm>
#include <limits>
#include "Utils/ReverseIterable.h"

LevelOptions LevelOptionsManager::get(int32_t level) const
{
	for (const auto& opt : reverse(options))
	{
		if (level >= opt.level)
		{
			return opt;
		}
	}
	return {};
}

void LevelOptionsManager::set(std::vector<LevelOptions>& options_)
{
	options = std::move(options_);
	std::sort(options.begin(), options.end(),
		[](const LevelOptions& a, const LevelOptions& b)
		{
			return a.level < b.level;
		}
	);
	for (auto& obj : options)
	{
		obj.levelMax = std::min(obj.levelMax, 0xFFFF);
		if (obj.levelMax <= 0)
		{
			obj.levelMax = 0xFFFF;
		}
		obj.level = std::clamp(obj.level, 0, obj.levelMax);
		obj.jewelsMax = std::min(obj.jewelsMax, std::numeric_limits<int32_t>::max());
		if (obj.scoreMax <= 0)
		{
			obj.scoreMax = std::numeric_limits<int64_t>::max();
		}
		if (obj.scoreMin >= obj.scoreMax)
		{
			obj.scoreMin = std::numeric_limits<int64_t>::min();
		}
		obj.jewelsMax = std::min(obj.jewelsMax, std::numeric_limits<int32_t>::max());
		if (obj.jewelsMax <= 0)
		{
			obj.jewelsMax = std::numeric_limits<int32_t>::max();
		}
		obj.dropSteps = std::clamp(obj.dropSteps, 1u, 24u);
		obj.dropSpeed = sf::milliseconds(
			std::clamp(
				obj.dropSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
		obj.timeoutSpeed = sf::milliseconds(
			std::clamp(
				obj.timeoutSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
		obj.updateScoreSpeed = sf::milliseconds(
			std::clamp(
				obj.updateScoreSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
	}
}
