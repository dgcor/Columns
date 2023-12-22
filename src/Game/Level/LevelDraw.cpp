#include "LevelDraw.h"
#include "Game/Drawables/Animation.h"
#include "Game/Game.h"
#include "Level.h"
#include "Utils/StringHash.h"

void LevelDraw::setAnimationPosition(const Level& level, Animation& anim, PairFloat boardPos)
{
	boardPos.y -= level.board.ColumnHeight();
	auto animSize = anim.Size();
	auto drawCoord = level.board.toDrawCoord(boardPos);
	drawCoord.x = drawCoord.x - animSize.x + level.board.TileSize();
	drawCoord.y = drawCoord.y - animSize.y + level.board.TileSize();
	anim.Position(drawCoord);
}

void LevelDraw::updateAnimations(Level& level, Game& game)
{
	updateCurrentJewelsAnimation(level, game);
	level.selectionAnim.update(game);
	level.highlightAnim.update(game);

	for (int16_t y = 0; y < level.board.Size().y; y++)
	{
		for (int16_t x = 0; x < level.board.Size().x; x++)
		{
			auto& cell = level.board.get(x, y);
			if (cell.jewel != nullptr)
			{
				cell.jewel->update(game);
				level.clearedJewels |= cell.jewel->hasPlayOnceAnimationFinished();
			}
		}
	}
}

void LevelDraw::updateCurrentJewelsAnimation(Level& level, Game& game)
{
	for (const auto& jewel : level.gameState.currentJewels)
	{
		jewel->update(game);
	}
}

void LevelDraw::draw(const Level& level, const Game& game, sf::RenderTarget& target)
{
	if (level.Visible() == false)
	{
		return;
	}

	auto origView = target.getView();

	level.surface.clear(sf::Color::Transparent);

	for (int16_t y = level.board.ColumnHeight(); y < level.board.Size().y; y++)
	{
		for (int16_t x = 0; x < level.board.Size().x; x++)
		{
			auto& cell = level.board.get(x, y);
			level.surface.draw(game, cell.tile);
		}
	}
	for (int16_t y = level.board.ColumnHeight(); y < level.board.Size().y; y++)
	{
		for (int16_t x = 0; x < level.board.Size().x; x++)
		{
			auto& cell = level.board.get(x, y);
			if (cell.jewel != nullptr)
			{
				level.surface.draw(game, *cell.jewel);
			}
		}
	}
	for (const auto& jewel : level.gameState.currentJewels)
	{
		if (jewel != nullptr)
		{
			level.surface.draw(game, *jewel);
		}
	}

	level.surface.draw(game, level.selectionAnim);
	if (level.Plr().options.hints == true)
	{
		for (const auto& boardPos : level.highlightPositions)
		{
			setAnimationPosition(level, level.highlightAnim, boardPos);
			level.surface.draw(game, level.highlightAnim);
		}
	}

	auto surfaceStates(sf::RenderStates::Default);
	if (level.gameShader != nullptr)
	{
		auto shader = level.gameShader->shader.get();
		surfaceStates.shader = shader;
		for (auto uniformHash : level.gameShader->uniforms)
		{
			switch (uniformHash)
			{
			case str2int16("elapsedTime"):
			{
				shader->setUniform("elapsedTime", game.getTotalElapsedTime().asSeconds());
				break;
			}
			case str2int16("mousePosition"):
			{
				if (level.hasMouseInside == true)
				{
					shader->setUniform("mousePosition", sf::Glsl::Vec2(
						(game.MousePositionf().x - level.surface.Position().x) /
						level.surface.Size().x,
						(game.MousePositionf().y - level.surface.Position().y) /
						level.surface.Size().y
					));
				}
				break;
			}
			case str2int16("textureSize"):
			{
				shader->setUniform("textureSize", sf::Glsl::Vec2(
					level.surface.Size().x,
					level.surface.Size().y
				));
				break;
			}
			case str2int16("visibleRect"):
			{
				shader->setUniform("visibleRect", sf::Glsl::Vec4(
					level.surface.visibleRect.left,
					level.surface.visibleRect.top,
					level.surface.visibleRect.width,
					level.surface.visibleRect.height
				));
				break;
			}
			default:
				break;
			}
		}
	}
	level.surface.draw(target, surfaceStates);

	target.setView(origView);
}
