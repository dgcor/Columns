#pragma once

#include "Game/Action.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "Game/Utils/GameUtils.h"

class ActLevelAddHighScore : public Action
{
private:
	std::string id;
	LevelScore levelScore;

public:
	ActLevelAddHighScore(const std::string_view id_, const std::string_view name,
		int32_t level, int64_t score, int32_t jewels, sf::Time time) : id(id_)
	{
		levelScore.name = name;
		levelScore.level = level;
		levelScore.score = score;
		levelScore.jewels = jewels;
		levelScore.time = time;
	}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->HighScores().add(levelScore);
		}
		return true;
	}
};

class ActLevelLoadGame : public Action
{
private:
	std::string id;
	std::string file;

public:
	ActLevelLoadGame(const std::string_view id_, const std::string_view file_)
		: id(id_), file(file_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->loadGame(game, GameUtils::replaceStringWithVarOrProp(file, game));
		}
		return true;
	}
};

class ActLevelMoveJewels : public Action
{
private:
	std::string id;
	PairInt16 offset;
	bool addDropPoints;

public:
	ActLevelMoveJewels(const std::string_view id_, PairInt16 offset_, bool addDropPoints_)
		: id(id_), offset(offset_), addDropPoints(addDropPoints_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->moveSelectedJewels(game, offset, addDropPoints);
		}
		return true;
	}
};

class ActLevelNewGame : public Action
{
private:
	std::string id;
	std::string gameType;
	sf::Time timeLimit;
	int16_t numPlayers;
	int16_t height;
	int32_t initialLevel;

public:
	ActLevelNewGame(const std::string_view id_, const std::string_view gameType_,
		sf::Time timeLimit_, int16_t numPlayers_, int16_t height_, int32_t initialLevel_)
		: id(id_), gameType(gameType_), timeLimit(timeLimit_), numPlayers(numPlayers_),
		height(height_), initialLevel(initialLevel_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->newGame(gameType, timeLimit, numPlayers, height, initialLevel);
		}
		return true;
	}
};

class ActLevelPause : public Action
{
private:
	std::string id;
	bool pause;

public:
	ActLevelPause(const std::string_view id_, bool pause_) : id(id_), pause(pause_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->Pause(pause);
		}
		return true;
	}
};

class ActLevelRotateJewels : public Action
{
private:
	std::string id;
	bool reverse;

public:
	ActLevelRotateJewels(const std::string_view id_, bool reverse_)
		: id(id_), reverse(reverse_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->rotateSelectedJewels(game, reverse);
		}
		return true;
	}
};

class ActLevelSaveGame : public Action
{
private:
	std::string id;
	std::string file;

public:
	ActLevelSaveGame(const std::string_view id_, const std::string_view file_)
		: id(id_), file(file_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->saveGame(GameUtils::replaceStringWithVarOrProp(file, game));
		}
		return true;
	}
};

class ActLevelSetShader : public Action
{
private:
	std::string id;
	std::string idShader;

public:
	ActLevelSetShader(const std::string_view id_, const std::string_view idShader_)
		: id(id_), idShader(idShader_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			auto shader = game.Resources().Shaders().get(idShader);
			level->setShader(shader);
		}
		return true;
	}
};
