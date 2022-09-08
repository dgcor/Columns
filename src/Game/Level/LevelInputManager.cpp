#include "LevelInputManager.h"
#include "Game/Game.h"
#include "Level.h"
#include "Utils/StringHash.h"

void LevelInputManager::Init(const std::string& eventNamePrefix)
{
	moveDownEventHash = str2int16(eventNamePrefix + ".down");
	moveLeftEventHash = str2int16(eventNamePrefix + ".left");
	moveRightEventHash = str2int16(eventNamePrefix + ".right");
	rotateEventHash = str2int16(eventNamePrefix + ".rotate");
	reverseRotateEventHash = str2int16(eventNamePrefix + ".reverseRotate");
}

bool LevelInputManager::hasActiveInputEvent(Game& game, uint16_t eventHash)
{
	return game.hasFocus() &&
		game.Resources().hasActiveInputEvents({ eventHash });
}

void LevelInputManager::processInputEvents(Level& level, Game& game)
{
	moveDownTime.update(game.getElapsedTime(), [&]()
		{
			if (hasActiveInputEvent(game, moveDownEventHash) == true)
			{
				level.moveSelectedJewels(game, { 0, 1 }, true);
			}
		});

	auto moveLeftPressed = hasActiveInputEvent(game, moveLeftEventHash);

	if (moveLeftPressed == true &&
		leftMoves == 0)
	{
		moveLeftTime.currentTime = moveLeftTime.timeout;
	}

	moveLeftTime.update(game.getElapsedTime(), [&]()
		{
			if (moveLeftPressed == true)
			{
				level.moveSelectedJewels(game, { -1, 0 }, false);
				leftMoves++;
				switch (leftMoves)
				{
				case 0:
				case 1:
					moveLeftTime.timeout = sf::milliseconds(100);
					break;
				case 2:
					moveLeftTime.timeout = sf::milliseconds(50);
					break;
				case 3:
					moveLeftTime.timeout = sf::milliseconds(20);
					break;
				default:
					break;
				}
				moveLeftTime.currentTime = sf::milliseconds(0);
			}
			else
			{
				leftMoves = 0;
			}
		});

	auto moveRightPressed = hasActiveInputEvent(game, moveRightEventHash);

	if (moveRightPressed == true &&
		rightMoves == 0)
	{
		moveRightTime.currentTime = moveRightTime.timeout;
	}

	moveRightTime.update(game.getElapsedTime(), [&]()
		{
			if (moveRightPressed == true)
			{
				level.moveSelectedJewels(game, { 1, 0 }, false);
				rightMoves++;
				switch (rightMoves)
				{
				case 0:
				case 1:
					moveRightTime.timeout = sf::milliseconds(100);
					break;
				case 2:
					moveRightTime.timeout = sf::milliseconds(50);
					break;
				case 3:
					moveRightTime.timeout = sf::milliseconds(20);
					break;
				default:
					break;
				}
			}
			else
			{
				moveRightTime.timeout = sf::milliseconds(100);
				rightMoves = 0;
			}
		});

	if (hasActiveInputEvent(game, rotateEventHash) == true)
	{
		if (rotateWasPressed == false)
		{
			level.rotateSelectedJewels(game, false);
			rotateWasPressed = true;
		}
	}
	else
	{
		rotateWasPressed = false;
	}

	if (hasActiveInputEvent(game, reverseRotateEventHash) == true)
	{
		if (reverseRotateWasPressed == false)
		{
			level.rotateSelectedJewels(game, true);
			reverseRotateWasPressed = true;
		}
	}
	else
	{
		reverseRotateWasPressed = false;
	}
}
