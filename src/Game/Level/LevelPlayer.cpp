#include "LevelPlayer.h"
#include "Utils/StringHash.h"

bool LevelPlayer::getProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	switch (str2int16(prop))
	{
	case str2int16("level"):
		var = Variable((int64_t)getLevel());
		return true;
	case str2int16("levelStart"):
		var = Variable((int64_t)levelStart);
		return true;
	case str2int16("levelUp"):
		var = Variable((int64_t)levelUp);
		return true;
	case str2int16("score"):
		var = Variable(score);
		return true;
	case str2int16("scoreRound"):
		var = Variable((int64_t)scoreRound);
		return true;
	case str2int16("jewels"):
		var = Variable((int64_t)jewels);
		return true;
	case str2int16("jewelsDestroyed"):
		var = Variable((int64_t)jewelsDestroyed);
		return true;
	case str2int16("jewelsPoints"):
		var = Variable((int64_t)jewelsPoints);
		return true;
	case str2int16("magicJewels"):
		var = Variable((int64_t)options.magicJewels);
		return true;
	case str2int16("magicJewelsUsed"):
		var = Variable((int64_t)magicJewelsUsed);
		return true;
	default:
		return false;
	}
	return false;
}
