#pragma once

#include "LevelCell.h"
#include <vector>

class LevelBoard
{
private:
	std::vector<LevelCell> cells;

	// size of cells. height = board height + columnHeight
	PairInt16 size;

	// visible board height
	PairInt16 visibleSize;

	// height of jewel column
	int16_t columnHeight{ 3 };

	// size of single jewel width/height
	float tileSize{ 0 };

public:
	auto begin() noexcept { return cells.begin(); }
	auto end() noexcept { return cells.end(); }
	auto begin() const noexcept { return cells.begin(); }
	auto end() const noexcept { return cells.end(); }
	auto cbegin() const noexcept { return cells.cbegin(); }
	auto cend() const noexcept { return cells.cend(); }
	auto rbegin() noexcept { return cells.rbegin(); }
	auto rend() noexcept { return cells.rend(); }
	auto rbegin() const noexcept { return cells.rbegin(); }
	auto rend() const noexcept { return cells.rend(); }
	auto crbegin() const noexcept { return cells.crbegin(); }
	auto crend() const noexcept { return cells.crend(); }

	void Init();
	void Init(PairInt16 size_, int16_t columnHeight_);

	// board height + jewelSize
	const auto& Size() const noexcept { return size; }

	const auto& VisibleSize() const noexcept { return visibleSize; }

	// size of jewel column
	auto ColumnHeight() const noexcept { return columnHeight; }

	auto TileSize() const noexcept { return tileSize; }
	void TileSize(float tileSize_) noexcept { tileSize = tileSize_; }

	sf::Vector2f toDrawCoord(int16_t x, int16_t y) const;
	sf::Vector2f toDrawCoord(float x, float y) const;
	sf::Vector2f toDrawCoord(PairFloat boardPos) const;

	const LevelCell& get(int16_t x, int16_t y) const;
	LevelCell& get(int16_t x, int16_t y);
	const LevelCell& get(float x, float y) const;
	LevelCell& get(float x, float y);
	const LevelCell& get(PairFloat boardPos) const;
	LevelCell& get(PairFloat boardPos);

	void set(int16_t x, int16_t y, LevelCell&& cell);

	bool isCoordValid(PairFloat boardPos) const;

	bool hasJewel(PairFloat boardPos) const;
	bool addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat boardPos);
	bool deleteJewel(const std::shared_ptr<Jewel>& jewel);
	bool deleteJewel(PairFloat boardPos);
	Jewel* moveJewel(PairFloat oldPos, PairFloat newPos);

	// true if there are jewels above visible area
	bool isGameOver() const;
};
