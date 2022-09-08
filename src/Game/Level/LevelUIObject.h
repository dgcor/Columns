#pragma once

#include "Game/Variable.h"
#include <memory>
#include <string_view>

class Action;
class Level;

class LevelUIObject
{
public:
	static std::shared_ptr<Action> getAction(const Level& level, uint16_t nameHash16) noexcept;

	static bool setAction(Level& level, uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept;

	static bool getProperty(const Level& level, const std::string_view prop, Variable& var);
};
