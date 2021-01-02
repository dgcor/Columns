#pragma once

#include "Animation.h"
#include "JewelClass.h"
#include "Utils/PairXY.h"

class Level;

class Jewel : public Animation
{
private:
	const JewelClass* class_;
	PairFloat mapPosition;
	int16_t points{ 0 };
	bool flash{ false };
	bool deleted{ false };

	void updateAnimation();

public:
	Jewel(const JewelClass* class__) noexcept : Jewel(class__, { -1.f, -1.f }) {}
	Jewel(const JewelClass* class__, PairFloat mapPosition_) noexcept;

	const JewelClass* Class() const noexcept { return class_; }
	const PairFloat& MapPosition() const noexcept { return mapPosition; }
	int16_t Points() const noexcept { return points; }
	bool Flash() const noexcept { return flash; }
	bool Deleted() const noexcept { return deleted; }

	void MapPosition(PairFloat mapPos) noexcept { mapPosition = mapPos; }
	void Points(int32_t points_) noexcept { points = points_; }
	void Flash(bool flash_);
	bool Delete();

	bool getProperty(const std::string_view prop, Variable& var) const override;
	bool getTexture(uint32_t textureNumber, TextureInfo& ti) const override;
};
