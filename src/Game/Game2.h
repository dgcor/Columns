#pragma once

#include "Game/Game.h"

class Game2 : public Game
{
public:
	Game2(bool reset_ = false);

	bool getGameProperty(const std::string_view prop1, const std::string_view prop2, Variable& var) const override;
};
