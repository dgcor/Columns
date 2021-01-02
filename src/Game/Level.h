#pragma once

#include "Animation.h"
#include <array>
#include "Formula.h"
#include <functional>
#include "Jewel.h"
#include "LevelSurface.h"
#include <memory>
#include "Save/SaveLevel.h"
#include "SFML/Sprite2.h"
#include "SFML/View2.h"
#include "TexturePacks/TexturePack.h"
#include "UIObject.h"
#include "Utils/FixedArray.h"
#include "Utils/PairXY.h"
#include "Utils/UnorderedStringMap.h"
#include <vector>

struct LevelOptions
{
	int32_t level{ 0 };
	int32_t levelMax{ 199 };
	int64_t scoreMin{ 0 };
	int64_t scoreMax{ 99999999 };
	int32_t jewelsMax{ 9999 };
	uint32_t dropSteps{ 2 };
	int16_t dropPoints{ 1 };
	int16_t magicJewels{ 0 };
	int16_t magicHeight{ 0 };
	bool hints{ false };
	sf::Time dropSpeed{ sf::milliseconds(250) };
	sf::Time timeoutSpeed{ sf::milliseconds(500) };
	sf::Time updateScoreSpeed{ sf::milliseconds(400) };
	std::vector<std::string> jewels;
};

struct LevelScore
{
	std::string name;
	int32_t level{ 0 };
	int64_t score{ 0 };
	int32_t jewels{ 0 };
	sf::Time time;
};

class Level : public UIObject
{
private:
	struct LevelCell
	{
		Image tile;
		std::shared_ptr<Jewel> jewel;
	};

	struct LevelPlayer
	{
		int32_t levelStart{ 0 };
		int32_t levelUp{ 0 };
		int64_t score{ 0 };
		int32_t scoreRound{ 0 };
		int32_t jewels{ 0 };
		int32_t jewelsDestroyed{ 0 };
		int32_t jewelsPoints{ 0 };
		int32_t magicJewelsUsed{ 0 };
		LevelOptions options;

		int32_t getLevel() const noexcept { return levelStart + levelUp; }
	};

	enum class LevelState
	{
		NextRound,
		AddJewels,
		ClearJewels,
		ClearingJewels,
		ClearedJewels,
		GameOver
	};

	LevelSurface surface;
	GameShader* gameShader{ nullptr };

	std::vector<LevelCell> cells;
	PairInt16 mapSize;

	Animation selectionAnim;
	mutable Animation highlightAnim;
	std::vector<PairFloat> highlightPositions;

	std::shared_ptr<TexturePack> tilesTexturePack;
	std::shared_ptr<TexturePack> selectTexturePack;
	std::shared_ptr<TexturePack> highlightTexturePack;

	float tileSize{ 0 };

	std::string levelId;
	std::string name;

	bool pause{ false };
	bool hasMouseInside{ false };

	bool flashGame{ false };
	bool flashDestroyed{ false };
	sf::Time timer;
	sf::Time timeLimit;
	bool timeTrial{ false };

	LevelState gameState{ LevelState::GameOver };
	bool clearedJewels{ false };
	bool magicJewelDestroyed{ false };
	float mapPosDropOffset{ 0 };

	FixedArray<LevelPlayer, 2> players;
	uint32_t playerIdx{ 0 };
	std::array<int16_t, 2> startPositions;
	int16_t flashPosition;
	int16_t height{ 0 };
	int32_t chain{ 0 };
	Formula scoreFormula;
	Formula levelUpFormula;
	std::string magicJewel;
	const JewelClass* magicJewelClass{ nullptr };

	std::vector<std::shared_ptr<Jewel>> currentJewels;
	std::vector<std::shared_ptr<Jewel>> nextJewels;

	ElapsedTime sleepTime;
	ElapsedTime moveDownTime{ sf::milliseconds(1) };
	ElapsedTime moveLeftTime{ sf::milliseconds(180) };
	ElapsedTime moveRightTime{ sf::milliseconds(180) };
	bool blockMoveDown{ false };
	bool blockMoveLeft{ false };
	bool blockMoveRight{ false };
	int16_t consecutiveLeftMoves{ 0 };
	int16_t consecutiveRightMoves{ 0 };

	std::vector<LevelOptions> allOptions;

	std::shared_ptr<Action> levelUpAction;
	std::shared_ptr<Action> rotateAction;
	std::shared_ptr<Action> touchBaseAction;
	std::shared_ptr<Action> destroyAction;
	std::shared_ptr<Action> gameOverAction;

	UnorderedStringMap<std::shared_ptr<JewelClass>> jewelsClasses;

	std::vector<LevelScore> highScores;

	uint16_t moveDownEventHash{ 0 };
	uint16_t moveLeftEventHash{ 0 };
	uint16_t moveRightEventHash{ 0 };

	friend void Save::save(const std::string_view filePath,
		const Save::Properties& props, const Game& game, const Level& level);
	friend void Save::serialize(void* serializeObj,
		const Save::Properties& props, const Game& game, const Level& level);

	sf::Vector2f toDrawCoord(int16_t x, int16_t y) const;
	sf::Vector2f toDrawCoord(float x, float y) const;
	sf::Vector2f toDrawCoord(PairFloat mapPos) const;

	const LevelCell& get(int16_t x, int16_t y) const;
	LevelCell& get(int16_t x, int16_t y);
	const LevelCell& get(float x, float y) const;
	LevelCell& get(float x, float y);
	const LevelCell& get(PairFloat mapPos) const;
	LevelCell& get(PairFloat mapPos);

	LevelPlayer& Plr() noexcept { return players[playerIdx]; }
	const LevelPlayer& Plr() const noexcept { return players[playerIdx]; }

	void setPlayerStartPosition(size_t idx, int16_t startPos_);

	sf::Time getTimeLeft() const noexcept;
	void updateTimer(Game& game);

	void setAnimationPosition(Animation& anim, PairFloat mapPos) const;
	void updateCurrentJewelsAnimation(Game& game);

	std::shared_ptr<Jewel> makeRandomJewel() const;
	std::vector<std::shared_ptr<Jewel>> makeRandomJewels(int size_) const;
	std::vector<std::shared_ptr<Jewel>> makeMagicJewels(int size_) const;
	void updateLevelOptions(LevelPlayer& plr);
	void addJewelRows(int16_t numRows, bool setFlashJewel);
	void updateHeight();
	void processInputEvents(Game& game);
	void processStep(Game& game);
	void doGameOver(Game& game);
	void doHints();
	void checkBoard(int16_t startX, int16_t startY, int16_t stopX, int16_t stopY,
		const std::function<bool(Jewel&)> func = {});
	bool setDeleteAndUpdatePoints(Jewel& jewel);
	bool checkIfTouchedBase();
	bool checkIfTouchedBase(Game& game);
	void deleteJewelAndShift(PairFloat mapPos);
	void updateScore(Game& game);
	bool getStatsProperty(uint32_t idx, const std::string_view prop, Variable& var) const;
	bool getHighScoreProperty(const std::string_view prop, Variable& var) const;
	void updateHighScores();

public:
	Level(Game& game, const std::shared_ptr<TexturePack>& tilesTexturePack_,
		const std::shared_ptr<TexturePack>& selectTexturePack_,
		const std::shared_ptr<TexturePack>& highlightTexturePack_,
		PairInt16 mapSize_, float tileSize_);

	void setShader(GameShader* shader) noexcept { gameShader = shader; }

	void Id(const std::string_view id_);
	void Name(const std::string_view name_) { name = name_; }

	std::shared_ptr<Action> getAction(uint16_t nameHash16) const noexcept override;
	bool setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept override;

	Anchor getAnchor() const noexcept override { return surface.getAnchor(); }
	void setAnchor(const Anchor anchor) noexcept override { surface.setAnchor(anchor); }

	void updateSize(const Game& game) override { surface.updateSize(game); }

	const sf::Vector2f& DrawPosition() const noexcept override { return surface.Position(); }
	const sf::Vector2f& Position() const noexcept override { return surface.Position(); }
	void Position(const sf::Vector2f& position) override { surface.Position(position); }
	sf::Vector2f Size() const noexcept override { return surface.Size(); }
	void Size(const sf::Vector2f& size) override {}

	bool Pause() const noexcept { return pause; }
	void Pause(bool pause_) noexcept { pause = pause_; }

	bool Visible() const noexcept override { return surface.visible; }
	void Visible(bool visible) noexcept override { surface.visible = visible; }

	void save(const std::string_view filePath,
		const Save::Properties& props, const Game& game) const
	{
		Save::save(filePath, props, game, *this);
	}
	void serialize(void* serializeObj, const Save::Properties& props,
		const Game& game, const Level& level)
	{
		Save::serialize(serializeObj, props, game, *this);
	}

	void draw(const Game& game, sf::RenderTarget& target) const override;
	void update(Game& game) override;
	bool getProperty(const std::string_view prop, Variable& var) const override;
	const Queryable* getQueryable(const std::string_view prop) const override;

	std::vector<std::variant<const Queryable*, Variable>> getQueryableList(
		const std::string_view prop) const;

	// game related

	const PairInt16& MapSize() const noexcept { return mapSize; }

	void setPlayer1StartPosition(int16_t startPos_) { setPlayerStartPosition(0, startPos_); }
	void setPlayer2StartPosition(int16_t startPos_) { setPlayerStartPosition(1, startPos_); }

	void setFlashPosition(int16_t flashPos_) { flashPosition = flashPos_; }

	void addJewelClass(const std::string_view key, std::shared_ptr<JewelClass> obj);
	const JewelClass* getJewelClass(const std::string_view key) const;

	void setScoreFormula(const std::string_view str) { scoreFormula = str; }
	void setLevelUpFormula(const std::string_view str) { levelUpFormula = str; }
	void setMagicJewel(const std::string_view str) { magicJewel = str; }

	void setOptions(std::vector<LevelOptions>& options_);

	bool isMapCoordValid(PairFloat mapPos) const;

	bool hasJewel(PairFloat mapPos) const;
	void addJewel(const JewelClass* class_, PairFloat mapPos);
	void addJewel(const std::shared_ptr<Jewel>& jewel);
	void addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat mapPos);
	bool deleteJewel(const std::shared_ptr<Jewel>& jewel);
	bool deleteJewel(PairFloat mapPos);
	bool moveJewel(PairFloat oldPos, PairFloat newPos);

	void newGame(const std::string_view gameType_, sf::Time timeLimit_,
		int16_t numPlayers, int16_t height_, int32_t initialLevel);

	void rotateSelectedJewels(Game& game, bool reverse);
	void moveSelectedJewels(Game& game, PairInt16 mapPosOffset, bool addDropPoints);

	void addHighScore(const LevelScore& levelScore);
	void setHighScores(std::vector<LevelScore>& highScores_);
};
