#include "Level.h"
#include "Game.h"
#include "GameUtils.h"
#include "Utils/Utils.h"

static LevelOptions defaultOptions;

Level::Level(Game& game, const std::shared_ptr<TexturePack>& tilesTexturePack_,
	const std::shared_ptr<TexturePack>& selectTexturePack_,
	const std::shared_ptr<TexturePack>& highlightTexturePack_,
	PairInt16 mapSize_, float tileSize_) : tilesTexturePack(tilesTexturePack_),
	selectTexturePack(selectTexturePack_), highlightTexturePack(highlightTexturePack_)
{
	if (mapSize_.x < 4 || mapSize_.x > 100 ||
		mapSize_.y < 4 || mapSize_.y > 100)
	{
		mapSize = { 6, 13 };
	}
	else
	{
		mapSize = mapSize_;
	}

	startPositions[0] = (mapSize.x / 2 - 1);
	startPositions[1] = (mapSize.x / 2);

	flashPosition = startPositions[1];

	if (tileSize_ > 0.f)
	{
		tileSize = tileSize_;
	}
	else if (tilesTexturePack != nullptr)
	{
		tileSize = (float)tilesTexturePack->getWidth(0);
	}
	if (tileSize <= 0.f)
	{
		tileSize = 32.f;
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
		(float)mapSize.x * tileSize,
		(float)mapSize.y * tileSize
	));
	surface.setCenter();
	surface.init(game);
	surface.updateView(game);
}

void Level::Id(const std::string_view id_)
{
	levelId = id_;
	moveDownEventHash = str2int16(levelId + ".down");
	moveLeftEventHash = str2int16(levelId + ".left");
	moveRightEventHash = str2int16(levelId + ".right");
}

std::shared_ptr<Action> Level::getAction(uint16_t nameHash16) const noexcept
{
	switch (nameHash16)
	{
	case str2int16("levelUp"):
		return levelUpAction;
	case str2int16("rotate"):
		return rotateAction;
	case str2int16("touchBase"):
		return touchBaseAction;
	case str2int16("destroy"):
		return destroyAction;
	case str2int16("gameOver"):
		return gameOverAction;
	default:
		return nullptr;
	}
}

bool Level::setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	switch (nameHash16)
	{
	case str2int16("levelUp"):
		levelUpAction = action;
		break;
	case str2int16("rotate"):
		rotateAction = action;
		break;
	case str2int16("touchBase"):
		touchBaseAction = action;
		break;
	case str2int16("destroy"):
		destroyAction = action;
		break;
	case str2int16("gameOver"):
		gameOverAction = action;
		break;
	default:
		return false;
	}
	return true;
}

void Level::draw(const Game& game, sf::RenderTarget& target) const
{
	if (Visible() == false)
	{
		return;
	}

	auto origView = target.getView();

	surface.clear(sf::Color::Transparent);

	if (cells.empty() == false)
	{
		for (int16_t y = 0; y < mapSize.y; y++)
		{
			for (int16_t x = 0; x < mapSize.x; x++)
			{
				auto& cell = get(x, y);
				surface.draw(game, cell.tile);
			}
		}
		for (int16_t y = 0; y < mapSize.y; y++)
		{
			for (int16_t x = 0; x < mapSize.x; x++)
			{
				auto& cell = get(x, y);
				if (cell.jewel != nullptr)
				{
					surface.draw(game, *cell.jewel);
				}
			}
		}
		for (const auto& jewel : currentJewels)
		{
			if (jewel != nullptr)
			{
				surface.draw(game, *jewel);
			}
		}
	}

	surface.draw(game, selectionAnim);
	if (Plr().options.hints == true)
	{
		for (const auto& mapPos : highlightPositions)
		{
			setAnimationPosition(highlightAnim, mapPos);
			surface.draw(game, highlightAnim);
		}
	}

	auto surfaceStates(sf::RenderStates::Default);
	if (gameShader != nullptr)
	{
		auto shader = gameShader->shader.get();
		surfaceStates.shader = shader;
		for (auto uniformHash : gameShader->uniforms)
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
				if (hasMouseInside == true)
				{
					shader->setUniform("mousePosition", sf::Glsl::Vec2(
						(game.MousePositionf().x - surface.Position().x) /
						surface.Size().x,
						(game.MousePositionf().y - surface.Position().y) /
						surface.Size().y
					));
				}
				break;
			}
			case str2int16("textureSize"):
			{
				shader->setUniform("textureSize", sf::Glsl::Vec2(
					surface.Size().x,
					surface.Size().y
				));
				break;
			}
			case str2int16("visibleRect"):
			{
				shader->setUniform("visibleRect", sf::Glsl::Vec4(
					surface.visibleRect.left,
					surface.visibleRect.top,
					surface.visibleRect.width,
					surface.visibleRect.height
				));
				break;
			}
			default:
				break;
			}
		}
	}
	surface.draw(target, surfaceStates);

	target.setView(origView);
}

void Level::update(Game& game)
{
	if (Visible() == false)
	{
		return;
	}
	if (pause == true)
	{
		return;
	}

	sf::FloatRect rect(surface.Position(), surface.Size());
	if (rect.contains(game.MousePositionf()) == true)
	{
		hasMouseInside = true;
	}
	else
	{
		hasMouseInside = false;
	}

	updateTimer(game);
	processInputEvents(game);

	if (sleepTime.timeout != sf::Time::Zero &&
		sleepTime.update(game.getElapsedTime()) == true)
	{
		processStep(game);
	}

	updateCurrentJewelsAnimation(game);
	selectionAnim.update(game);
	highlightAnim.update(game);

	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			auto& cell = get(x, y);
			if (cell.jewel != nullptr)
			{
				cell.jewel->update(game);
				clearedJewels |= cell.jewel->hasPlayOnceAnimationFinished();
			}
		}
	}
}

bool Level::getProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("id"):
		var = Variable(levelId);
		return true;
	case str2int16("name"):
		var = Variable(name);
		return true;
	case str2int16("mapSize"):
	{
		if (props.second == "x")
		{
			var = Variable((int64_t)mapSize.x);
		}
		else
		{
			var = Variable((int64_t)mapSize.y);
		}
		return true;
	}
	case str2int16("pause"):
		var = Variable(pause);
		return true;
	case str2int16("timer"):
		var = Variable(GameUtils::getTime(timer, props.second, true));
		return true;
	case str2int16("timeLimit"):
		var = Variable(GameUtils::getTime(timeLimit, props.second));
		return true;
	case str2int16("timeLeft"):
		var = Variable(GameUtils::getTime(getTimeLeft(), props.second));
		return true;
	case str2int16("timeTrial"):
		var = Variable(timeTrial);
		return true;
	case str2int16("gameType"):
		var = Variable(flashGame ? "flash" : "original");
		return true;
	case str2int16("gameOver"):
		var = Variable(gameState == LevelState::GameOver);
		return true;
	case str2int16("level"):
		var = Variable((int64_t)Plr().getLevel());
		return true;
	case str2int16("levelStart"):
		var = Variable((int64_t)Plr().levelStart);
		return true;
	case str2int16("levelUp"):
		var = Variable((int64_t)Plr().levelUp);
		return true;
	case str2int16("score"):
		var = Variable(Plr().score);
		return true;
	case str2int16("scoreRound"):
		var = Variable((int64_t)Plr().scoreRound);
		return true;
	case str2int16("jewels"):
		var = Variable((int64_t)Plr().jewels);
		return true;
	case str2int16("jewelsDestroyed"):
		var = Variable((int64_t)Plr().jewelsDestroyed);
		return true;
	case str2int16("jewelsPoints"):
		var = Variable((int64_t)Plr().jewelsPoints);
		return true;
	case str2int16("magicJewels"):
		var = Variable((int64_t)Plr().options.magicJewels);
		return true;
	case str2int16("magicJewelsUsed"):
		var = Variable((int64_t)Plr().magicJewelsUsed);
		return true;
	case str2int16("magicJewelDestroyed"):
		var = Variable(magicJewelDestroyed);
		return true;
	case str2int16("height"):
		var = Variable((int64_t)height);
		return true;
	case str2int16("chain"):
		var = Variable((int64_t)chain);
		return true;
	case str2int16("hasHighScore"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(idx < highScores.size());
		return true;
	}
	case str2int16("players"):
		var = Variable((int64_t)players.size());
		return true;
	case str2int16("playerIdx"):
		var = Variable((int64_t)playerIdx);
		return true;
	case str2int16("player1"):
		return getStatsProperty(0, props.second, var);
	case str2int16("player2"):
		return getStatsProperty(1, props.second, var);
	case str2int16("highScore"):
		return getHighScoreProperty(props.second, var);
	default:
		return getUIObjProp(propHash, props.second, var);
	}
	return false;
}

bool Level::getStatsProperty(uint32_t idx, const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1 || idx >= players.size())
	{
		return false;
	}
	switch (str2int16(prop))
	{
	case str2int16("level"):
		var = Variable((int64_t)players[idx].getLevel());
		return true;
	case str2int16("levelStart"):
		var = Variable((int64_t)players[idx].levelStart);
		return true;
	case str2int16("levelUp"):
		var = Variable((int64_t)players[idx].levelUp);
		return true;
	case str2int16("score"):
		var = Variable(players[idx].score);
		return true;
	case str2int16("scoreRound"):
		var = Variable((int64_t)players[idx].scoreRound);
		return true;
	case str2int16("jewels"):
		var = Variable((int64_t)players[idx].jewels);
		return true;
	case str2int16("jewelsDestroyed"):
		var = Variable((int64_t)players[idx].jewelsDestroyed);
		return true;
	case str2int16("jewelsPoints"):
		var = Variable((int64_t)players[idx].jewelsPoints);
		return true;
	case str2int16("magicJewels"):
		var = Variable((int64_t)players[idx].options.magicJewels);
		return true;
	case str2int16("magicJewelsUsed"):
		var = Variable((int64_t)players[idx].magicJewelsUsed);
		return true;
	default:
		return false;
	}
	return false;
}

bool Level::getHighScoreProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	int idx = -1;
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("highest"):
		idx = (int)(highScores.empty() == false ? 0 : -1);
		break;
	case str2int16("lowest"):
		idx = (int)(highScores.empty() == false ? highScores.size() - 1 : -1);
		break;
	default:
		idx = Utils::strtou(props.first);
		break;
	}
	if (idx >= 0 && (size_t)idx < highScores.size())
	{
		props = Utils::splitStringIn2(props.second, '.');
		switch (str2int16(props.first))
		{
		case str2int16("name"):
			var = Variable(highScores[idx].name);
			return true;
		case str2int16("level"):
			var = Variable((int64_t)highScores[idx].level);
			return true;
		case str2int16("score"):
			var = Variable((int64_t)highScores[idx].score);
			return true;
		case str2int16("jewels"):
			var = Variable((int64_t)highScores[idx].jewels);
			return true;
		case str2int16("time"):
			var = Variable(GameUtils::getTime(highScores[idx].time, props.second, true));
			return true;
		default:
			return false;
		}
	}
	return false;
}

const Queryable* Level::getQueryable(const std::string_view prop) const
{
	return this;
}

std::vector<std::variant<const Queryable*, Variable>> Level::getQueryableList(
	const std::string_view prop) const
{
	return {};
}

sf::Vector2f Level::toDrawCoord(int16_t x, int16_t y) const
{
	return { tileSize * x, tileSize * y };
}

sf::Vector2f Level::toDrawCoord(float x, float y) const
{
	return { std::round(tileSize * x), std::round(tileSize * y) };
}

sf::Vector2f Level::toDrawCoord(PairFloat mapPos) const
{
	return toDrawCoord(mapPos.x, mapPos.y);
}

const Level::LevelCell& Level::get(int16_t x, int16_t y) const
{
	return cells[x + y * mapSize.x];
}

Level::LevelCell& Level::get(int16_t x, int16_t y)
{
	return cells[x + y * mapSize.x];
}

const Level::LevelCell& Level::get(float x, float y) const
{
	return get((int16_t)x, (int16_t)y);
}

Level::LevelCell& Level::get(float x, float y)
{
	return get((int16_t)x, (int16_t)y);
}

const Level::LevelCell& Level::get(PairFloat mapPos) const
{
	return get(std::ceil(mapPos.x), std::ceil(mapPos.y));
}

Level::LevelCell& Level::get(PairFloat mapPos)
{
	return get(std::ceil(mapPos.x), std::ceil(mapPos.y));
}

void Level::setPlayerStartPosition(size_t idx, int16_t startPos_)
{
	startPositions[idx] = std::clamp(startPos_, (int16_t)0, (int16_t)(mapSize.x - 1));
}

sf::Time Level::getTimeLeft() const noexcept
{
	if (timeTrial == true && timer < timeLimit)
	{
		return timeLimit - timer;
	}
	return sf::Time::Zero;
}

void Level::updateTimer(Game& game)
{
	if (gameState != LevelState::GameOver)
	{
		timer += game.getElapsedTime();
		if (timeTrial == true && timer >= timeLimit)
		{
			doGameOver(game);
		}
	}
}

void Level::setAnimationPosition(Animation& anim, PairFloat mapPos) const
{
	auto animSize = anim.Size();
	auto drawCoord = toDrawCoord(mapPos);
	drawCoord.x = drawCoord.x - animSize.x + tileSize;
	drawCoord.y = drawCoord.y - animSize.y + tileSize;
	anim.Position(drawCoord);
}

void Level::updateCurrentJewelsAnimation(Game& game)
{
	for (const auto& jewel : currentJewels)
	{
		jewel->update(game);
	}
}

void Level::addJewelClass(const std::string_view key, std::shared_ptr<JewelClass> obj)
{
	jewelsClasses.insert(std::make_pair(key, std::move(obj)));
}

const JewelClass* Level::getJewelClass(const std::string_view key) const
{
	auto it = jewelsClasses.find(sv2str(key));
	if (it != jewelsClasses.end())
	{
		return it->second.get();
	}
	return nullptr;
}

void Level::setOptions(std::vector<LevelOptions>& options_)
{
	allOptions = std::move(options_);
	std::sort(allOptions.begin(), allOptions.end(),
		[](const LevelOptions& a, const LevelOptions& b)
		{
			return a.level < b.level;
		}
	);
	for (auto& obj : allOptions)
	{
		obj.levelMax = std::min(obj.levelMax, 0xFFFF);
		if (obj.levelMax <= 0)
		{
			obj.levelMax = 0xFFFF;
		}
		obj.level = std::clamp(obj.level, 0, obj.levelMax);
		obj.jewelsMax = std::min(obj.jewelsMax, std::numeric_limits<int32_t>::max());
		if (obj.scoreMax <= 0)
		{
			obj.scoreMax = std::numeric_limits<int64_t>::max();
		}
		if (obj.scoreMin >= obj.scoreMax)
		{
			obj.scoreMin = std::numeric_limits<int64_t>::min();
		}
		obj.jewelsMax = std::min(obj.jewelsMax, std::numeric_limits<int32_t>::max());
		if (obj.jewelsMax <= 0)
		{
			obj.jewelsMax = std::numeric_limits<int32_t>::max();
		}
		obj.dropSteps = std::clamp(obj.dropSteps, 1u, 24u);
		obj.dropSpeed = sf::milliseconds(
			std::clamp(
				obj.dropSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
		obj.timeoutSpeed = sf::milliseconds(
			std::clamp(
				obj.timeoutSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
		obj.updateScoreSpeed = sf::milliseconds(
			std::clamp(
				obj.updateScoreSpeed.asMilliseconds(),
				sf::milliseconds(1).asMilliseconds(),
				sf::milliseconds(2000).asMilliseconds()
			)
		);
	}
}

bool Level::isMapCoordValid(PairFloat mapPos) const
{
	return (mapPos.x >= 0.f &&
		std::ceil(mapPos.x) < (float)mapSize.x &&
		mapPos.y >= 0.f &&
		std::ceil(mapPos.y) < (float)mapSize.y);
}

bool Level::hasJewel(PairFloat mapPos) const
{
	if (isMapCoordValid(mapPos) == true)
	{
		return get(mapPos).jewel != nullptr;
	}
	return false;
}

void Level::addJewel(const JewelClass* class_, PairFloat mapPos)
{
	if (class_ != nullptr)
	{
		addJewel(std::make_shared<Jewel>(class_), mapPos);
	}
}

void Level::addJewel(const std::shared_ptr<Jewel>& jewel)
{
	if (jewel != nullptr)
	{
		addJewel(jewel, jewel->MapPosition());
	}
}

void Level::addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat mapPos)
{
	if (isMapCoordValid(mapPos) == true)
	{
		auto& cell = get(mapPos);
		deleteJewel(cell.jewel);
		cell.jewel = jewel;
		if (jewel != nullptr)
		{
			jewel->MapPosition(mapPos);
			setAnimationPosition(*jewel, mapPos);
		}
	}
}

bool Level::deleteJewel(const std::shared_ptr<Jewel>& jewel)
{
	if (jewel != nullptr)
	{
		return deleteJewel(jewel->MapPosition());
	}
	return false;
}

bool Level::deleteJewel(PairFloat mapPos)
{
	if (isMapCoordValid(mapPos) == true)
	{
		auto& cell = get(mapPos);
		if (cell.jewel != nullptr)
		{
			cell.jewel.reset();
			return true;
		}
	}
	return false;
}

bool Level::moveJewel(PairFloat oldPos, PairFloat newPos)
{
	if (isMapCoordValid(oldPos) == true &&
		isMapCoordValid(newPos) == true)
	{
		auto& oldCell = get(oldPos);
		auto& newCell = get(newPos);
		if (oldCell.jewel != nullptr &&
			newCell.jewel == nullptr)
		{
			newCell.jewel = std::move(oldCell.jewel);
			newCell.jewel->MapPosition(newPos);
			setAnimationPosition(*newCell.jewel, newPos);
			return true;
		}
	}
	return false;
}

void Level::newGame(const std::string_view gameType_, sf::Time timeLimit_,
	int16_t numPlayers, int16_t height_, int32_t initialLevel)
{
	if (gameType_ == "flash")
	{
		flashGame = true;
		height_ = std::max(height_, (int16_t)2);
	}
	else
	{
		flashGame = false;
	}
	height_ = std::clamp(height_, (int16_t)0, (int16_t)(mapSize.y - 3));
	flashDestroyed = false;
	timer = {};
	timeLimit = timeLimit_ >= sf::Time::Zero ? timeLimit_ : sf::Time::Zero;
	timeTrial = timeLimit > sf::Time::Zero;
	numPlayers = std::clamp(numPlayers, (int16_t)1, (int16_t)2);

	cells.clear();

	int32_t numTiles = -1;
	if (tilesTexturePack != nullptr)
	{
		numTiles = (int32_t)tilesTexturePack->size();
		numTiles--;
	}
	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			LevelCell cell;
			if (numTiles >= 0)
			{
				TextureInfo ti;
				auto randomIdx = Utils::Random::get((uint32_t)numTiles);
				if (tilesTexturePack->get(randomIdx, ti) == true)
				{
					cell.tile.setTexture(ti);
				}
			}
			cell.tile.Position(toDrawCoord(x, y));
			cells.push_back(cell);
		}
	}

	selectionAnim.Visible(false);
	highlightAnim.Visible(false);

	gameState = LevelState::AddJewels;
	clearedJewels = false;
	magicJewelDestroyed = false;
	players = {};
	for (int16_t i = 0; i < numPlayers; i++)
	{
		players.push_back({});
		players[i].levelStart = initialLevel;
		updateLevelOptions(players[i]);
	}
	playerIdx = 0;
	height = 0;
	chain = 0;
	magicJewelClass = getJewelClass(magicJewel);
	currentJewels.clear();
	nextJewels.clear();

	addJewelRows(height_, flashGame);
}

void Level::updateLevelOptions(LevelPlayer& plr)
{
	bool hasOptions = false;
	for (const auto& opt : reverse(allOptions))
	{
		if (plr.getLevel() >= opt.level)
		{
			plr.options = opt;
			hasOptions = true;
			break;
		}
	}
	if (hasOptions == false)
	{
		plr.options = defaultOptions;
	}
	sleepTime.timeout = plr.options.timeoutSpeed;
	mapPosDropOffset = std::max(1.f / (float)plr.options.dropSteps, 0.01f);
}

void Level::addJewelRows(int16_t numRows, bool setFlashJewel)
{
	if (jewelsClasses.size() <= 1 ||
		Plr().options.jewels.size() <= 1)
	{
		return;
	}
	height = numRows;
	int16_t y = mapSize.y - 1;
	int16_t numTries = 100;
	while (numRows > 0 && numTries > 0)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			addJewel(makeRandomJewel(), PairFloat((float)x, (float)y));
		}
		bool hasMatch = false;
		checkBoard(0, y, mapSize.x, mapSize.y,
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
		auto& cell = get(flashPosition, mapSize.y - 1);
		cell.jewel->Flash(true);
	}
}

void Level::updateHeight()
{
	int16_t newHeight = 0;
	for (int16_t y = 0; y < mapSize.y; y++)
	{
		if (newHeight > 0)
		{
			break;
		}
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			if (hasJewel(PairFloat((float)x, (float)y)) == true)
			{
				newHeight = mapSize.y - y;
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
			auto idx = Utils::Random::get(Plr().options.jewels.size() - 1);
			const auto& classId = Plr().options.jewels[idx];
			auto jewelClass = getJewelClass(classId);
			if (jewelClass != nullptr)
			{
				return std::make_shared<Jewel>(jewelClass);
			}
		}
	}
	auto idx = Utils::Random::get(jewelsClasses.size() - 1);
	auto it = jewelsClasses.cbegin();
	for (size_t i = 0; i < idx; i++)
	{
		++it;
	}
	return std::make_shared<Jewel>(it->second.get());
}

std::vector<std::shared_ptr<Jewel>> Level::makeRandomJewels(int size_) const
{
	std::vector<std::shared_ptr<Jewel>> jewels;
	if (jewelsClasses.empty() == true)
	{
		return jewels;
	}
	PairFloat mapPos((float)startPositions[playerIdx], -1.f);
	while (size_ > 0)
	{
		auto jewel = makeRandomJewel();
		jewel->MapPosition(mapPos);
		setAnimationPosition(*jewel, mapPos);
		jewels.push_back(std::move(jewel));
		size_--;
		mapPos.y--;
	}
	return jewels;
}

std::vector<std::shared_ptr<Jewel>> Level::makeMagicJewels(int size_) const
{
	std::vector<std::shared_ptr<Jewel>> jewels;
	if (magicJewelClass == nullptr)
	{
		return jewels;
	}
	PairFloat mapPos((float)startPositions[playerIdx], -1.f);
	while (size_ > 0)
	{
		auto jewel = std::make_shared<Jewel>(magicJewelClass);
		jewel->MapPosition(mapPos);
		setAnimationPosition(*jewel, mapPos);
		jewels.push_back(std::move(jewel));
		size_--;
		mapPos.y--;
	}
	return jewels;
}

void Level::processInputEvents(Game& game)
{
	if (moveDownTime.updateAndReset(game.getElapsedTime()) == true)
	{
		blockMoveDown = false;
	}
	if (moveLeftTime.updateAndReset(game.getElapsedTime()) == true)
	{
		blockMoveLeft = false;
	}
	if (moveRightTime.updateAndReset(game.getElapsedTime()) == true)
	{
		blockMoveRight = false;
	}
	if (game.Resources().hasActiveInputEvents({ moveLeftEventHash }) == true)
	{
		if (blockMoveLeft == false)
		{
			moveSelectedJewels(game, { -1, 0 }, false);
			consecutiveLeftMoves++;
			blockMoveLeft = true;
			switch (consecutiveLeftMoves)
			{
			case 0:
			case 1:
				moveLeftTime.timeout = sf::milliseconds(180);
				break;
			case 2:
				moveLeftTime.timeout = sf::milliseconds(90);
				break;
			case 3:
				moveLeftTime.timeout = sf::milliseconds(30);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		moveLeftTime.timeout = sf::milliseconds(180);
		consecutiveLeftMoves = 0;
	}
	if (game.Resources().hasActiveInputEvents({ moveRightEventHash }) == true)
	{
		if (blockMoveRight == false)
		{
			moveSelectedJewels(game, { 1, 0 }, false);
			consecutiveRightMoves++;
			blockMoveRight = true;
			switch (consecutiveRightMoves)
			{
			case 0:
			case 1:
				moveRightTime.timeout = sf::milliseconds(180);
				break;
			case 2:
				moveRightTime.timeout = sf::milliseconds(90);
				break;
			case 3:
				moveRightTime.timeout = sf::milliseconds(30);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		moveRightTime.timeout = sf::milliseconds(180);
		consecutiveRightMoves = 0;
	}
	if (blockMoveDown == false &&
		game.Resources().hasActiveInputEvents({ moveDownEventHash }) == true)
	{
		moveSelectedJewels(game, { 0, 1 }, true);
		blockMoveDown = true;
	}
}

void Level::processStep(Game& game)
{
	switch (gameState)
	{
	case LevelState::NextRound:
	{
		if (players.size() > 0)
		{
			playerIdx = (playerIdx + 1) % players.size();
			updateLevelOptions(Plr());
		}
		gameState = LevelState::AddJewels;
		[[fallthrough]];
	}
	case LevelState::AddJewels:
	{
		if (chain > 0)
		{
			chain = 0;
		}
		if (currentJewels.empty() == true)
		{
			bool addedMagicJewel = false;
			if (Plr().magicJewelsUsed < Plr().options.magicJewels &&
				height >= Plr().options.magicHeight)
			{
				if (Utils::Random::get(4) == 0)
				{
					currentJewels = makeMagicJewels(3);
					Plr().magicJewelsUsed++;
					addedMagicJewel = true;
				}
			}
			if (addedMagicJewel == false)
			{
				currentJewels = makeRandomJewels(3);
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
		for (auto& jewel : currentJewels)
		{
			auto mapPos = jewel->MapPosition();
			mapPos.y += mapPosDropOffset;
			if (std::ceil(mapPos.y) < (float)mapSize.y)
			{
				if (hasJewel(mapPos) == false)
				{
					jewel->MapPosition(mapPos);
					setAnimationPosition(*jewel, mapPos);
					if (updateSelected == false)
					{
						setAnimationPosition(selectionAnim, mapPos);
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
			if (isMapCoordValid(currentJewels.back()->MapPosition()) == false)
			{
				doGameOver(game);
				break;
			}
			for (auto& jewel : currentJewels)
			{
				auto mapPos = jewel->MapPosition();
				if (isMapCoordValid(mapPos) == true)
				{
					get(mapPos).jewel = std::move(jewel);
				}
			}
			currentJewels.clear();
			updateHeight();
			gameState = LevelState::ClearJewels;
		}
		else
		{
			checkIfTouchedBase(game);
		}
		break;
	}
	case LevelState::ClearJewels:
	{
		selectionAnim.Visible(false);
		highlightAnim.Visible(false);
		highlightPositions.clear();

		auto func = std::bind(&Level::setDeleteAndUpdatePoints, this, std::placeholders::_1);
		checkBoard(0, 0, mapSize.x, mapSize.y, func);
		if (Plr().jewelsDestroyed > 0)
		{
			clearedJewels = false;
			gameState = LevelState::ClearingJewels;
			chain++;
			Plr().scoreRound = (int32_t)scoreFormula.eval(*this);
			sleepTime.timeout = Plr().options.updateScoreSpeed;
		}
		else
		{
			gameState = LevelState::NextRound;
			sleepTime.timeout = Plr().options.timeoutSpeed;
		}
		break;
	}
	case LevelState::ClearingJewels:
	{
		gameState = LevelState::ClearedJewels;
		sleepTime.timeout = sf::milliseconds(1);
		if (destroyAction != nullptr)
		{
			game.Events().addBack(destroyAction);
		}
		updateScore(game);
		break;
	}
	case LevelState::ClearedJewels:
	{
		if (clearedJewels == true)
		{
			magicJewelDestroyed = false;
			if (flashDestroyed == true)
			{
				doGameOver(game);
			}
			else
			{
				gameState = LevelState::ClearJewels;
			}
			sleepTime.timeout = Plr().options.timeoutSpeed;
			Plr().scoreRound = 0;
			Plr().jewelsDestroyed = 0;
			Plr().jewelsPoints = 0;
			for (int16_t y = 0; y < mapSize.y; y++)
			{
				for (int16_t x = 0; x < mapSize.x; x++)
				{
					deleteJewelAndShift(PairFloat((float)x, (float)y));
				}
			}
			updateHeight();
		}
		break;
	}
	case LevelState::GameOver:
	default:
		break;
	}
}

void Level::doGameOver(Game& game)
{
	gameState = LevelState::GameOver;
	if (gameOverAction != nullptr)
	{
		game.Events().addBack(gameOverAction);
	}
}

void Level::doHints()
{
	std::vector<const JewelClass*> checkClasses;
	for (const auto& jewel : currentJewels)
	{
		auto cls = jewel->Class();
		if (std::find(checkClasses.cbegin(), checkClasses.cend(), cls) == checkClasses.cend())
		{
			checkClasses.push_back(cls);
		}
	}
	for (int16_t x = 0; x < mapSize.x; x++)
	{
		auto height = (int16_t)currentJewels.size() - 1;
		for (int16_t y = mapSize.y - 1; y >= 0 && height >= 0; y--)
		{
			PairFloat mapPos(x, y);
			if (get(mapPos).jewel != nullptr)
			{
				continue;
			}
			height--;
			for (auto cls : checkClasses)
			{
				// horizontal 1
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y);
					PairFloat mapPosB(mapPos.x + 1.f, mapPos.y);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// horizontal 2
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y);
					PairFloat mapPosB(mapPos.x - 2.f, mapPos.y);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// horizontal 3
				{
					PairFloat mapPosA(mapPos.x + 1.f, mapPos.y);
					PairFloat mapPosB(mapPos.x + 2.f, mapPos.y);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// vertical
				{
					PairFloat mapPosA(mapPos.x, mapPos.y + 1.f);
					PairFloat mapPosB(mapPos.x, mapPos.y + 2.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 1
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y - 1.f);
					PairFloat mapPosB(mapPos.x + 1.f, mapPos.y + 1.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 1 - 2
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y - 1.f);
					PairFloat mapPosB(mapPos.x - 2.f, mapPos.y - 2.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 1 - 3
				{
					PairFloat mapPosA(mapPos.x + 1.f, mapPos.y + 1.f);
					PairFloat mapPosB(mapPos.x + 2.f, mapPos.y + 2.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 2 - 1
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y + 1.f);
					PairFloat mapPosB(mapPos.x + 1.f, mapPos.y - 1.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 2 - 2
				{
					PairFloat mapPosA(mapPos.x - 1.f, mapPos.y + 1.f);
					PairFloat mapPosB(mapPos.x - 2.f, mapPos.y + 2.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}

				// diagonal 2 - 3
				{
					PairFloat mapPosA(mapPos.x + 1.f, mapPos.y - 1.f);
					PairFloat mapPosB(mapPos.x + 2.f, mapPos.y - 2.f);
					if (isMapCoordValid(mapPosA) == true &&
						isMapCoordValid(mapPosB) == true)
					{
						const auto& cellA = get(mapPosA);
						const auto& cellB = get(mapPosB);
						if (cellA.jewel != nullptr &&
							cellB.jewel != nullptr &&
							cellA.jewel->Class() == cls &&
							cellB.jewel->Class() == cls)
						{
							highlightPositions.push_back(mapPosA);
							highlightPositions.push_back(mapPosB);
						}
					}
				}
			}
		}
	}
	highlightAnim.reset();
	highlightAnim.Visible(true);
}

void Level::checkBoard(int16_t startX, int16_t startY, int16_t stopX, int16_t stopY,
	const std::function<bool(Jewel&)> func)
{
	for (int16_t y = stopY - 1; y >= startY; y--)
	{
		for (int16_t x = startX; x < stopX; x++)
		{
			PairFloat mapPos(x, y);
			const auto& cell = get(mapPos);
			if (cell.jewel == nullptr)
			{
				continue;
			}

			// magic
			auto cellClass = cell.jewel->Class();
			if (cellClass == magicJewelClass)
			{
				PairFloat mapPosA(mapPos.x, mapPos.y + 1.f);
				if (isMapCoordValid(mapPosA) == true)
				{
					const auto& cellA = get(mapPosA);
					if (cellA.jewel != nullptr)
					{
						const auto& cellAClass = cellA.jewel->Class();
						if (cellAClass != magicJewelClass)
						{
							for (int16_t y2 = startY; y2 < stopY; y2++)
							{
								for (int16_t x2 = startX; x2 < stopX; x2++)
								{
									PairFloat mapPosB(x2, y2);
									const auto& cellB = get(mapPosB);
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
				PairFloat mapPosA(mapPos.x - 1.f, mapPos.y);
				PairFloat mapPosB(mapPos.x + 1.f, mapPos.y);
				if (isMapCoordValid(mapPosA) == true &&
					isMapCoordValid(mapPosB) == true)
				{
					const auto& cellA = get(mapPosA);
					const auto& cellB = get(mapPosB);
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
				PairFloat mapPosA(mapPos.x, mapPos.y - 1.f);
				PairFloat mapPosB(mapPos.x, mapPos.y + 1.f);
				if (isMapCoordValid(mapPosA) == true &&
					isMapCoordValid(mapPosB) == true)
				{
					const auto& cellA = get(mapPosA);
					const auto& cellB = get(mapPosB);
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
				PairFloat mapPosA(mapPos.x - 1.f, mapPos.y - 1.f);
				PairFloat mapPosB(mapPos.x + 1.f, mapPos.y + 1.f);
				if (isMapCoordValid(mapPosA) == true &&
					isMapCoordValid(mapPosB) == true)
				{
					const auto& cellA = get(mapPosA);
					const auto& cellB = get(mapPosB);
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
				PairFloat mapPosA(mapPos.x - 1.f, mapPos.y + 1.f);
				PairFloat mapPosB(mapPos.x + 1.f, mapPos.y - 1.f);
				if (isMapCoordValid(mapPosA) == true &&
					isMapCoordValid(mapPosB) == true)
				{
					const auto& cellA = get(mapPosA);
					const auto& cellB = get(mapPosB);
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

bool Level::setDeleteAndUpdatePoints(Jewel& jewel)
{
	if (jewel.Delete() == true)
	{
		Plr().jewelsDestroyed++;
		Plr().jewelsPoints += jewel.Points();
	}
	if (flashGame == true && jewel.Flash() == true)
	{
		flashDestroyed = true;
	}
	if (magicJewelClass != nullptr &&
		magicJewelClass == jewel.Class())
	{
		magicJewelDestroyed = true;
	}
	return true;
}

bool Level::checkIfTouchedBase()
{
	if (currentJewels.empty() == true)
	{
		return false;
	}
	sleepTime.timeout = Plr().options.dropSpeed;
	auto mapPos = currentJewels.front()->MapPosition();
	mapPos.y += mapPosDropOffset;
	return (std::ceil(mapPos.y) >= (float)mapSize.y || hasJewel(mapPos) == true);
}

bool Level::checkIfTouchedBase(Game& game)
{
	if (checkIfTouchedBase() == true)
	{
		sleepTime.timeout = Plr().options.timeoutSpeed;
		if (touchBaseAction != nullptr)
		{
			game.Events().addBack(touchBaseAction);
		}
		return true;
	}
	return false;
}

void Level::deleteJewelAndShift(PairFloat mapPos)
{
	if (hasJewel(mapPos) == false)
	{
		return;
	}
	const auto& cell = get(mapPos);
	if (cell.jewel == nullptr ||
		cell.jewel->Deleted() == false)
	{
		return;
	}
	if (deleteJewel(mapPos) == true)
	{
		PairFloat mapPosA;
		PairFloat mapPosB = mapPos;
		do
		{
			mapPosA = mapPosB;
			mapPosB.y--;
		} while (moveJewel(mapPosB, mapPosA) == true);
	}
}

void Level::rotateSelectedJewels(Game& game, bool reverse)
{
	if (currentJewels.empty() == true ||
		gameState == LevelState::GameOver ||
		pause == true)
	{
		return;
	}
	auto mapPos = currentJewels.front()->MapPosition();
	if (reverse == false)
	{
		std::rotate(currentJewels.begin(), currentJewels.begin() + 1, currentJewels.end());
	}
	else
	{
		std::rotate(currentJewels.begin(), currentJewels.end() - 1, currentJewels.end());
	}
	for (auto& jewel : currentJewels)
	{
		jewel->MapPosition(mapPos);
		setAnimationPosition(*jewel, mapPos);
		mapPos.y--;
	}
	if (rotateAction != nullptr)
	{
		game.Events().addBack(rotateAction);
	}
}

void Level::moveSelectedJewels(Game& game, PairInt16 mapPosOffset, bool addDropPoints)
{
	if (currentJewels.empty() == true ||
		gameState == LevelState::GameOver ||
		pause == true)
	{
		return;
	}
	auto mapPos = currentJewels.front()->MapPosition();
	auto newMapPos = mapPos;
	newMapPos.x += (float)mapPosOffset.x;
	newMapPos.y += (float)mapPosOffset.y * mapPosDropOffset;
	auto checkMapPos = newMapPos;
	if (checkMapPos.x < 0.f ||
		std::ceil(checkMapPos.x) >= (float)mapSize.x ||
		checkMapPos.y < -1.f ||
		std::ceil(checkMapPos.y) >= (float)mapSize.y ||
		hasJewel(checkMapPos) == true)
	{
		return;
	}
	if (addDropPoints == true && mapPosOffset.y > 0)
	{
		Plr().score += (int32_t)(Plr().options.dropPoints * mapPosOffset.y);
	}
	setAnimationPosition(selectionAnim, newMapPos);
	selectionAnim.Visible(true);
	for (auto& jewel : currentJewels)
	{
		jewel->MapPosition(newMapPos);
		setAnimationPosition(*jewel, newMapPos);
		newMapPos.y--;
	}
	if (checkIfTouchedBase(game) == true)
	{
		for (auto& jewel : currentJewels)
		{
			setAnimationPosition(*jewel, jewel->MapPosition());
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
		updateLevelOptions(Plr());
		if (levelUpAction != nullptr)
		{
			game.Events().addBack(levelUpAction);
		}
	}
}

void Level::addHighScore(const LevelScore& levelScore)
{
	highScores.push_back(levelScore);
	updateHighScores();
}

void Level::setHighScores(std::vector<LevelScore>& highScores_)
{
	highScores = std::move(highScores_);
	updateHighScores();
}

void Level::updateHighScores()
{
	std::sort(highScores.begin(), highScores.end(),
		[](const LevelScore& a, const LevelScore& b)
		{
			if (a.time > b.time)
			{
				return true;
			}
			else if(a.score > b.score)
			{
				return true;
			}
			else if (a.level > b.level)
			{
				return true;
			}
			else if (a.jewels > b.jewels)
			{
				return true;
			}
			return false;
		}
	);
	if (highScores.size() > 10)
	{
		highScores.resize(10);
	}
}
