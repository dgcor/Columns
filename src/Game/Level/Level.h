#pragma once

#include "Game/ActionQueryable.h"
#include "Game/Drawables/Animation.h"
#include "Game/Formula.h"
#include "LevelBoard.h"
#include "LevelInputManager.h"
#include "LevelOptionsManager.h"
#include "LevelPlayerManager.h"
#include "LevelScoreManager.h"
#include "LevelStateObject.h"
#include "SFML/Surface.h"
#include "Resources/TexturePack.h"

class Level : public ActionQueryable, public UIObject
{
private:
	friend class LevelDraw;
	friend class LevelSave;
	friend class LevelUIObject;

	Surface surface;
	GameShader* gameShader{ nullptr };

	LevelBoard board;

	Animation selectionAnim;
	mutable Animation highlightAnim;
	std::vector<PairFloat> highlightPositions;

	std::shared_ptr<TexturePack> tilesTexturePack;
	std::shared_ptr<TexturePack> selectTexturePack;
	std::shared_ptr<TexturePack> highlightTexturePack;

	std::string levelId;
	std::string name;

	bool pause{ false };
	bool hasMouseInside{ false };

	LevelStateObject gameState;

	LevelPlayerManager playerManager;

	bool clearedJewels{ false };
	bool flashDestroyed{ false };
	bool magicJewelDestroyed{ false };
	float boardPosDropOffset{ 0 };

	int16_t flashPosition{ 0 };
	int16_t height{ 0 };
	Formula scoreFormula;
	Formula levelUpFormula;
	std::string magicJewel;
	const JewelClass* magicJewelClass{ nullptr };

	ElapsedTime sleepTime;

	LevelInputManager inputManager;
	LevelOptionsManager optionsManager;

	std::shared_ptr<Action> levelUpAction;
	std::shared_ptr<Action> rotateAction;
	std::shared_ptr<Action> touchBaseAction;
	std::shared_ptr<Action> destroyAction;
	std::shared_ptr<Action> gameOverAction;

	UnorderedStringMap<std::shared_ptr<JewelClass>> jewelsClasses;

	LevelScoreManager highScoresManager;

	auto& Plr() noexcept { return playerManager.Plr(); }
	const auto& Plr() const noexcept { return playerManager.Plr(); }

	void setPlayerStartPosition(uint32_t idx, int16_t startPos_);

	void updateTimer(Game& game);

	void initGame();
	std::shared_ptr<Jewel> makeRandomJewel() const;
	Jewels makeRandomJewels(int size_) const;
	Jewels makeMagicJewels(int size_) const;
	void updateLevelOptions();
	void addJewelRows(int16_t numRows, bool setFlashJewel);
	void updateHeight();
	void processStep(Game& game);
	void onNextRound();
	void onAddJewels(Game& game);
	void onClearJewels(bool updateStats);
	void onClearingJewels(Game& game);
	void onClearedJewels(Game& game);
	void doGameOver(Game& game);
	void doHints();
	bool setDelete(Jewel& jewel);
	bool setDeleteAndUpdatePoints(Jewel& jewel);
	void updateLogic(Jewel& jewel);
	bool checkIfTouchedBase();
	bool checkIfTouchedBase(Game& game);
	void deleteJewelAndShift(PairFloat boardPos);
	void updateScore(Game& game);

public:
	Level(Game& game, const std::shared_ptr<TexturePack>& tilesTexturePack_,
		const std::shared_ptr<TexturePack>& selectTexturePack_,
		const std::shared_ptr<TexturePack>& highlightTexturePack_,
		PairInt16 boardSize_, float tileSize_);

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

	void draw(const Game& game, sf::RenderTarget& target) const override;
	void update(Game& game) override;
	bool getProperty(const std::string_view prop, Variable& var) const override;
	QueryObject getQueryable(const std::string_view prop) const override;

	// game related

	auto& Board() const noexcept { return board; }

	auto& Options() noexcept { return optionsManager; }
	auto& Options() const noexcept { return optionsManager; }

	auto& HighScores() noexcept { return highScoresManager; }
	auto& HighScores() const noexcept { return highScoresManager; }

	auto& MagicJewel() const noexcept { return magicJewel; }
	auto MagicJewelClass() const noexcept { return magicJewelClass; }
	auto& CurrentJewels() const noexcept { return gameState.currentJewels; }
	auto& NextJewels() const noexcept { return gameState.nextJewels; }

	void setPlayer1StartPosition(int16_t startPos_) { setPlayerStartPosition(0, startPos_); }
	void setPlayer2StartPosition(int16_t startPos_) { setPlayerStartPosition(1, startPos_); }

	void setFlashPosition(int16_t flashPos_) { flashPosition = flashPos_; }

	void addJewelClass(const std::string_view key, std::shared_ptr<JewelClass> obj);
	const JewelClass* getJewelClass(const std::string_view key) const;

	void setScoreFormula(const std::string_view str) { scoreFormula = str; }
	void setLevelUpFormula(const std::string_view str) { levelUpFormula = str; }
	void setMagicJewel(const std::string_view str) { magicJewel = str; }

	void addJewel(const JewelClass* class_, PairFloat boardPos);
	void addJewel(const std::shared_ptr<Jewel>& jewel);
	void addJewel(const std::shared_ptr<Jewel>& jewel, PairFloat boardPos);
	bool moveJewel(PairFloat oldPos, PairFloat newPos);

	void newGame(const std::string_view gameType_, sf::Time timeLimit_,
		int16_t numPlayers, int16_t height_, int32_t initialLevel);
	void loadGame(Game& game, const std::string_view filePath);
	void saveGame(const std::string_view filePath) const;

	void rotateSelectedJewels(Game& game, bool reverse);
	void moveSelectedJewels(Game& game, PairInt16 boardPosOffset, bool addDropPoints);
};
