#pragma once

#include "Utils/PairXY.h"
#include <SFML/Graphics/RenderTarget.hpp>

class Animation;
class Game;
class Level;

class LevelDraw
{
private:
	static void updateCurrentJewelsAnimation(Level& level, Game& game);

public:
	static void setAnimationPosition(const Level& level, Animation& anim, PairFloat boardPos);

	static void updateAnimations(Level& level, Game& game);

	static void draw(const Level& level, const Game& game, sf::RenderTarget& target);
};
