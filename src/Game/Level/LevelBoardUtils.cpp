#include "LevelBoardUtils.h"
#include "Level.h"

std::vector<PairFloat> LevelBoardUtils::getHintPositions(const Level& level)
{
	std::vector<PairFloat> hintPositions;

	std::vector<const JewelClass*> checkClasses;
	for (const auto& jewel : level.CurrentJewels())
	{
		auto cls = jewel->Class();
		if (std::find(checkClasses.cbegin(), checkClasses.cend(), cls) == checkClasses.cend())
		{
			checkClasses.push_back(cls);
		}
	}
	for (int16_t x = 0; x < level.Board().Size().x; x++)
	{
		auto height = (int16_t)level.CurrentJewels().size() - 1;
		for (int16_t y = level.Board().Size().y - 1; y >= 0 && height >= 0; y--)
		{
			PairFloat boardPos(x, y);
			if (level.Board().get(boardPos).jewel != nullptr)
			{
				continue;
			}
			height--;
			for (auto cls : checkClasses)
			{
				// horizontal 1
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y);
					PairFloat boardPosB(boardPos.x + 1.f, boardPos.y);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// horizontal 2
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y);
					PairFloat boardPosB(boardPos.x - 2.f, boardPos.y);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// horizontal 3
				{
					PairFloat boardPosA(boardPos.x + 1.f, boardPos.y);
					PairFloat boardPosB(boardPos.x + 2.f, boardPos.y);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// vertical
				{
					PairFloat boardPosA(boardPos.x, boardPos.y + 1.f);
					PairFloat boardPosB(boardPos.x, boardPos.y + 2.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 1
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y - 1.f);
					PairFloat boardPosB(boardPos.x + 1.f, boardPos.y + 1.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 1 - 2
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y - 1.f);
					PairFloat boardPosB(boardPos.x - 2.f, boardPos.y - 2.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 1 - 3
				{
					PairFloat boardPosA(boardPos.x + 1.f, boardPos.y + 1.f);
					PairFloat boardPosB(boardPos.x + 2.f, boardPos.y + 2.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 2 - 1
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y + 1.f);
					PairFloat boardPosB(boardPos.x + 1.f, boardPos.y - 1.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 2 - 2
				{
					PairFloat boardPosA(boardPos.x - 1.f, boardPos.y + 1.f);
					PairFloat boardPosB(boardPos.x - 2.f, boardPos.y + 2.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}

				// diagonal 2 - 3
				{
					PairFloat boardPosA(boardPos.x + 1.f, boardPos.y - 1.f);
					PairFloat boardPosB(boardPos.x + 2.f, boardPos.y - 2.f);
					if (level.Board().isCoordValid(boardPosA) == true &&
						level.Board().isCoordValid(boardPosB) == true)
					{
						const auto& cellA = level.Board().get(boardPosA);
						const auto& cellB = level.Board().get(boardPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							hintPositions.push_back(boardPosA);
							hintPositions.push_back(boardPosB);
						}
					}
				}
			}
		}
	}
	return hintPositions;
}

void LevelBoardUtils::checkBoard(const Level& level, int16_t startX, int16_t startY,
	int16_t stopX, int16_t stopY, const std::function<bool(Jewel&)> func)
{
	for (int16_t y = stopY - 1; y >= startY; y--)
	{
		for (int16_t x = startX; x < stopX; x++)
		{
			PairFloat boardPos(x, y);
			const auto& cell = level.Board().get(boardPos);
			if (cell.jewel == nullptr)
			{
				continue;
			}

			// magic
			auto cellClass = cell.jewel->Class();
			if (cellClass == level.MagicJewelClass())
			{
				PairFloat boardPosA(boardPos.x, boardPos.y + 1.f);
				if (level.Board().isCoordValid(boardPosA) == true)
				{
					const auto& cellA = level.Board().get(boardPosA);
					if (cellA.jewel != nullptr)
					{
						const auto& cellAClass = cellA.jewel->Class();
						if (cellAClass != level.MagicJewelClass())
						{
							for (int16_t y2 = startY; y2 < stopY; y2++)
							{
								for (int16_t x2 = startX; x2 < stopX; x2++)
								{
									PairFloat boardPosB(x2, y2);
									const auto& cellB = level.Board().get(boardPosB);
									if (cellB.jewel == nullptr)
									{
										continue;
									}
									const auto& cellBClass = cellB.jewel->Class();
									if (cellBClass != cellClass &&
										cellBClass != cellAClass)
									{
										continue;
									}
									if (func(*cellB.jewel) == false)
									{
										return;
									}
								}
							}
							return;
						}
					}
				}
			}

			// horizontal
			{
				PairFloat boardPosA(boardPos.x - 1.f, boardPos.y);
				PairFloat boardPosB(boardPos.x + 1.f, boardPos.y);
				if (level.Board().isCoordValid(boardPosA) == true &&
					level.Board().isCoordValid(boardPosB) == true)
				{
					const auto& cellA = level.Board().get(boardPosA);
					const auto& cellB = level.Board().get(boardPosB);
					if (cellA.jewel != nullptr &&
						cellB.jewel != nullptr &&
						cellA.jewel->Class() == cell.jewel->Class() &&
						cellB.jewel->Class() == cell.jewel->Class())
					{
						if (func(*cell.jewel) == false ||
							func(*cellA.jewel) == false ||
							func(*cellB.jewel) == false)
						{
							return;
						}
					}
				}
			}

			// vertical
			{
				PairFloat boardPosA(boardPos.x, boardPos.y - 1.f);
				PairFloat boardPosB(boardPos.x, boardPos.y + 1.f);
				if (level.Board().isCoordValid(boardPosA) == true &&
					level.Board().isCoordValid(boardPosB) == true)
				{
					const auto& cellA = level.Board().get(boardPosA);
					const auto& cellB = level.Board().get(boardPosB);
					if (cellA.jewel != nullptr &&
						cellB.jewel != nullptr &&
						cellA.jewel->Class() == cell.jewel->Class() &&
						cellB.jewel->Class() == cell.jewel->Class())
					{
						if (func(*cell.jewel) == false ||
							func(*cellA.jewel) == false ||
							func(*cellB.jewel) == false)
						{
							return;
						}
					}
				}
			}

			// diagonal 1
			{
				PairFloat boardPosA(boardPos.x - 1.f, boardPos.y - 1.f);
				PairFloat boardPosB(boardPos.x + 1.f, boardPos.y + 1.f);
				if (level.Board().isCoordValid(boardPosA) == true &&
					level.Board().isCoordValid(boardPosB) == true)
				{
					const auto& cellA = level.Board().get(boardPosA);
					const auto& cellB = level.Board().get(boardPosB);
					if (cellA.jewel != nullptr &&
						cellB.jewel != nullptr &&
						cellA.jewel->Class() == cell.jewel->Class() &&
						cellB.jewel->Class() == cell.jewel->Class())
					{
						if (func(*cell.jewel) == false ||
							func(*cellA.jewel) == false ||
							func(*cellB.jewel) == false)
						{
							return;
						}
					}
				}
			}

			// diagonal 2
			{
				PairFloat boardPosA(boardPos.x - 1.f, boardPos.y + 1.f);
				PairFloat boardPosB(boardPos.x + 1.f, boardPos.y - 1.f);
				if (level.Board().isCoordValid(boardPosA) == true &&
					level.Board().isCoordValid(boardPosB) == true)
				{
					const auto& cellA = level.Board().get(boardPosA);
					const auto& cellB = level.Board().get(boardPosB);
					if (cellA.jewel != nullptr &&
						cellB.jewel != nullptr &&
						cellA.jewel->Class() == cell.jewel->Class() &&
						cellB.jewel->Class() == cell.jewel->Class())
					{
						if (func(*cell.jewel) == false ||
							func(*cellA.jewel) == false ||
							func(*cellB.jewel) == false)
						{
							return;
						}
					}
				}
			}
		}
	}
}
