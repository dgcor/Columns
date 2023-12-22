#include "LevelBoard.h"
#include <cmath>

void LevelBoard::Init()
{
	cells.clear();
	cells.resize((int)size.x * (int)size.y);
}

void LevelBoard::Init(PairInt16 size_, int16_t columnHeight_)
{
	if (size_.x < 4 || size_.x > 100 ||
		size_.y < 4 || size_.y > 100)
	{
		size_ = { 6, 13 };
	}
	if (columnHeight_ < 2 || columnHeight_ > size_.y / 2)
	{
		columnHeight_ = size_.y / 2;
	}

	visibleSize = size_;
	size = size_;
	size.y += columnHeight_;
	columnHeight = columnHeight_;
	Init();
}

sf::Vector2f LevelBoard::toDrawCoord(int16_t x, int16_t y) const
{
	return { tileSize * x, tileSize * y };
}

sf::Vector2f LevelBoard::toDrawCoord(float x, float y) const
{
	return { std::round(tileSize * x), std::round(tileSize * y) };
}

sf::Vector2f LevelBoard::toDrawCoord(PairFloat boardPos) const
{
	return toDrawCoord(boardPos.x, boardPos.y);
}

const LevelCell& LevelBoard::get(int16_t x, int16_t y) const
{
	return cells[x + y * size.x];
}

LevelCell& LevelBoard::get(int16_t x, int16_t y)
{
	return cells[x + y * size.x];
}

const LevelCell& LevelBoard::get(float x, float y) const
{
	return get((int16_t)x, (int16_t)y);
}

LevelCell& LevelBoard::get(float x, float y)
{
	return get((int16_t)x, (int16_t)y);
}

const LevelCell& LevelBoard::get(PairFloat boardPos) const
{
	return get(std::ceil(boardPos.x), std::ceil(boardPos.y));
}

LevelCell& LevelBoard::get(PairFloat boardPos)
{
	return get(std::ceil(boardPos.x), std::ceil(boardPos.y));
}

void LevelBoard::set(int16_t x, int16_t y, LevelCell&& cell)
{
	get(x, y) = std::move(cell);
}

bool LevelBoard::isCoordValid(PairFloat boardPos) const
{
	return (boardPos.x >= 0.f &&
		std::ceil(boardPos.x) < (float)size.x &&
		boardPos.y >= 0.f &&
		std::ceil(boardPos.y) < (float)size.y);
}

bool LevelBoard::hasJewel(PairFloat boardPos) const
{
	if (isCoordValid(boardPos) == true)
	{
		return get(boardPos).jewel != nullptr;
	}
	return false;
}

bool LevelBoard::addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat boardPos)
{
	if (isCoordValid(boardPos) == true)
	{
		auto& cell = get(boardPos);
		deleteJewel(cell.jewel);
		cell.jewel = jewel;
		if (jewel != nullptr)
		{
			jewel->BoardPosition(boardPos);
			return true;
		}
	}
	return false;
}

bool LevelBoard::deleteJewel(const std::shared_ptr<Jewel>& jewel)
{
	if (jewel != nullptr)
	{
		return deleteJewel(jewel->BoardPosition());
	}
	return false;
}

bool LevelBoard::deleteJewel(PairFloat boardPos)
{
	if (isCoordValid(boardPos) == true)
	{
		auto& cell = get(boardPos);
		if (cell.jewel != nullptr)
		{
			cell.jewel.reset();
			return true;
		}
	}
	return false;
}

Jewel* LevelBoard::moveJewel(PairFloat oldPos, PairFloat newPos)
{
	if (isCoordValid(oldPos) == true &&
		isCoordValid(newPos) == true)
	{
		auto& oldCell = get(oldPos);
		auto& newCell = get(newPos);
		if (oldCell.jewel != nullptr &&
			newCell.jewel == nullptr)
		{
			newCell.jewel = std::move(oldCell.jewel);
			newCell.jewel->BoardPosition(newPos);
			return newCell.jewel.get();
		}
	}
	return nullptr;
}

bool LevelBoard::isGameOver() const
{
	for (int16_t x = 0; x < size.x; x++)
	{
		if (hasJewel(PairFloat((float)x, (float)(columnHeight - 1))) == true)
		{
			return true;
		}
	}
	return false;
}
