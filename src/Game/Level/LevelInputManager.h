#pragma once

#include <string>
#include "Utils/ElapsedTime.h"

class Game;
class Level;

class LevelInputManager
{
private:
	uint16_t moveDownEventHash{ 0 };
	uint16_t moveLeftEventHash{ 0 };
	uint16_t moveRightEventHash{ 0 };
	uint16_t rotateEventHash{ 0 };
	uint16_t reverseRotateEventHash{ 0 };

	ElapsedTime moveDownTime{ sf::milliseconds(16) };
	ElapsedTime moveLeftTime{ sf::milliseconds(100) };
	ElapsedTime moveRightTime{ sf::milliseconds(100) };

	int16_t leftMoves{ 0 };
	int16_t rightMoves{ 0 };
	bool rotateWasPressed{ false };
	bool reverseRotateWasPressed{ false };

	bool hasActiveInputEvent(Game& game, uint16_t eventHash);

public:
	void Init(const std::string& eventNamePrefix);

	void processInputEvents(Level& level, Game& game);
};
