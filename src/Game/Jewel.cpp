#include "Jewel.h"
#include "Game.h"
#include "Level.h"
#include "Utils/Utils.h"

Jewel::Jewel(const JewelClass* class__, PairFloat mapPosition_) noexcept
	: class_(class__), mapPosition(mapPosition_)
{
	updateAnimation();
	points = class_->Points();
}

void Jewel::updateAnimation()
{
	std::shared_ptr<TexturePack> texturePack;
	AnimationInfo animInfo;
	if (deleted == false)
	{
		texturePack = class_->getTexturePack();
		animInfo = texturePack->getAnimation(flash ? 1 : 0, 0);
	}
	else
	{
		texturePack = class_->getTexturePackDestroy();
		animInfo = texturePack->getAnimation(0, 0);
		animInfo.animType = AnimationType::PlayOnce;
	}
	if (animInfo.refresh == sf::Time::Zero)
	{
		animInfo.refresh = sf::milliseconds(50);
	}
	setAnimation(texturePack, animInfo);
}

void Jewel::Flash(bool flash_)
{
	if (flash == flash_)
	{
		return;
	}
	flash = flash_;
	updateAnimation();
}

bool Jewel::Delete()
{
	if (deleted == false)
	{
		deleted = true;
		updateAnimation();
		return true;
	}
	return false;
}

bool Jewel::getProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("points"):
		var = Variable((int64_t)points);
		break;
	case str2int16("flash"):
		var = Variable(flash);
		break;
	case str2int16("mapPosition"):
	{
		if (props.second == "x")
		{
			var = Variable((int64_t)mapPosition.x);
		}
		else
		{
			var = Variable((int64_t)mapPosition.y);
		}
		break;
	}
	default:
		return Animation::getProperty(prop, var);
	}
	return true;
}

bool Jewel::getTexture(uint32_t textureNumber, TextureInfo& ti) const
{
	return class_->getTexturePack()->get(textureNumber, ti);
}
