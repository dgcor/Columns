#pragma once

#include "Resources/TexturePack.h"
#include <string>

class JewelClass
{
private:
	std::shared_ptr<TexturePack> texturePack;
	std::shared_ptr<TexturePack> texturePackDestroy;

	std::string id;
	std::string type;
	int16_t points{ 0 };

public:
	JewelClass(const std::shared_ptr<TexturePack>& texturePack_,
		const std::shared_ptr<TexturePack>& texturePackDestroy_)
		: texturePack(texturePack_), texturePackDestroy(texturePackDestroy_) {}

	auto& getTexturePack() const noexcept { return texturePack; }
	auto& getTexturePackDestroy() const noexcept { return texturePackDestroy; }

	auto& Id() const noexcept { return id; }
	auto& Type() const noexcept { return type; }
	auto Points() const noexcept { return points; }

	void Id(const std::string_view id_) { id = id_; }
	void Type(const std::string_view type_) { type = type_; }
	void Points(int16_t val) noexcept { points = val; }
};
