#include "Level.h"
#include "Game/Game.h"
#include "LevelBoardUtils.h"
#include "LevelDraw.h"
#include "LevelSave.h"
#include "LevelUIObject.h"
#include "Utils/Random.h"

Level::Level(Game& game, const std::shared_ptr<TexturePack>& tilesTexturePack_,
	const std::shared_ptr<TexturePack>& selectTexturePack_,
	const std::shared_ptr<TexturePack>& highlightTexturePack_,
	PairInt16 boardSize_, float tileSize_) : tilesTexturePack(tilesTexturePack_),
	selectTexturePack(selectTexturePack_), highlightTexturePack(highlightTexturePack_)
{
	board.Init(boardSize_, 3);

	playerManager.setStartPosition(0, board.Size().x / 2 - 1);
	playerManager.setStartPosition(1, board.Size().x / 2);

	flashPosition = playerManager.getStartPosition(1);

	if (tileSize_ > 0.f)
	{
		board.TileSize(tileSize_);
	}
	else if (tilesTexturePack != nullptr)
	{
		board.TileSize((float)tilesTexturePack->getTextureSize(0).x);
	}
	if (board.TileSize() <= 0.f)
	{
		board.TileSize(32.f);
	}

	if (selectTexturePack != nullptr)
	{
		selectionAnim.setAnimation(selectTexturePack, 0, 0);
	}
	selectionAnim.Visible(false);

	if (highlightTexturePack != nullptr)
	{
		highlightAnim.setAnimation(highlightTexturePack, 0, 0);
	}
	highlightAnim.Visible(false);

	surface.Size(sf::Vector2f(
		(float)board.VisibleSize().x * board.TileSize(),
		(float)board.VisibleSize().y * board.TileSize()
	));
	surface.setCenter();
	surface.init(game);
	surface.updateView(game);
}

void Level::Id(const std::string_view id_)
{
	levelId = id_;
	inputManager.Init(levelId);
}

std::shared_ptr<Action> Level::getAction(uint16_t nameHash16) const noexcept
{
	return LevelUIObject::getAction(*this, nameHash16);
}

bool Level::setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	return LevelUIObject::setAction(*this, nameHash16, action);
}

void Level::draw(const Game& game, sf::RenderTarget& target) const
{
	LevelDraw::draw(*this, game, target);
}

void Level::update(Game& game)
{
	if (Visible() == false ||
		pause == true)
	{
		return;
	}

	sf::FloatRect rect(surface.Position(), surface.Size());
	hasMouseInside = rect.contains(game.MousePositionf());

	updateTimer(game);
	inputManager.processInputEvents(*this, game);

	if (sleepTime.timeout != sf::Time::Zero)
	{
		sleepTime.update(game.getElapsedTime(), [&]()
			{
				processStep(game);
			});
	}

	LevelDraw::updateAnimations(*this, game);
}

bool Level::getProperty(const std::string_view prop, Variable& var) const
{
	return LevelUIObject::getProperty(*this, prop, var);
}

QueryObject Level::getQueryable(const std::string_view prop) const
{
	return this;
}

void Level::setPlayerStartPosition(uint32_t idx, int16_t startPos_)
{
	playerManager.setStartPosition(idx, std::clamp(startPos_, (int16_t)0, (int16_t)(board.Size().x - 1)));
}

void Level::updateTimer(Game& game)
{
	if (gameState.state != LevelState::GameOver)
	{
		gameState.timer += game.getElapsedTime();
		if (gameState.isTimeOver() == true)
		{
			doGameOver(game);
		}
	}
}

void Level::addJewelClass(const std::string_view key, std::shared_ptr<JewelClass> obj)
{
	jewelsClasses.insert(std::make_pair(key, std::move(obj)));
}

const JewelClass* Level::getJewelClass(const std::string_view key) const
{
	auto it = jewelsClasses.find(key);
	if (it != jewelsClasses.end())
	{
		return it->second.get();
	}
	return nullptr;
}

void Level::addJewel(const JewelClass* class_, PairFloat boardPos)
{
	if (class_ != nullptr)
	{
		addJewel(std::make_shared<Jewel>(class_), boardPos);
	}
}

void Level::addJewel(const std::shared_ptr<Jewel>& jewel)
{
	if (jewel != nullptr)
	{
		addJewel(jewel, jewel->BoardPosition());
	}
}

void Level::addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat boardPos)
{
	if (board.addJewel(jewel, boardPos) == true)
	{
		LevelDraw::setAnimationPosition(*this, *jewel, boardPos);
	}
}

bool Level::moveJewel(PairFloat oldPos, PairFloat newPos)
{
	auto jewel = board.moveJewel(oldPos, newPos);
	if (jewel != nullptr)
	{
		LevelDraw::setAnimationPosition(*this, *jewel, newPos);
		return true;
	}
	return false;
}

void Level::initGame()
{
	board.Init();

	int32_t numTiles = -1;
	if (tilesTexturePack != nullptr)
	{
		numTiles = (int32_t)tilesTexturePack->size();
		numTiles--;
	}
	for (int16_t y = 0; y < board.Size().y; y++)
	{
		for (int16_t x = 0; x < board.Size().x; x++)
		{
			LevelCell cell;
			if (numTiles >= 0)
			{
				TextureInfo ti;
				auto randomIdx = Random::get((uint32_t)numTiles);
				if (tilesTexturePack->get(randomIdx, ti) == true)
				{
					cell.tile.setTexture(ti);
				}
			}
			cell.tile.Position(board.toDrawCoord(x, y - board.ColumnHeight()));
			board.set(x, y, std::move(cell));
		}
	}

	selectionAnim.Visible(false);
	highlightAnim.Visible(false);

	clearedJewels = false;
	flashDestroyed = false;
	magicJewelDestroyed = false;
	height = 0;
	magicJewelClass = getJewelClass(magicJewel);
}

void Level::newGame(const std::string_view gameType_, sf::Time timeLimit_,
	int16_t numPlayers, int16_t height_, int32_t initialLevel)
{
	initGame();
	gameState = {};

	if (gameType_ == "flash")
	{
		gameState.flashGame = true;
		height_ = std::max(height_, (int16_t)2);
	}
	else
	{
		gameState.flashGame = false;
	}
	height_ = std::clamp(height_, (int16_t)0, (int16_t)(board.VisibleSize().y - board.ColumnHeight()));
	gameState.timer = {};
	gameState.timeLimit = timeLimit_ >= sf::Time::Zero ? timeLimit_ : sf::Time::Zero;
	numPlayers = std::clamp(numPlayers, (int16_t)1, (int16_t)2);

	gameState.state = LevelState::AddJewels;
	playerManager.Init(numPlayers, initialLevel, optionsManager);
	updateLevelOptions();

	addJewelRows(height_, gameState.flashGame);
}

void Level::loadGame(Game& game, const std::string_view filePath)
{
	initGame();
	gameState = {};

	if (LevelSave::load(*this, filePath) == true)
	{
		updateLevelOptions();
		updateHeight();

		for (const auto& jewel : gameState.currentJewels)
		{
			LevelDraw::setAnimationPosition(*this, *jewel, jewel->BoardPosition());
		}

		switch (gameState.state)
		{
		case LevelState::AddJewels:
		{
			if (Plr().options.hints == true)
			{
				doHints();
			}
			break;
		}
		case LevelState::ClearedJewels:
		case LevelState::ClearingJewels:
		{
			gameState.state = LevelState::ClearJewels;
			onClearJewels(false);
			break;
		}
		case LevelState::GameOver:
			doGameOver(game);
			break;
		default:
			break;
		}
	}
}

void Level::saveGame(const std::string_view filePath) const
{
	LevelSave::save(*this, filePath);
}

void Level::updateLevelOptions()
{
	Plr().options = optionsManager.get(Plr().getLevel());

	sleepTime.timeout = Plr().options.timeoutSpeed;
	boardPosDropOffset = std::max(1.f / (float)Plr().options.dropSteps, 0.01f);
}

void Level::addJewelRows(int16_t numRows, bool setFlashJewel)
{
	if (jewelsClasses.size() <= 1 ||
		Plr().options.jewels.size() <= 1)
	{
		return;
	}
	height = numRows;
	int16_t y = board.Size().y - 1;
	int16_t numTries = 100;
	while (numRows > 0 && numTries > 0)
	{
		for (int16_t x = 0; x < board.Size().x; x++)
		{
			addJewel(makeRandomJewel(), PairFloat((float)x, (float)y));
		}
		bool hasMatch = false;
		LevelBoardUtils::checkBoard(*this, 0, y, board.Size().x, board.Size().y,
			[&hasMatch](Jewel& a) -> bool
			{
				hasMatch = true;
				return false;
			}
		);
		if (hasMatch == false)
		{
			numRows--;
			y--;
		}
		else
		{
			numTries--;
		}
	}
	if (setFlashJewel == true)
	{
		auto& cell = board.get(flashPosition, board.Size().y - 1);
		cell.jewel->Flash(true);
	}
}

void Level::updateHeight()
{
	int16_t newHeight = 0;
	for (int16_t y = 0; y < board.Size().y; y++)
	{
		if (newHeight > 0)
		{
			break;
		}
		for (int16_t x = 0; x < board.Size().x; x++)
		{
			if (board.hasJewel(PairFloat((float)x, (float)y)) == true)
			{
				newHeight = board.Size().y - y;
				break;
			}
		}
	}
	height = newHeight;
}

std::shared_ptr<Jewel> Level::makeRandomJewel() const
{
	if (Plr().options.jewels.empty() == false)
	{
		for (int numTries = 3; numTries > 0; numTries--)
		{
			auto idx = Random::get(Plr().options.jewels.size() - 1);
			const auto& classId = Plr().options.jewels[idx];
			auto jewelClass = getJewelClass(classId);
			if (jewelClass != nullptr)
			{
				return std::make_shared<Jewel>(jewelClass);
			}
		}
	}
	auto idx = Random::get(jewelsClasses.size() - 1);
	auto it = jewelsClasses.cbegin();
	for (size_t i = 0; i < idx; i++)
	{
		++it;
	}
	return std::make_shared<Jewel>(it->second.get());
}

Jewels Level::makeRandomJewels(int height_) const
{
	Jewels jewels;
	if (jewelsClasses.empty() == true)
	{
		return jewels;
	}
	PairFloat boardPos((float)playerManager.StartPosition(), (float)board.ColumnHeight() - 1.f);
	while (height_ > 0)
	{
		auto jewel = makeRandomJewel();
		jewel->BoardPosition(boardPos);
		LevelDraw::setAnimationPosition(*this, *jewel, boardPos);
		jewels.push_back(std::move(jewel));
		height_--;
		boardPos.y--;
	}
	return jewels;
}

Jewels Level::makeMagicJewels(int height_) const
{
	Jewels jewels;
	if (magicJewelClass == nullptr)
	{
		return jewels;
	}
	PairFloat boardPos((float)playerManager.StartPosition(), (float)board.ColumnHeight() - 1.f);
	while (height_ > 0)
	{
		auto jewel = std::make_shared<Jewel>(magicJewelClass);
		jewel->BoardPosition(boardPos);
		LevelDraw::setAnimationPosition(*this, *jewel, boardPos);
		jewels.push_back(std::move(jewel));
		height_--;
		boardPos.y--;
	}
	return jewels;
}

void Level::processStep(Game& game)
{
	switch (gameState.state)
	{
	case LevelState::NextRound:
		onNextRound();
		[[fallthrough]];
	case LevelState::AddJewels:
		onAddJewels(game);
		break;
	case LevelState::ClearJewels:
		onClearJewels(true);
		break;
	case LevelState::ClearingJewels:
		onClearingJewels(game);
		break;
	case LevelState::ClearedJewels:
		onClearedJewels(game);
		break;
	case LevelState::GameOver:
	default:
		break;
	}
}

void Level::onNextRound()
{
	if (playerManager.size() > 0)
	{
		playerManager.nextPlayer();
		updateLevelOptions();
	}
	gameState.state = LevelState::AddJewels;
}

void Level::onAddJewels(Game& game)
{
	if (board.isGameOver() == true)
	{
		doGameOver(game);
	}
	if (gameState.chain > 0)
	{
		gameState.chain = 0;
	}
	if (gameState.currentJewels.empty() == true)
	{
		bool addedMagicJewel = false;
		if (Plr().magicJewelsUsed < Plr().options.magicJewels &&
			height >= Plr().options.magicHeight)
		{
			if (Random::get(4) == 0)
			{
				gameState.currentJewels = makeMagicJewels(board.ColumnHeight());
				Plr().magicJewelsUsed++;
				addedMagicJewel = true;
			}
		}
		if (addedMagicJewel == false)
		{
			gameState.currentJewels = makeRandomJewels(board.ColumnHeight());
			if (Plr().options.hints == true)
			{
				doHints();
			}
		}
		sleepTime.timeout = Plr().options.dropSpeed;
		return;
	}
	bool generateNew = false;
	bool updateSelected = false;
	for (auto& jewel : gameState.currentJewels)
	{
		auto boardPos = jewel->BoardPosition();
		boardPos.y += boardPosDropOffset;
		if (std::ceil(boardPos.y) < (float)board.Size().y)
		{
			if (board.hasJewel(boardPos) == false)
			{
				jewel->BoardPosition(boardPos);
				LevelDraw::setAnimationPosition(*this, *jewel, boardPos);
				if (updateSelected == false)
				{
					LevelDraw::setAnimationPosition(*this, selectionAnim, boardPos);
					selectionAnim.Visible(true);
					updateSelected = true;
				}
				continue;
			}
		}
		generateNew = true;
		break;
	}
	if (generateNew == true)
	{
		for (auto& jewel : gameState.currentJewels)
		{
			auto boardPos = jewel->BoardPosition();
			if (board.isCoordValid(boardPos) == true)
			{
				board.get(boardPos).jewel = std::move(jewel);
			}
		}
		gameState.currentJewels.clear();
		updateHeight();
		gameState.state = LevelState::ClearJewels;
	}
	else
	{
		checkIfTouchedBase(game);
	}
}

void Level::onClearJewels(bool updateStats)
{
	selectionAnim.Visible(false);
	highlightAnim.Visible(false);
	highlightPositions.clear();

	auto func = std::bind(
		updateStats == true ? &Level::setDeleteAndUpdatePoints : &Level::setDelete,
		this,
		std::placeholders::_1
	);
	LevelBoardUtils::checkBoard(*this, 0, 0, board.Size().x, board.Size().y, func);

	if (Plr().jewelsDestroyed > 0)
	{
		clearedJewels = false;
		gameState.state = LevelState::ClearingJewels;
		if (updateStats == true)
		{
			gameState.chain++;
			Plr().scoreRound = (int32_t)scoreFormula.eval(*this);
		}
		sleepTime.timeout = Plr().options.updateScoreSpeed;
	}
	else
	{
		gameState.state = LevelState::NextRound;
		sleepTime.timeout = Plr().options.timeoutSpeed;
	}
}

void Level::onClearingJewels(Game& game)
{
	gameState.state = LevelState::ClearedJewels;
	sleepTime.timeout = sf::milliseconds(1);
	game.Events().tryAddBack(destroyAction);
	updateScore(game);
}

void Level::onClearedJewels(Game& game)
{
	if (clearedJewels == false)
	{
		return;
	}
	magicJewelDestroyed = false;
	if (flashDestroyed == true)
	{
		doGameOver(game);
	}
	else
	{
		gameState.state = LevelState::ClearJewels;
	}
	sleepTime.timeout = Plr().options.timeoutSpeed;
	Plr().scoreRound = 0;
	Plr().jewelsDestroyed = 0;
	Plr().jewelsPoints = 0;
	for (int16_t y = 0; y < board.Size().y; y++)
	{
		for (int16_t x = 0; x < board.Size().x; x++)
		{
			deleteJewelAndShift(PairFloat((float)x, (float)y));
		}
	}
	updateHeight();
}

void Level::doGameOver(Game& game)
{
	gameState.state = LevelState::GameOver;
	game.Events().tryAddBack(gameOverAction);
}

void Level::doHints()
{
	highlightPositions = LevelBoardUtils::getHintPositions(*this);
	highlightAnim.reset();
	highlightAnim.Visible(true);
}

bool Level::setDelete(Jewel& jewel)
{
	// don't remove jewels outside visible area
	if (jewel.BoardPosition().y < (float)board.ColumnHeight())
	{
		return true;
	}
	jewel.Delete();
	updateLogic(jewel);
	return true;
}

bool Level::setDeleteAndUpdatePoints(Jewel& jewel)
{
	// don't remove jewels outside visible area
	if (jewel.BoardPosition().y < (float)board.ColumnHeight())
	{
		return true;
	}
	if (jewel.Delete() == true)
	{
		Plr().jewelsDestroyed++;
		Plr().jewelsPoints += jewel.Points();
	}
	updateLogic(jewel);
	return true;
}

void Level::updateLogic(Jewel& jewel)
{
	if (gameState.flashGame == true && jewel.Flash() == true)
	{
		flashDestroyed = true;
	}
	if (magicJewelClass != nullptr &&
		magicJewelClass == jewel.Class())
	{
		magicJewelDestroyed = true;
	}
}

bool Level::checkIfTouchedBase()
{
	if (gameState.currentJewels.empty() == true)
	{
		return false;
	}
	sleepTime.timeout = Plr().options.dropSpeed;
	auto boardPos = gameState.currentJewels.front()->BoardPosition();
	boardPos.y += boardPosDropOffset;
	return (std::ceil(boardPos.y) >= (float)board.Size().y || board.hasJewel(boardPos) == true);
}

bool Level::checkIfTouchedBase(Game& game)
{
	if (checkIfTouchedBase() == true)
	{
		sleepTime.timeout = Plr().options.timeoutSpeed;
		game.Events().tryAddBack(touchBaseAction);
		return true;
	}
	return false;
}

void Level::deleteJewelAndShift(PairFloat boardPos)
{
	if (board.hasJewel(boardPos) == false)
	{
		return;
	}
	const auto& cell = board.get(boardPos);
	if (cell.jewel == nullptr ||
		cell.jewel->Deleted() == false)
	{
		return;
	}
	if (board.deleteJewel(boardPos) == true)
	{
		PairFloat boardPosA;
		PairFloat boardPosB = boardPos;
		do
		{
			boardPosA = boardPosB;
			boardPosB.y--;
		} while (moveJewel(boardPosB, boardPosA) == true);
	}
}

void Level::rotateSelectedJewels(Game& game, bool reverse)
{
	if (gameState.currentJewels.empty() == true ||
		gameState.state == LevelState::GameOver ||
		pause == true)
	{
		return;
	}
	auto boardPos = gameState.currentJewels.front()->BoardPosition();

	gameState.currentJewels.rotate(reverse);

	for (auto& jewel : gameState.currentJewels)
	{
		jewel->BoardPosition(boardPos);
		LevelDraw::setAnimationPosition(*this, *jewel, boardPos);
		boardPos.y--;
	}
	game.Events().tryAddBack(rotateAction);
}

void Level::moveSelectedJewels(Game& game, PairInt16 boardPosOffset, bool addDropPoints)
{
	if (gameState.currentJewels.empty() == true ||
		gameState.state == LevelState::GameOver ||
		pause == true)
	{
		return;
	}
	auto boardPos = gameState.currentJewels.front()->BoardPosition();
	auto newboardPos = boardPos;
	newboardPos.x += (float)boardPosOffset.x;
	newboardPos.y += (float)boardPosOffset.y * boardPosDropOffset;
	auto checkboardPos = newboardPos;
	if (checkboardPos.x < 0.f ||
		std::ceil(checkboardPos.x) >= (float)board.Size().x ||
		checkboardPos.y < -1.f ||
		std::ceil(checkboardPos.y) >= (float)board.Size().y ||
		board.hasJewel(checkboardPos) == true)
	{
		return;
	}
	if (addDropPoints == true && boardPosOffset.y > 0)
	{
		Plr().score += (int32_t)(Plr().options.dropPoints * boardPosOffset.y);
	}
	LevelDraw::setAnimationPosition(*this, selectionAnim, newboardPos);
	selectionAnim.Visible(true);
	for (auto& jewel : gameState.currentJewels)
	{
		jewel->BoardPosition(newboardPos);
		LevelDraw::setAnimationPosition(*this, *jewel, newboardPos);
		newboardPos.y--;
	}
	if (checkIfTouchedBase(game) == true)
	{
		for (auto& jewel : gameState.currentJewels)
		{
			LevelDraw::setAnimationPosition(*this, *jewel, jewel->BoardPosition());
		}
	}
}

void Level::updateScore(Game& game)
{
	Plr().score = std::clamp(Plr().score + Plr().scoreRound, Plr().options.scoreMin, Plr().options.scoreMax);
	Plr().jewels = std::clamp(Plr().jewels + Plr().jewelsDestroyed, 0, Plr().options.jewelsMax);
	auto levelUp = (int32_t)levelUpFormula.eval(*this);
	if (levelUp > Plr().levelUp && (Plr().levelStart + levelUp) <= Plr().options.levelMax)
	{
		Plr().levelUp = levelUp;
		updateLevelOptions();
		game.Events().tryAddBack(levelUpAction);
	}
}
