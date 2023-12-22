#include "LevelUIObject.h"
#include "Game/Utils/GameUtils.h"
#include "Game/Utils/UIObjectUtils.h"
#include "Level.h"
#include "Utils/StringHash.h"
#include "Utils/Utils.h"

std::shared_ptr<Action> LevelUIObject::getAction(const Level& level, uint16_t nameHash16) noexcept
{
	switch (nameHash16)
	{
	case str2int16("levelUp"):
		return level.levelUpAction;
	case str2int16("rotate"):
		return level.rotateAction;
	case str2int16("touchBase"):
		return level.touchBaseAction;
	case str2int16("destroy"):
		return level.destroyAction;
	case str2int16("gameOver"):
		return level.gameOverAction;
	default:
		return nullptr;
	}
}

bool LevelUIObject::setAction(Level& level, uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	switch (nameHash16)
	{
	case str2int16("levelUp"):
		level.levelUpAction = action;
		break;
	case str2int16("rotate"):
		level.rotateAction = action;
		break;
	case str2int16("touchBase"):
		level.touchBaseAction = action;
		break;
	case str2int16("destroy"):
		level.destroyAction = action;
		break;
	case str2int16("gameOver"):
		level.gameOverAction = action;
		break;
	default:
		return false;
	}
	return true;
}

bool LevelUIObject::getProperty(const Level& level, const std::string_view prop, Variable& var)
{
	if (prop.size() <= 1)
	{
		return false;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("id"):
		var = Variable(level.levelId);
		return true;
	case str2int16("name"):
		var = Variable(level.name);
		return true;
	case str2int16("boardSize"):
		var = UIObjectUtils::getTuple2iProp(level.board.Size(), prop);
		return true;
	case str2int16("pause"):
		var = Variable(level.pause);
		return true;
	case str2int16("timer"):
		var = Variable(GameUtils::getTime(level.gameState.timer, props.second));
		return true;
	case str2int16("timeLimit"):
		var = Variable(GameUtils::getTime(level.gameState.timeLimit, props.second, true));
		return true;
	case str2int16("timeLeft"):
		var = Variable(GameUtils::getTime(level.gameState.getTimeLeft(), props.second, true));
		return true;
	case str2int16("timeTrial"):
		var = Variable(level.gameState.isTimeTrial());
		return true;
	case str2int16("gameType"):
		var = Variable(level.gameState.flashGame ? "flash" : "original");
		return true;
	case str2int16("gameOver"):
		var = Variable(level.gameState.state == LevelState::GameOver);
		return true;
	case str2int16("level"):
		var = Variable((int64_t)level.Plr().getLevel());
		return true;
	case str2int16("levelStart"):
		var = Variable((int64_t)level.Plr().levelStart);
		return true;
	case str2int16("levelUp"):
		var = Variable((int64_t)level.Plr().levelUp);
		return true;
	case str2int16("score"):
		var = Variable(level.Plr().score);
		return true;
	case str2int16("scoreRound"):
		var = Variable((int64_t)level.Plr().scoreRound);
		return true;
	case str2int16("jewels"):
		var = Variable((int64_t)level.Plr().jewels);
		return true;
	case str2int16("jewelsDestroyed"):
		var = Variable((int64_t)level.Plr().jewelsDestroyed);
		return true;
	case str2int16("jewelsPoints"):
		var = Variable((int64_t)level.Plr().jewelsPoints);
		return true;
	case str2int16("magicJewels"):
		var = Variable((int64_t)level.Plr().options.magicJewels);
		return true;
	case str2int16("magicJewelsUsed"):
		var = Variable((int64_t)level.Plr().magicJewelsUsed);
		return true;
	case str2int16("magicJewelDestroyed"):
		var = Variable(level.magicJewelDestroyed);
		return true;
	case str2int16("height"):
		var = Variable((int64_t)level.height);
		return true;
	case str2int16("chain"):
		var = Variable((int64_t)level.gameState.chain);
		return true;
	case str2int16("hasHighScore"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(idx < level.highScoresManager.size());
		return true;
	}
	case str2int16("players"):
		var = Variable((int64_t)level.playerManager.size());
		return true;
	case str2int16("playerIdx"):
		var = Variable((int64_t)level.playerManager.PlayerIdx());
		return true;
	case str2int16("player1"):
		return level.playerManager.getProperty(0, props.second, var);
	case str2int16("player2"):
		return level.playerManager.getProperty(1, props.second, var);
	case str2int16("highScore"):
		return level.highScoresManager.getProperty(props.second, var);
	default:
		return level.getUIObjProp(propHash, props.second, var);
	}
	return false;
}
